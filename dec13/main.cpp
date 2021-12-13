
#include "../common.hpp"

struct point {
    int x;
    int y;

    bool operator==(point const&) const = default;
    auto operator<=>(point const&) const = default;
};

enum class fold_axis { x, y };

struct fold_instruction {
    fold_axis axis;
    int pos;
};

constexpr auto contains = [](auto const& points, point const& p) -> bool
{
    auto const iter = std::ranges::lower_bound(points, p);
    return iter != points.end() && *iter == p;
};

constexpr auto uniquify = [](auto& vec) {
    std::ranges::sort(vec);
    auto to_erase = std::ranges::unique(vec);
    vec.erase(to_erase.begin(), to_erase.end());
};

constexpr auto process_fold = [](auto& points, fold_instruction inst)
{
    for (point& p : points) {
        if (inst.axis == fold_axis::y && p.y > inst.pos) {
            p = {p.x, 2 * inst.pos - p.y};
        } else if (inst.axis == fold_axis::x && p.x > inst.pos) {
            p = {2 * inst.pos - p.x, p.y};
        }
    }
};

constexpr auto part1 = [](auto points, auto const& folds)
{
    process_fold(points, folds.at(0));
    uniquify(points);
    return points.size();
};

constexpr auto part2 = [](auto points, auto const& folds)
{
    for (auto const& i : folds) {
        process_fold(points, i);
    }
    uniquify(points);
    return points;
};

constexpr auto display_points = [](auto const& points)
{
    auto [min_x, max_x] = flow::map(points, &point::x).minmax().value();
    auto [min_y, max_y] = flow::map(points, &point::y).minmax().value();

    flow::cartesian_product_with(
        [&](int y, int x) {
            return x == max_x + 1 ? '\n' :
                    contains(points, {x, y}) ? '#' : ' ';
        },
        flow::iota(min_y, max_y + 1), flow::iota(min_x, max_x + 2))
        .write_to(std::cout, "");
};

constexpr auto parse_points = [](std::string_view input)
{
    return aoc::split_string(input, "\n")
        .map([](std::string_view sv) {
            auto c = sv.find(',');
            int x = aoc::try_parse<int>(sv.substr(0, c)).value();
            int y = aoc::try_parse<int>(sv.substr(c + 1)).value();
            return point{x, y};
        })
        .to_vector();
};

constexpr auto parse_folds = [](std::string_view input)
{
    return aoc::split_string(input, "\n")
        .map([](std::string_view sv) {
            auto const axis = [&] {
                switch (sv.at(11)) {
                case 'x': return fold_axis::x;
                case 'y': return fold_axis::y;
                default: throw std::runtime_error("Unexpected fold instruction");
                }
            }();

            int const pos = aoc::try_parse<int>(sv.substr(13)).value();

            return fold_instruction{.axis = axis, .pos = pos};
        })
        .to_vector();
};

constexpr auto parse_input = [](std::string_view input)
{
    auto split_pt = input.find("\n\n");

    return std::pair{parse_points(input.substr(0, split_pt)),
                     parse_folds(input.substr(split_pt + 2))};
};

constexpr std::string_view test_data =
R"(6,10
0,14
9,10
0,3
10,4
4,11
6,0
6,12
4,1
0,13
10,12
3,4
3,0
8,4
1,10
2,14
8,10
9,0

fold along y=7
fold along x=5)";

int main(int argc, char** argv)
{
    {
        auto const [points, folds] = parse_input(test_data);
        assert(part1(points, folds) == 17);
        auto const p2 = part2(points, folds);
        assert(p2.size() == 16);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input");
    }

    auto const input = aoc::string_from_file(argv[1]);

    auto const [points, folds] = parse_input(input);

    fmt::print("Part 1: {}\n", part1(points, folds));

    aoc::timer t;
    auto const p2 = part2(points, folds);
    auto e = t.elapsed();

    fmt::print("Part 2: ({})\n", e);
    display_points(p2);
}