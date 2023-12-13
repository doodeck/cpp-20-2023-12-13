#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include <compare>

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

    Point(int x = 0, int y = 0)
        : x{x}
        , y{y}
    { }

    bool operator==(const Point& other) const = default;
};

struct NamedPoint : Point
{
    std::string name;

    NamedPoint(int x = 0, int y = 0, std::string name = "not-set")
        : Point{x, y}
        , name{std::move(name)}
    { }

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

/////////////////////////////////////////////////////////////////////

struct Number
{
    int value;

    Number(int v)
        : value{v}
    { }

    // bool operator==(const Number& other) const = default; // implicitly declared when <=> is defaulted

    auto operator<=>(const Number& other) const = default; // -> strong_ordering deduced
};

struct FloatNumber
{
    float value;

    FloatNumber(float v)
        : value{v}
    { }

    auto operator<=>(const FloatNumber& other) const = default; // -> partial_ordering

    std::partial_ordering operator<=>(const Number& other) const
    {
        return value <=> other.value;
    }
};

TEST_CASE("defining order")
{
    std::vector<Number> numbers = {Number{10}, 20, -5, 42, 665, 8};

    std::ranges::sort(numbers);

    Number n{42};
    CHECK(n == Number{42});
    CHECK(42 == n);
    CHECK(n != Number{665});
    CHECK(n < Number{665});
    CHECK(n > Number{6});
    CHECK(n >= Number{6});
    CHECK(n <= Number{42});

    CHECK(Number{10} <=> Number{10} == 0);

    bool result = Number{1} <=> Number{10} < 0;
    CHECK(result);
    CHECK((Number{10} <=> Number{1}) > 0);

    CHECK((Number{10} <=> FloatNumber{20}) == std::partial_ordering::less);
}

TEST_CASE("three-way-comparison <=>")
{
    SECTION("result is a comparison category")
    {
        auto result = Number{10} <=> Number{20};
        static_assert(std::is_same_v<decltype(result), std::strong_ordering>);
    }

    SECTION("operators: < > <= >= are synthetized")
    {
        CHECK(Number{10} < Number{20}); // operator< is synthetized: Number{10} <=> Number{20} < 0
        CHECK(Number{10} <= Number{20}); // (Number{10} <=> Number{20}) <= 0
    }

    SECTION("int <=> double - implicit conversion")
    {
        auto result = 4 <=> 4.14;
    }
}

TEST_CASE("comparison categories")
{
    SECTION("strong_ordering")
    {
        CHECK(4 <=> 5 == std::strong_ordering::less);
        CHECK(4 <=> 4 == std::strong_ordering::equal);
        CHECK(5 <=> 4 == std::strong_ordering::greater);

        std::string str1 = "abc";
        CHECK(str1 <=> "abc"s == std::strong_ordering::equal);
    }

    SECTION("partial_ordering")
    {
        CHECK(3.14 <=> 4.13 == std::partial_ordering::less);
        CHECK(3.14 <=> 3.14 == std::partial_ordering::equivalent);
        CHECK(4.34 <=> std::numeric_limits<double>::quiet_NaN() == std::partial_ordering::unordered);
    }
}