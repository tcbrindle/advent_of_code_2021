
#include "../common.hpp"

#include <cmath>
#include <compare>

struct coord {
    int x;
    int y;

    auto operator<=>(const coord&) const = default;
    bool operator==(const coord&) const = default;
};

struct line {
    coord from;
    coord to;
};

constexpr auto parse_input = [](std::string_view input)
{
    return aoc::split_string(input, "\n")
        .map([](std::string_view sv) {
            std::string str(sv);
            int x1, y1, x2, y2;
            std::sscanf(str.c_str(), "%d,%d -> %d,%d", &x1, &y1, &x2, &y2);

            return line{.from{x1, y1}, .to{x2, y2}};
        })
        .to_vector();
};

constexpr auto part1 = [](const auto& lines)
{
    std::map<coord, int> counts;

    for (auto const& line : lines) {
        if (line.from.x == line.to.x) {
            auto [miny, maxy] = std::ranges::minmax(line.from.y, line.to.y);
            for (int y = miny; y <= maxy; y++) {
                ++counts[{line.from.x, y}];
            }
        } else if (line.from.y == line.to.y) {
            auto [minx, maxx] = std::ranges::minmax(line.from.x, line.to.x);
            for (int x = minx; x <= maxx; x++) {
                ++counts[{x, line.to.y}];
            }
        }
    }

    return flow::values(counts).count_if(flow::pred::gt(1));
};

constexpr auto part2 = [](const auto& lines)
{
    std::map<coord, int> counts;

    for (auto const& line : lines) {
        int xinc = std::clamp(line.to.x - line.from.x, -1, 1);
        int yinc = std::clamp(line.to.y - line.from.y, -1, 1);

        int steps = std::max(std::abs(line.from.x - line.to.x),
                             std::abs(line.from.y - line.to.y));

        for (int i = 0; i <= steps; i++) {
            ++counts[{line.from.x + i * xinc, line.from.y + i * yinc}];
        }
    }

    return flow::values(counts).count_if(flow::pred::gt(1));
};

constexpr std::string_view test_data =
R"(0,9 -> 5,9
8,0 -> 0,8
9,4 -> 3,4
2,2 -> 2,1
7,0 -> 7,4
6,4 -> 2,0
0,9 -> 2,9
3,4 -> 1,4
0,0 -> 8,8
5,5 -> 8,2)";

int main(int argc, char** argv)
{
    {
        const auto lines = parse_input(test_data);
        assert(part1(lines) == 5);
        assert(part2(lines) == 12);
    }

    const auto input = aoc::string_from_file(argv[1]);
    const auto lines = parse_input(input);

    fmt::print("Part 1: {}\n", part1(lines));
    fmt::print("Part 2: {}\n", part2(lines));
}