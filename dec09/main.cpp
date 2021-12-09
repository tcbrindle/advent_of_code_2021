
#include "../common.hpp"

#include <set>

using coord = std::pair<std::uint64_t, std::uint64_t>;

constexpr auto indices = [](auto&& rng) {
    return flow::ints(0, rng.size());
};

constexpr auto at = [](auto const& input, coord pos) -> char {
    if (pos.first >= 0 && pos.first < input.size() &&
           pos.second >=0 && pos.second < input[0].size()) {
        return input[pos.first][pos.second];
    } else {
        return '9';
    }
};

constexpr auto neighbours = [](coord pos)
{
    return flow::of(coord{pos.first - 1, pos.second},
                    coord{pos.first + 1, pos.second},
                    coord{pos.first, pos.second - 1},
                    coord{pos.first, pos.second + 1});
};

constexpr auto is_low_point = [](auto const& input, coord pos)
{
    return neighbours(pos)
            .map([&](coord c) { return at(input, c); })
            .all(flow::pred::gt(at(input, pos)));
};

constexpr auto part1 = [](auto const& input) {
    return flow::cartesian_product(indices(input), indices(input[0]))
        .filter([&](coord p) { return is_low_point(input, p); })
        .map([&](coord p) { return 1 + at(input, p) - '0'; })
        .sum();
};

void basin_size_rec(auto& pts, auto const& input, coord pos)
{
    pts.insert(pos);

    neighbours(pos)
        .filter([&] (coord c) {
            return at(input, c) > at(input, pos) && at(input, c) < '9';
        })
        .for_each([&](coord c) { basin_size_rec(pts, input, c); });
}

constexpr auto basin_size = [](auto const& input, coord pos) -> int
{
    std::set<coord> points;
    basin_size_rec(points, input, pos);
    return points.size();
};

constexpr auto part2 = [](auto const& input)
{
    auto const basin_sizes =
        flow::cartesian_product(indices(input), indices(input[0]))
            .filter([&](coord p) { return is_low_point(input, p); })
            .map([&](coord p) { return basin_size(input, p); })
            .to_vector();

    std::array<int, 3> top3;
    std::ranges::partial_sort_copy(basin_sizes, top3, std::greater<>{});

    return flow::product(top3);
};

constexpr auto read_input = [](char const* filename)
{
    std::ifstream file(filename);
    return aoc::lines(file).to_vector();
};

constexpr std::array<std::string_view, 5> test_data = {
"2199943210",
"3987894921",
"9856789892",
"8767896789",
"9899965678"
};

int main(int argc, char** argv)
{
    {
        static_assert(part1(test_data) == 15);
        assert(part2(test_data) == 1134);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const input = read_input(argv[1]);

    fmt::print("Part 1: {}\n", part1(input));
    fmt::print("Part 2: {}\n", part2(input));
}