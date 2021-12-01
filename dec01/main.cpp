
#include "../common.hpp"

constexpr auto count_increasing = [](int win_sz, const auto& input) {
    return flow::zip_with(std::greater<>{}, flow::drop(input, win_sz), input)
                .count(true);
};

constexpr auto part1 = std::bind_front(count_increasing, 1);
constexpr auto part2 = std::bind_front(count_increasing, 3);

constexpr std::array test_data = {199, 200, 208, 210, 200, 207, 240, 269, 260, 263};
static_assert(part1(test_data) == 7);
static_assert(part2(test_data) == 5);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const input = aoc::vector_from_file<int>(argv[1]);

    fmt::print("Part 1: {}\n", part1(input));
    fmt::print("Part 2: {}\n", part2(input));
}