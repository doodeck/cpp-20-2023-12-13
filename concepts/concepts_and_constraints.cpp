#include <cassert>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std::literals;

namespace Traits
{
    template <typename T>
    struct is_void
    {
        static constexpr bool value = false;
    };

    template <>
    struct is_void<void>
    {
        static constexpr bool value = true;
    };

    // template variable
    template <typename T>
    constexpr bool is_void_v = is_void<T>::value;

    /////////////////////////////////////////////

    template <typename T>
    struct is_pointer
    {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_pointer<T*>
    {
        static constexpr bool value = true;
    };

    // template variable
    template <typename T>
    constexpr bool is_pointer_v = is_pointer<T>::value;
} // namespace Traits

template <typename T>
void foo()
{
    static_assert(!Traits::is_void_v<T>, "T cannot be void");
}

template <typename T>
decltype(auto) deref(T ptr)
{
    static_assert(Traits::is_pointer_v<T>, "T must a pointer type");
    assert(ptr != nullptr);
    return *ptr;
}

TEST_CASE("traits")
{
    foo<int>();
    // foo<void>();

    int x = 10;
    CHECK(deref(&x) == 10);
    // deref(x);

    std::string str = "text";
    deref(&str) = "abc";
    CHECK(str == "abc"s);
}

////////////////////////////////////////////////////

namespace ver_1
{
    template <typename T1, typename T2>
    auto max_value(T1 a, T2 b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires Traits::is_pointer_v<T>
    auto max_value(T a, auto b)
        requires(Traits::is_pointer_v<decltype(b)>) // trailing requires clause
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace ver_1

template <typename T>
concept Pointer = Traits::is_pointer_v<T>;

namespace ver_2
{
    template <typename T1, typename T2>
    auto max_value(T1 a, T2 b)
    {
        return a < b ? b : a;
    }

    template <typename T>
        requires Pointer<T>
    auto max_value(T a, auto b)
        requires(Pointer<decltype(b)>) // trailing requires clause
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace ver_2

namespace ver_3
{
    template <typename T>
    auto max_value(T a, T b)
    {
        return a < b ? b : a;
    }

    template <Pointer T>
    auto max_value(T a, T b)
    {
        assert(a != nullptr);
        assert(b != nullptr);
        return *a < *b ? *b : *a;
    }
} // namespace ver_3

auto max_value(auto a, auto b)
{
    return a < b ? b : a;
}

auto max_value(Pointer auto a, Pointer auto b)
{
    assert(a != nullptr);
    assert(b != nullptr);
    return *a < *b ? *b : *a;
}

TEST_CASE("max_value")
{
    int x = 10;
    int y = 20;
    CHECK(max_value(x, y) == 20);

    CHECK(max_value(&x, &y) == 20);

    static_assert(Pointer<int*>);
    static_assert(Pointer<const double*>);
    static_assert(!Pointer<int>);
}