
#include "../common.hpp"

#include <ranges>

// This is a bit of an ugly mix of Flow, Ranges and imperative for loops
// It's not exactly stylish, but hey, it works...

struct coord {
    int x;
    int y;
};

constexpr auto parse_input = [](std::string_view input)
{
    std::array<int8_t, 100> grid;
    flow::filter(input, flow::pred::neq('\n'))
        .map([](char c) -> uint8_t { return c - '0'; })
        .take(100)
        .output_to(grid.begin());
    return grid;
};

constexpr std::array<coord, 8> adjacents = {
        coord{-1, -1}, {0, -1}, {1, -1},
             {-1,  0},          {1,  0},
             {-1,  1}, {0,  1}, {1,  1}};

constexpr auto neighbour_indices = [](int idx)
{
    const coord pos = { idx % 10, idx/10 };

    return adjacents
        | std::views::transform([pos](coord c) { return coord{c.x + pos.x, c.y + pos.y}; })
        | std::views::filter([](coord c) {
            return c.x >= 0 && c.x < 10 &&
                   c.y >= 0 && c.y < 10;
            })
        | std::views::transform([](coord c) { return c.x + 10 * c.y; });
};

constexpr auto process_step = [](auto& grid) -> int
{
    for (auto& val : grid) {
        val += 1;
    }

    while (true) {
        auto iter = std::ranges::find_if(grid, flow::pred::gt(9));

        if (iter == grid.end()) {
            break;
        }

        for (int idx : neighbour_indices(iter - grid.begin())) {
            grid[idx] += 1;
        }

        *iter = -10;
    }

    int flash_cnt = 0;

    for (auto& val : grid) {
        if (val < 0) {
            ++flash_cnt;
            val = 0;
        }
    }

    return flash_cnt;
};

constexpr auto part1 = [](auto grid) -> int
{
    return flow::ints(0, 100)
        .map([&](int) { return process_step(grid); })
        .sum();
};

constexpr auto part2 = [](auto grid) -> int
{
    return *std::ranges::find(std::views::iota(1), 100, [&](int) { return process_step(grid); });
};

constexpr auto print_grid = [](auto const& grid)
{
    for (int y = 0; y < 10; y++) {
        flow::drop(grid.data_, 10 * y).take(10).template as<int>().write_to(std::cout, "");
        std::cout << std::endl;
    }
};

constexpr std::string_view test_data =
R"(5483143223
2745854711
5264556173
6141336146
6357385478
4167524645
2176841721
6882881134
4846848554
5283751526)";

int main(int argc, char** argv)
{
    {
        constexpr auto test_grid = parse_input(test_data);
        // These could theoretically be static_asserts, but
        // doing so exceeds GCC's constexpr operation limit :-(
        assert(part1(test_grid) == 1656);
        assert(part2(test_grid) == 195);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);
    auto const grid = parse_input(input);

    fmt::print("Part 1: {}\n", part1(grid));
    fmt::print("Part 2: {}\n", part2(grid));
}