#include <catch2/catch_test_macros.hpp>
#include <helpers.hpp>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::literals;

TEST_CASE("ranges")
{
    auto data = helpers::create_numeric_dataset<100>(42);
    helpers::print(data, "data");
}

template <auto Value_> // NTTP
struct EndValue
{
    bool operator==(std::input_or_output_iterator auto pos) const
    {
        return *pos == Value_;
    }
};

TEST_CASE("ranges - algorithms")
{
    SECTION("basics")
    {
        auto data = helpers::create_numeric_dataset<100>(42);

        std::ranges::sort(data);
        helpers::print(data, "data");

        std::vector<int> positive_numbers;
        std::ranges::copy_if(data, std::back_inserter(positive_numbers), [](int n) { return n > 0; });
        helpers::print(positive_numbers, "positive_numbers");
    }

    SECTION("projections")
    {
        std::vector<std::string> words = {"twenty-two"s, "a"s, "abc"s, "b"s, "one"s, "aa"s};

        std::sort(words.begin(), words.end(), [](const auto& s1, const auto& s2) { return s1.size() < s2.size(); });
        std::ranges::sort(words, std::less{}, /*projection*/ [](const auto& s) { return s.size(); });

        helpers::print(words, "words sorted by size");
    }

    SECTION("sentinels")
    {
        std::string str = "fajsdkh.gjadfg";

        std::ranges::sort(str.begin(), EndValue<'.'>{});

        helpers::print(str, "str after sort with sentinel");

        EndValue<'\0'> null_term;

        auto& txt = "acbgdef\0ajdhfgajsdhfgkasdjhfg"; // const char(&txt)[30]
        std::array txt_array = std::to_array(txt);

        std::ranges::sort(txt_array.begin(), null_term, std::greater{});

        helpers::print(txt_array, "txt_after sort");

        std::vector data = {5, 423, 665, 1, 235, 42, 6, 345, 33, 665};

        auto pos = std::ranges::find(data.begin(), std::unreachable_sentinel, 42);

        for (auto it = std::counted_iterator{data.begin(), 5}; it != std::default_sentinel; ++it)
        {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        std::vector<int> items1(5);
        std::ranges::copy(std::span{data.begin(), 5}, items1.begin());

        std::vector<int> items2(5);
        std::ranges::copy(std::counted_iterator{data.begin(), 5}, std::default_sentinel, items2.begin());

        CHECK(items1 == items2);
    }
}