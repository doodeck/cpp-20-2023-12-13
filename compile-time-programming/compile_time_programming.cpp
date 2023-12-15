#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <numeric>
#include <ranges>
#include <algorithm>

using namespace std::literals;

int runtime_func(int x)
{
    return x * x;
}
 
constexpr int constexpr_func(int x) 
{
    return x * x;
}
 
consteval int consteval_func(int x)
{
    return x * x;
}

void compile_time_error() // runtime function
{ }

consteval int next_two_digit_value(int value)
{
    if (value < 9 || value >= 99)
    {
        //compile_time_error();
        throw 13;
    }

    return ++value;
}

TEST_CASE("runtime vs. constexpr vs. consteval")
{
    int x = 42;
    auto runtime_result = runtime_func(x);

    constexpr auto constexpr_result = constexpr_func(42);
    runtime_result = constexpr_func(x); // runtime call

    constexpr auto another_constexpr_result = consteval_func(42);   

    runtime_result = consteval_func(665); // immidiate function call

    auto value = next_two_digit_value(66);
    //value = next_two_digit_value(99);
}

TEST_CASE("lambda + consteval")
{
    auto square = [](int n) consteval { return n * n; };

    std::array data = { square(6), square(42), square(665) };
}

constexpr int len(const char* s)
{
    if (std::is_constant_evaluated())
    // if consteval // since C++23
    {
        // compile-time friendly code
        int idx = 0;
        while (s[idx] != '\0')
            ++idx;
        return idx;
    }
    else
    {
       return strlen(s); // function called at runtime
    }
}

TEST_CASE("is_constant_evaluated")
{
    constexpr auto str_length = len("abc");
}

template <size_t N>
constexpr auto create_powers()
{
    std::array<uint32_t, N> powers{};

    std::iota(powers.begin(), powers.end(), 1); // std algorithms are constexpr

    std::ranges::transform(powers, powers.begin(), [](int x) { return x * x; }); // ranges algorithms are constexpr

    return powers;
}

TEST_CASE("constexpr algorithms")
{
    constexpr auto lookup_squares = create_powers<100>();
}

template <std::ranges::input_range... TRng_>
constexpr auto avg_for_unique(const TRng_&... rng)
{
    using TElement = std::common_type_t<std::ranges::range_value_t<TRng_>...>;

    std::vector<TElement> vec;                            // empty vector
    vec.reserve((rng.size() + ...));                      // reserve a buffer - fold expression C++17
    (vec.insert(vec.end(), rng.begin(), rng.end()), ...); // fold expression C++17

    // sort items
    std::ranges::sort(vec); // std::sort(vec.begin(), vec.end());

    // create span of unique_items
    auto new_end = std::unique(vec.begin(), vec.end());
    std::span unique_items{vec.begin(), new_end};

    // calculate sum of unique items
    auto sum = std::accumulate(unique_items.begin(), unique_items.end(), TElement{});

    return sum / static_cast<double>(unique_items.size());
}

struct Base
{
    virtual constexpr std::string_view foo() const
    {
        return "Base::foo";
    }

    constexpr virtual ~Base() = default;
};

struct Derived : Base
{
    constexpr std::string_view foo() const override
    {
        return "Derived::foo";
    }

    constexpr std::string_view derived_only()
    {
        return "Derived only";
    }

    constexpr ~Derived() = default;
};

constexpr std::string_view with_dynamic_cast()
{
    std::string_view result{"default"};

    Derived obj;
    Base& base_ref = obj;

    // Derived& derived_ref = dynamic_cast<Derived&>(base_ref);
    // result = derived_ref.derived_only();
    
    Derived* derived_ptr = dynamic_cast<Derived*>(&base_ref);
    if (derived_ptr)
         result = derived_ptr->derived_only();

    return result;
}

TEST_CASE("avg for unique")
{
    constexpr std::array lst1 = {1, 2, 3, 4, 5};
    constexpr std::array lst2 = {5, 6, 7, 8, 9};

    constexpr auto avg = avg_for_unique(lst1, lst2);

    std::cout << "AVG: " << avg << "\n";

    constexpr auto result = with_dynamic_cast();
    static_assert(result == "Derived only"sv);
}

//////////////////////////////////////////////////////////////////////////////////

constexpr static std::array values{1, 2, 3};
    
std::array constinit data = values; // initialized at compile time - data is mutable varaible but initialized at compile time

//////////////////////
// another cpp file

std::array other_data = data;