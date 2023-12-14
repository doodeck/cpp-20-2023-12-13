#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

struct Rating
{
    int value;

    bool operator==(const Rating& other) const
    {
        std::cout << "Rating::op==()\n";
        return value == other.value;
    }

    bool operator<(const Rating& other) const
    {
        std::cout << "Rating::op<()\n";
        return value < other.value;
    }
};

struct Gadget
{
    std::string name;
    double price;

    bool operator==(const Gadget& other) const = default;
    
    std::strong_ordering operator<=>(const Gadget& other) const
    {
        if (auto nameResult = name <=> other.name; nameResult !=0)
        {
            return nameResult;
        }
        else
        {
            return std::strong_order(price, other.price);
        }
    }
};

struct SuperGadget : Gadget
{
    Rating rating;

    std::strong_ordering operator<=>(const SuperGadget& other) const = default;

    // bool operator==(const SuperGadget& other) const = default;

    // auto operator<=>(const SuperGadget& other) const
    // {
    //     if (const auto& ratingResult = std::compare_strong_order_fallback(rating, other.rating); ratingResult !=0)
    //     {
    //         return ratingResult;
    //     }
    //     else
    //     {
    //         return std::strong_order(price, other.price);
    //     }
    // }
};

TEST_CASE("Gadget - write custom operator <=> - stronger category than auto detected")
{
    SECTION("==")
    {
        CHECK(Gadget{"ipad", 1.0} == Gadget{"ipad", 1.0});
    }

    SECTION("<=>")
    {
        auto result = Gadget{"ipad", 1.0} <=> Gadget{"ipad", 1.0};
        static_assert(std::is_same_v<decltype(result), std::strong_ordering>);
        CHECK(Gadget{"ipad", 1.0} <=> Gadget{"ipad", 1.0} == std::strong_ordering::equal);
    }
}

TEST_CASE("SuperGadget - write custom operator <=> - member without compare-three-way operator")
{
    SECTION("==")
    {
        CHECK(SuperGadget{{"ipad", 1.0}, Rating{1}} != SuperGadget{{"ipad", 1.0}, Rating{2}});
    }

    SECTION("<=>")
    {
        auto result = SuperGadget{{"ipad", 1.0}, Rating{1}} <=> SuperGadget{{"ipad", 1.0}, Rating{2}};
        static_assert(std::is_same_v<decltype(result), std::strong_ordering>);
        CHECK(SuperGadget{{"ipad", 1.0}, Rating{1}} <=> SuperGadget{{"ipad", 1.0}, Rating{2}} == std::strong_ordering::less);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

enum class RatingValue : uint8_t {
    very_poor = 1,
    poor,
    satisfactory,
    good,
    very_good,
    excellent
};

struct RatingStar
{
public:
    RatingValue value;

    explicit RatingStar(RatingValue rating_value)
        : value{rating_value}
    { }

    std::strong_ordering operator<=>(const RatingStar& other) const = default;

    std::strong_ordering operator<=>(const RatingValue& other) const 
    {
        return value <=> other;
    }
};

TEST_CASE("Rating Star - implement needed <=>")
{
    RatingStar r1{RatingValue::good};

    CHECK(r1 == RatingStar{RatingValue::good});
    CHECK(r1 <=> RatingStar{RatingValue::excellent} == std::strong_ordering::less);
    CHECK(r1 <=> RatingValue::excellent == std::strong_ordering::less);
}