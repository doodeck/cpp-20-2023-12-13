#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

///////////////////////////////////////////
// enable_if - how it works

template <bool Condition, typename T = void>
struct EnableIf
{
    using type = T;
};

template <typename T>
struct EnableIf<false, T>
{
};

// template alias
template <bool Condition, typename T = void>
using EnableIf_t = typename EnableIf<Condition, T>::type;

//////////////////////////////////////////

namespace LegacySfinae
{
    template <typename T>
    auto do_stuff(T obj) -> EnableIf_t<(sizeof(T) <= 8)>
    {
        std::cout << "do_stuff(small obj)\n";
    }

    template <typename T>
    auto do_stuff(const T& obj) -> EnableIf_t<(sizeof(T) > 8)>
    {
        std::cout << "do_stuff(large obj)\n";
    }
} // namespace LegacySfinae

template <typename T>
concept SmallType = (sizeof(T) <= 8);

static_assert(SmallType<char>);
static_assert(!SmallType<std::array<int, 1024>>);

template <typename T>
concept BigType = !SmallType<T>;

template <SmallType T>
auto do_stuff(T obj)
{
    std::cout << "do_stuff(small obj)\n";
}

template <BigType T>
auto do_stuff(const T& obj)
{
    std::cout << "do_stuff(large obj)\n";
}

TEST_CASE("enable if & sfinae")
{
    do_stuff(4); // T = int

    std::array<int, 1024> arr{};
    do_stuff(arr);
}