
#include "../common.hpp"

constexpr auto part1 = [](std::string_view input) {
    int horiz = 0;
    int depth = 0;

    aoc::split_string(input, "\n")
        .for_each([&](auto str) {
            const auto val = str.back() - '0';

            switch (str.front()) {
            case 'f': horiz += val; break;
            case 'd': depth += val; break;
            case 'u': depth -= val; break;
            }
        });

    return horiz * depth;
};

constexpr auto part2 = [](std::string_view input) {
    int horiz = 0;
    int depth = 0;
    int aim = 0;

    aoc::split_string(input, "\n")
        .for_each([&](auto str) {
            const auto val = str.back() - '0';

            switch (str.front()) {
            case 'f': horiz += val; depth += (aim * val); break;
            case 'd': aim += val; break;
            case 'u': aim -= val; break;
            }
        });

    return horiz * depth;
};

constexpr std::string_view test_data =
R"(forward 5
down 5
forward 8
up 3
down 8
forward 2)";

static_assert(part1(test_data) == 150);
static_assert(part2(test_data) == 900);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    std::string input = aoc::string_from_file(argv[1]);

    fmt::print("Part 1: {}\n", part1(input));
    fmt::print("Part 2: {}\n", part2(input));
}