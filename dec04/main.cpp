
#include "../common.hpp"


struct board {

    explicit board(std::string_view vals)
    {
        std::istringstream iss{std::string(vals)};
        flow::from_istream<int>(iss).output_to(numbers.begin());
    }

    bool mark(int number) {
        auto iter = std::ranges::find(numbers, number);
        if (iter != numbers.end()) {
            marked.at(iter - numbers.begin()) = true;
        } else {
            return false;
        }

        // check marked rows
        bool won = flow::chunk(marked, 5)
            .map([](auto chunk) { return std::move(chunk).all(flow::pred::eq(true)); })
            .any(flow::pred::eq(true));

        if (won) {
            return true;
        }

        // Check marked cols
        return flow::ints(0, 5)
                .map([&](int i) {
                    return flow::drop(marked, i).stride(5).all(flow::pred::eq(true));
                })
                .any(flow::pred::eq(true));
    }

    int calculate_score() const
    {
        return flow::zip(numbers, marked)
            .filter([](auto p) { return !p.second; })
            .keys()
            .sum();
    }

    std::array<int, 25> numbers;
    std::array<bool, 25> marked{};
};

constexpr auto parse_input = [](std::string_view input) {

    auto first_newline = input.find('\n');

    auto numbers = aoc::split_string(input.substr(0, first_newline), ",")
        .map([](std::string_view sv) {
            return aoc::try_parse<int>(sv).value();
        })
        .to_vector();

    auto boards = aoc::split_string(input.substr(first_newline + 1), "\n\n")
        .map([](std::string_view sv) {
            return board(sv);
        })
        .to_vector();

    return std::pair(std::move(numbers), std::move(boards));
};

constexpr auto part1 = [](const std::vector<int> numbers, std::vector<board> boards)
    -> flow::maybe<int>
{
    return flow::cartesian_product(numbers, boards)
        .filter([](auto p) { return p.second.mark(p.first) == true; })
        .next()
        .map([](auto p) { return p.first * p.second.calculate_score(); });
};

constexpr auto part2 = [](const std::vector<int>& numbers, std::vector<board> boards) -> flow::maybe<int>
{
    std::vector<bool> eliminated(boards.size(), false);

    for (int i : numbers) {
        for (int n = 0; n < boards.size(); n++) {
            if (eliminated[n]) {
                continue;
            }

            if (boards[n].mark(i)) {
                eliminated[n] = true;
            }

            if (flow::all(eliminated, flow::pred::eq(true))) {
                return {i * boards[n].calculate_score()};
            }
        }
    }

    return {};
};

constexpr std::string_view test_input =
R"(7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1

22 13 17 11  0
 8  2 23  4 24
21  9 14 16  7
 6 10  3 18  5
 1 12 20 15 19

 3 15  0  2 22
 9 18 13 17  5
19  8  7 25 23
20 11 10 24  4
14 21 16 12  6

14 21 17 24  4
10 16 15  9 19
18  8 23 26 20
22 11 13  6  5
 2  0 12  3  7)";

int main(int argc, char** argv)
{
    {
        auto const [nums, boards] = parse_input(test_input);
        assert(part1(nums, boards).value() == 4512);
        assert(part2(nums, boards).value() == 1924);
    }

    auto const input = aoc::string_from_file(argv[1]);
    auto const [numbers, boards] = parse_input(input);

    fmt::print("Part 1: {}\n", part1(numbers, boards).value());
    fmt::print("Part 2: {}\n", part2(numbers, boards).value());
}