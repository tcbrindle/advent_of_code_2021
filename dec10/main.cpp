
#include "../common.hpp"

#include <stack>

constexpr auto illegal_char_score = [](char c) -> int
{
    switch (c) {
    case ')': return 3;
    case ']': return 57;
    case '}': return 1197;
    case '>': return 25137;
    default: return 0;
    }
};

constexpr auto autocomplete_char_score = [](char c) -> int
{
    switch (c) {
    case ')': return 1;
    case ']': return 2;
    case '}': return 3;
    case '>': return 4;
    default: return 0;
    }
};

enum class state {
    corrupted, incomplete
};

using result = std::pair<state, int64_t>;

constexpr auto get_scores = [](std::string_view line) -> result
{
    std::stack<char> expect;
    int64_t score = 0;

    for (char c : line) {
        switch (c) {
        case '(': expect.push(')'); break;
        case '[': expect.push(']'); break;
        case '{': expect.push('}'); break;
        case '<': expect.push('>'); break;

        case ')': [[fallthrough]];
        case ']': [[fallthrough]];
        case '}': [[fallthrough]];
        case '>':
            if (c == expect.top()) {
                expect.pop();
            } else {
                score = illegal_char_score(c);
                goto out;
            }
        }
    }

out:
    if (score > 0) {
        return result{state::corrupted, score};
    } else {
        while (!expect.empty()) {
            score *= 5;
            score += autocomplete_char_score(expect.top());
            expect.pop();
        }
        return result{state::incomplete, score};
    }
};

constexpr auto calculate = [](std::string_view input)
{
    int p1 = 0;
    std::vector<int64_t> p2;

    aoc::split_string(input, "\n")
        .map(get_scores)
        .for_each([&](result r) {
            switch (r.first) {
            case state::corrupted: p1 += r.second; break;
            case state::incomplete: p2.push_back(r.second); break;
            }
        });

    std::ranges::nth_element(p2, p2.begin() + p2.size()/2);

    return std::pair{p1, p2[p2.size()/2]};
};

constexpr std::string_view test_data =
R"([({(<(())[]>[[{[]{<()<>>
[(()[<>])]({[<{<<[]>>(
{([(<{}[<>[]}>{[]{[(<()>
(((({<>}<{<{<>}{[]{[]{}
[[<[([]))<([[{}[[()]]]
[{[{({}]{}}([{[{{{}}([]
{<[[]]>}<{[{[{[]{()[[[]
[<(<(<(<{}))><([]([]()
<{([([[(<>()){}]>(<<{{
<{([{{}}[<[[[<>{}]]]>[]])";

int main(int argc, char** argv)
{
    {
        auto const [test1, test2] = calculate(test_data);
        assert(test1 == 26397);
        assert(test2 == 288957);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);

    auto const [part1, part2] = calculate(input);

    fmt::print("Part 1: {}\n", part1);
    fmt::print("Part 2: {}\n", part2);
}