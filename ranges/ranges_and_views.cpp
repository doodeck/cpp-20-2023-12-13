#include <catch2/catch_test_macros.hpp>
#include <helpers.hpp>
#include <iostream>
#include <list>
#include <map>
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

template <std::ranges::range Rng>
    requires requires(std::ranges::range_value_t<Rng> ptr) { ptr == nullptr; }
auto find_null(Rng&& rng)
{
    for (auto it = std::ranges::begin(rng); it != std::ranges::end(rng); ++it)
    {
        if (*it == nullptr)
            return it;
    }

    return std::ranges::end(rng);
}

TEST_CASE("ranges - tools")
{
    std::shared_ptr<int> vec[] = {std::make_shared<int>(42), nullptr, std::make_shared<int>(665)};

    auto pos = find_null(vec);
    CHECK(*pos == nullptr);
}

TEST_CASE("ranges - views")
{
    std::list<int> lst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("subrange")
    {
        helpers::print(lst, "lst");

        auto sublst = std::ranges::subrange{lst.begin(), EndValue<5>{}};
        helpers::print(sublst, "sublist");

        std::ranges::fill(sublst, 0);
        helpers::print(lst, "list");
    }

    SECTION("all")
    {
        auto all_items = std::views::all(lst);

        helpers::print(all_items, "all items");
    }

    SECTION("counted")
    {
        auto first_half = std::views::counted(lst.begin(), lst.size() / 2);

        for (auto& item : first_half)
            item *= 2;

        helpers::print(lst, "lst");
    }

    SECTION("iota")
    {
        helpers::print(std::views::iota(1, 20), "iota");
    }

    SECTION("piping with |")
    {
        auto data = std::views::iota(1)
            | std::views::take(20)
            | std::views::filter([](int x) { return x % 2 == 0; })
            | std::views::transform([](int x) { return x * x; })
            | std::views::reverse;

        helpers::print(data, "data");
    }

    SECTION("keys - values")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

        helpers::print(dict | std::views::keys, "keys");
        helpers::print(dict | std::views::values, "values");
        helpers::print(dict | std::views::elements<1>, "values aka. elements<1>");
    }
}

std::vector<std::string_view> tokenize(std::string_view text, auto separator)
{
    auto tokens = text | std::views::split(separator);

    std::vector<std::string_view> tokens_sv;

    for (auto&& rng : tokens)
    {
        tokens_sv.push_back(std::string_view(&(*rng.begin()), rng.end() - rng.begin()));
        // tokens_sv.emplace_back(rng); // C++23
    }

    return tokens_sv;
}

template <typename T>
std::vector<std::span<T>> tokenize(std::span<T> text, auto separator)
{
    using Token = std::span<T>;

    std::vector<Token> tokens;

    for (auto&& rng : text | std::views::split(separator))
    {
        tokens.emplace_back(rng);
    }

    return tokens;
}

TEST_CASE("split")
{
    std::string str = "abc,def,ghi";

    SECTION("with string_view")
    {
        std::vector<std::string_view> tokens_sv = tokenize(str, ',');

        auto expected_tokens = std::vector{"abc"sv, "def"sv, "ghi"sv};

        CHECK(tokens_sv == expected_tokens);
        helpers::print(tokens_sv, "tokens_sv");
    }
}

namespace ConstFiasco
{
    template <typename T>
    concept PrintableRange = std::ranges::range<T>
        && requires(const std::ranges::range_value_t<T>& item) { std::cout << item; };

    template <std::ranges::view T>
    void print(T rng, std::string_view description)
    {
        std::cout << description << ": { ";
        for (const auto& item : rng)
        {
            std::cout << item << " ";
        }
        std::cout << "}\n";
    }
} // namespace ConstFiasco

TEST_CASE("const fiasco")
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    ConstFiasco::print(std::views::all(vec), "vec");
    ConstFiasco::print(std::ranges::subrange(vec.begin(), vec.begin() + 5), "subvec");
    ConstFiasco::print(vec | std::views::filter([](int x) { return x % 2 == 0; }), "evens");
}

TEST_CASE("borrowed iterator")
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("OK")
    {
        auto pos = std::ranges::find(vec, 5);
        CHECK(*pos == 5);
    }

    SECTION("Dangling")
    {
        auto pos = std::ranges::find(std::vector{1, 2, 3, 4, 5, 6}, 5);
        // CHECK(*pos == 5); // ERROR - pos is dangling
    }    
}