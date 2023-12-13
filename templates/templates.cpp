#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

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
    for(const auto& item : container)
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