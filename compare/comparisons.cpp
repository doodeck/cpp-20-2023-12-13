#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <compare>
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


    CHECK((Number{10} <=> Number{10} == 0));

    bool result = (Number{1} <=> Number{10}) < 0;
    CHECK(result);
    CHECK((Number{10} <=> Number{1} > 0));

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
        CHECK(Number{10} < Number{20});  // operator< is synthetized: Number{10} <=> Number{20} < 0
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

        std::tuple tpl1{1, "text"s};
        CHECK(tpl1 <=> std::tuple{1, "text"s} == std::strong_ordering::equal);
    }

    SECTION("partial_ordering")
    {
        CHECK(3.14 <=> 4.13 == std::partial_ordering::less);
        CHECK(3.14 <=> 3.14 == std::partial_ordering::equivalent);
        CHECK(4.34 <=> std::numeric_limits<double>::quiet_NaN() == std::partial_ordering::unordered);

        std::tuple tpl1{1.1, "text"s};

        auto result = tpl1 <=> std::tuple{1.1, "text"s}; // std::partial_ordering::equivalent
        CHECK(result == std::partial_ordering::equivalent);
    }
}

struct Temperature
{
    double value;

    bool operator==(const Temperature& other) const = default;

    std::strong_ordering operator<=>(const Temperature& other) const
    {
        return std::strong_order(value, other.value);
    }
};

TEST_CASE("strong_order - custom operator<=>")
{
    auto result = Temperature{9.9} <=> Temperature{9.9};

    CHECK(Temperature{9.0} == Temperature{9.0});
}

struct Human
{
    std::string name; // std::strong_ordering
    uint8_t age;      // std::strong_ordering
    double height;    // std::partial_ordering

    auto tied() const
    {
        return std::tie(name, age);
    }

    bool operator==(const Human& other) const
    {
        // return name == other.name && age == other.age;
        return tied() == other.tied();
    }

    std::strong_ordering operator<=>(const Human& other) const
    {
        // if (auto cmp_result = name <=> other.name; cmp_result == 0)
        // {
        //     return age <=> other.age;
        // }
        // else
        // {
        //     return cmp_result;
        // }

        return tied() <=> other.tied();
    }
};

TEST_CASE("custom <=> - many fields")
{
    Human jan1{"Jan", 42, 1.77};
    Human jan2{"Jan", 42, 1.87};

    CHECK(jan1 == jan2);
    CHECK(jan1 >= jan2);
}

///////////////////////////////////////////////////////////////////////////

struct CIString
{
    std::string str;

    std::string to_upper_copy() const
    {
        std::string upper_str{str};
        std::ranges::transform(upper_str, upper_str.begin(), [](auto c) { return std::toupper(c); });
        return upper_str;
    }

    bool operator==(const CIString& other) const
    {
        return str == other.str;
    }

    std::weak_ordering operator<=>(const CIString& other) const
    {
        std::string upper_str_left = to_upper_copy();
        std::string upper_str_right = other.to_upper_copy();

        // if (upper_str_left == upper_str_right)
        //     return std::weak_ordering::equivalent;
        // if (upper_str_left < upper_str_right)
        //     return std::weak_ordering::less;
        // return std::weak_ordering::greater;

        return upper_str_left <=> upper_str_right;
    }
};

TEST_CASE("Case-Insensitive String")
{
    CIString str1{"one"};
    CIString str2{"ONE"};

    CHECK(str1 != str2);
    CHECK((str1 <=> str2) == std::weak_ordering::equivalent);
}

struct Base
{
    std::string value;

    bool operator==(const Base& other) const { return value == other.value; }
    bool operator<(const Base& other) const { return value < other.value; }
};

struct Derived : Base
{
    std::vector<int> data;

    std::strong_ordering operator<=>(const Derived& other) const = default;
};

TEST_CASE("default <=> - how it works")
{
    Derived d1{{"text"}, {1, 2, 3}};
    Derived d2{{"text"}, {1, 2, 4}};

    CHECK(d1 < d2);
}

struct Data
{
    int* buffer_;
    std::size_t size_;

public:
    Data(std::initializer_list<int> lst)
        : buffer_{new int[lst.size()]}
        , size_{lst.size()}
    {
        std::ranges::copy(lst, buffer_);
    }

    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;

    ~Data()
    {
        delete[] buffer_;
    }

    bool operator==(const Data& other) const
    {
        return size_ == other.size_ && std::equal(buffer_, buffer_ + size_, other.buffer_);
    }

    auto operator<=>(const Data& other) const
    {
        return std::lexicographical_compare_three_way(buffer_, buffer_ + size_, other.buffer_, other.buffer_ + other.size_);
    }
};

TEST_CASE("Data - comparisons")
{
    Data ds1{1, 2, 3};
    Data ds2{1, 2, 3};
    Data ds3{1, 2, 4};

    CHECK(ds1 == ds2);
    CHECK(ds1 < ds3);
}