#include <cassert>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <set>
#include <memory>

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

namespace ver_2
{
    template <typename T>
    concept Pointer = Traits::is_pointer_v<T>;

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

template <typename T>
concept Pointer = requires(T ptr) {
    *ptr;
    ptr == nullptr;
};

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

    auto ptr1 = std::make_shared<int>(10);
    auto ptr2 = std::make_shared<int>(20);
    CHECK(max_value(ptr1, ptr2) == 20);
}

//////////////////////////////////////////////

template <typename T>
concept PrintableRange = std::ranges::range<T>
    && requires(const std::ranges::range_value_t<T>& item) { std::cout << item; };

template <PrintableRange T>
void print(const T& rng, std::string_view description)
{
    std::cout << description << ": {";
    for (const auto& item : rng)
    {
        std::cout << item << " ";
    }
    std::cout << "}\n";
}

TEST_CASE("printable range concept")
{
    std::vector<int> vec = {1, 2, 3};
    print(vec, "vec");

    // std::vector<std::pair<int, int>> pairs = { {1, 1}, {2, 2} };
    // print(pairs, "pairs");
}

namespace Concepts
{

    template <std::integral T>
    struct Integer
    {
        T value;
    };

    template <typename T>
    struct Wrapper
    {
        T value;

        void print() const
        {
            std::cout << "value: " << value << "\n";
        }

        void print() const
            requires PrintableRange<T>
        {
            ::print(value, "values");
        }
    };
} // namespace Concepts

TEST_CASE("concepts & class templates")
{
    Concepts::Integer<int> i1{42};
    // Integer<float> i2{3.14};

    Concepts::Wrapper<int> w1{42};
    w1.print();

    std::vector vec = {1, 2, 3};
    Concepts::Wrapper w2{vec};
    w2.print();
}

std::unsigned_integral auto get_id()
{
    static uint64_t id{};
    return ++id;
}

TEST_CASE("concepts + auto")
{
    std::convertible_to<uintmax_t> auto id = get_id(); // std::convertible_to<decltype(id), uintmax_t>
}

//////////////////////////////////////////
// requires expression

template <typename T>
concept Hashable = requires(const T& obj) {
    {
        std::hash<T>{}(obj)
    } -> std::convertible_to<size_t>;
};

template <typename T>
    requires          // requires clause
    requires(T obj) { // requires expression
        requires sizeof(obj) >= 8;
    } // requires that evaluates predicate inside requires expression
void foo()
{ }

///////////////////////////////////////////

namespace ver_1
{
    template <typename T>
    concept AdditiveRange = requires(T&& c) {
        std::ranges::begin(c);
        std::ranges::end(c);
        typename std::ranges::range_value_t<T>; // type requirement
        requires requires(std::ranges::range_value_t<T> x) { x + x; };
    };
}

template <typename T>
concept AdditiveRange = std::ranges::range<T> && requires(std::ranges::range_value_t<T> x) { x + x; };

template <AdditiveRange Rng>
    requires std::default_initializable<std::ranges::range_value_t<Rng>>
auto sum(const Rng& data)
{
    return std::accumulate(std::begin(data), std::end(data),
        std::ranges::range_value_t<Rng>{});
}

TEST_CASE("sum with concepts")
{
    std::vector vec = {1, 2, 3};
    CHECK(sum(vec) == 6);

    std::vector<std::string> words = {"abc", "def"};
    sum(words);
}

template <typename TItem>
void add_to_container(auto& container, TItem&& item)
{
    if constexpr(requires { container.push_back(std::forward<TItem>(item)); })
        container.push_back(std::forward<TItem>(item));
    else
        container.insert(std::forward<TItem>(item));
}

TEST_CASE("add to container")
{
    std::vector<int> vec = {1, 2, 3};
    add_to_container(vec, 4);

    std::set<int> my_set = {1, 2, 3};
    add_to_container(my_set, 4);
}

/////////////////////////////////////////////////////////////////////
// concept subsumation

//////////////////////////////////////////////
// concept subsumation

struct BoundingBox
{
    int w, h;
};

struct Color
{
    uint8_t r, g, b;
};

template <typename T>
concept Shape = requires(const T& obj)
{
    { obj.box() } noexcept -> std::same_as<BoundingBox>;
    obj.draw();
};

template <typename T>
concept ShapeWithColor = Shape<T>
 && requires(T&& obj, Color c) { // ShapeWithColor subsumes Shape
    obj.set_color(c);
    { obj.get_color() } noexcept -> std::same_as<Color>;
};

struct Rect
{
    int w, h;
    Color color;

    void draw() const
    {
        std::cout << "Rect::draw()\n";
    }

    BoundingBox box() const noexcept
    {
        return BoundingBox{w, h};
    }

    Color get_color() const noexcept
    {
        return color;
    }

    void set_color(Color new_color)
    {
        color = new_color;
    }
};

static_assert(Shape<Rect>);
static_assert(ShapeWithColor<Rect>);

template <Shape T>
void render(T& shp)
{
    std::cout << "render<Shape T>\n";
    shp.draw();
}

template <ShapeWithColor T>
void render(T& shp)
{
    std::cout << "render<ShapeWithColor T>\n";
    shp.set_color(Color{0, 0, 0});
    shp.draw();
}

TEST_CASE("concept subsumation")
{
    Rect rect{10, 200};
    render(rect);
}

