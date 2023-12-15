#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <format>

using namespace std::literals;

auto lambda_cpp14 = [](auto a, auto b) {
    return a + b;
};

auto fun_cpp20(auto a, auto b)
{
    return a + b;
}

namespace CompilerInterpretetion
{
    template <typename T1, typename T2>
    auto fun_cpp20(T1 a, T2 b)
    {
        return a + b;
    }
} // namespace CompilerInterpretetion

void print_container(const auto& container, std::string_view name)
{
    std::cout << name << ": { ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "}\n";
};

TEST_CASE("functions with auto params")
{
    CHECK(fun_cpp20(42, 662u) == 704);
    CHECK(fun_cpp20("abc"s, "def"s) == "abcdef"s);

    std::vector vec = {1, 2, 3};
    print_container(vec, "vec");
}

/////////////////////////////////////////////

decltype(auto) call_wrapper(auto f, auto&&... args)
{
    std::cout << "Calling a function!\n";

    return f(std::forward<decltype(args)>(args)...);
}

int foo(int x)
{
    return x * 2;
}

TEST_CASE("call wrapper")
{
    auto result = call_wrapper(foo, 42);
    CHECK(result == 84);
}

TEST_CASE("templates & lambda expression")
{
    auto sum = []<typename T>(const std::vector<T>& vec, auto init_value) {
        return std::accumulate(vec.begin(), vec.end(), init_value);
    };

    std::vector vec = {1, 2, 3};
    CHECK(sum(vec, 0) == 6);

    auto forward_to_vector = []<typename T, typename... TArgs>(std::vector<T>& v, TArgs&&... args) {
        v.emplace_back(std::forward<TArgs>(args)...);
    };

    forward_to_vector(vec, 4);
}

TEST_CASE("default construction for lambda")
{
    SECTION("before C++20")
    {
        auto cmp_by_value = [](auto a, auto b) {
            return *a < *b;
        };

        std::set<std::shared_ptr<int>, decltype(cmp_by_value)> my_set(cmp_by_value);

        my_set.insert(std::make_shared<int>(42));
        my_set.insert(std::make_shared<int>(1));
        my_set.insert(std::make_shared<int>(665));
        my_set.insert(std::make_shared<int>(65));

        for (const auto& ptr : my_set)
        {
            std::cout << *ptr << " ";
        }
        std::cout << "\n";
    }

    SECTION("since C++20")
    {
        auto cmp_by_value = [](auto a, auto b) {
            return *a < *b;
        };

        decltype(cmp_by_value) another_instance_of_comparer;

        CHECK(cmp_by_value(std::make_shared<int>(41), std::make_shared<int>(42)));
        CHECK(another_instance_of_comparer(std::make_shared<int>(41), std::make_shared<int>(42)));

        std::set<std::shared_ptr<int>, decltype(cmp_by_value)> my_set;

        my_set.insert(std::make_shared<int>(42));
        my_set.insert(std::make_shared<int>(1));
        my_set.insert(std::make_shared<int>(665));
        my_set.insert(std::make_shared<int>(65));

        for (const auto& ptr : my_set)
        {
            std::cout << *ptr << " ";
        }
        std::cout << "\n";
    }
}

auto create_caller(auto f, auto... args)
{
    return [f, ... args = std::move(args)]() -> decltype(auto) {
        return f(args...);
    };
};

TEST_CASE("lambda - capture parameter pack")
{
    auto calculate = create_caller(std::plus{}, 4, 6);
    CHECK(calculate() == 10);
}

////////////////////////////////////////////////////////////////////////
// NTTP - Non-Type Template Parameter

template <double Factor, typename T>
auto scale(T x)
{
    return x * Factor;
}

namespace AlternativeTake
{
    template <auto Factor, typename T>
    auto scale(T x)
    {
        return x * Factor;
    }
} // namespace AlternativeTake

TEST_CASE("NTTP + double")
{
    CHECK(scale<42.1>(2) == 84.2);
    CHECK(AlternativeTake::scale<42.1>(2) == 84.2);
    CHECK(AlternativeTake::scale<42.1f>(2) == 84.2f);
    CHECK(AlternativeTake::scale<42u>(2) == 84u);
}

//////////////////////////////////////////////////

struct Tax
{
    double value;

    constexpr Tax(double v)
        : value{v}
    {
        assert(value >= 0 && v < 1);
    }

    constexpr double tax_value(double price) const
    {
        return price * value;
    }
};

template <Tax Vat>
double calc_gross_price(double net_price)
{
    return net_price + Vat.tax_value(net_price);
}

TEST_CASE("NTTP + structs")
{
    constexpr static Tax vat_pl{0.23};
    constexpr static Tax vat_ger{0.19};

    CHECK(calc_gross_price<vat_pl>(100.0) == 123.0);
    CHECK(calc_gross_price<Tax{0.23}>(100.0) == 123.0); // gcc || clang only
    CHECK(calc_gross_price<vat_ger>(100.0) == 119.0);
}

///////////////////////////////////////////////////////////////////////

template <size_t N>
struct Str
{
    char value[N];

    constexpr Str(const char (&str)[N])
    {
        std::copy(str, str + N, value);
    }

    friend std::ostream& operator<<(std::ostream& out, const Str& str)
    {
        out << str.value;

        return out;
    }

    auto operator<=>(const Str& other) const = default; // implicitly constexpr
};

template <Str LogPrefix>
struct Logger
{
    void log(std::string_view msg)
    {
        std::cout << LogPrefix << msg << "\n";
    }
};

TEST_CASE("NTTP + strings")
{
    Logger<">: "> my_logger_1;
    my_logger_1.log("Start");

    Logger<"log: "> my_logger_2;
    my_logger_2.log("End");

    std::cout << std::format("key: {0} - value: {1}", 42, "forty-two") << "\n";
}

/////////////////////////////////////////////////////////

template <std::invocable auto GetVat>
double calc_gross_price(double net_price)
{
    return net_price + net_price * GetVat();
}

TEST_CASE("NTTP + lambda")
{
    CHECK(calc_gross_price<[]{ return 0.23; }>(100.0) == 123.0);

    constexpr static auto vat_ger = []{ return 0.19; };
    CHECK(calc_gross_price<vat_ger>(100.0) == 119.0);
}