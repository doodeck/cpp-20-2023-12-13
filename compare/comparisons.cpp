#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

using namespace std::literals;

TEST_CASE("safe comparisons between integral numbers")
{
    int x = -7;
    unsigned y = 42;

    // CHECK(x < y);
    CHECK(std::cmp_less(x, y));
    CHECK(std::cmp_greater_equal(y, x));

    SECTION("generic comparers")
    {
        auto my_cmp_less = [](auto x, auto y) {
            if constexpr (std::is_integral_v<decltype(x)> && std::is_integral_v<decltype(y)>)
                return std::cmp_less(x, y);
            else
                return x < y;
        };

        CHECK(my_cmp_less(x, y));
        CHECK(my_cmp_less("one"s, "two"s));
    }

    SECTION("for loops - index with sign")
    {
        std::vector<int> vec = {1, 2, 3};
        for (int i = 0; std::cmp_less(i, vec.size()); ++i)
        {
            std::cout << vec[i] << "\n";
        }

        for (int i = 0; i < std::ranges::ssize(vec); ++i)
        {
            std::cout << vec[i] << "\n";
        }
    }

    SECTION("in_range")
    {
        CHECK(std::in_range<size_t>(665));
        CHECK(std::in_range<size_t>(-1) == false);
        CHECK(std::in_range<uint8_t>(257) == false);
    }
}

struct Point
{
    int x, y;

    Point(int x = 0, int y = 0) : x{x}, y{y}
    {}

    bool operator==(const Point& other) const = default;
};

struct NamedPoint : Point
{
    std::string name;

    NamedPoint(int x = 0, int y = 0, std::string name = "not-set") : Point{x, y}, name{std::move(name)}
    {}

    bool operator==(const NamedPoint& npt) const = default;
};

TEST_CASE("operator ==")
{
    Point pt{10, 20};

    CHECK(pt == Point{10, 20});
    CHECK(pt != Point{20, 10}); // rewriting of expression: !(pt == Point{20, 10})

    Point pt_other = 30;   // Point(30, 0)
    CHECK(pt_other == 30); // implicit conversion: pt_other == Point(30)
    CHECK(30 == pt_other); // rewriting of expression: pt_other == 30 -> pt_other == Point(30)

    NamedPoint named_pt{0, 0, "origin"};
    CHECK(named_pt == NamedPoint{0, 0, "origin"});
    CHECK(named_pt != NamedPoint{0, 0});

    CHECK(named_pt == Point{0, 0}); // works with base class
}