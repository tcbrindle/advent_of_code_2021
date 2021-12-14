
#include "../common.hpp"

using rules_t = std::map<std::pair<char, char>, char>;

constexpr auto parse_input = [](std::string_view input)
{
    auto blank = input.find("\n\n");

    auto start_str = std::string(input.substr(0, blank));

    auto rules = aoc::split_string(input.substr(blank + 2), "\n")
                    .map([&](auto line) {
                        return std::pair(std::pair(line.at(0), line.at(1)), line.at(6));
                    })
                    .to<rules_t>();

    return std::pair(std::move(start_str), std::move(rules));
};

constexpr auto calculate = [](std::string const& templ, rules_t const& rules, int const steps)
{
    std::array<int64_t, 26> freqs{};

    // Populate frequency table
    flow::for_each(templ, [&freqs](char c) { ++freqs.at(c - 'A'); });

    using pair_counts_t = std::map<std::pair<char, char>, int64_t>;

    pair_counts_t pair_counts{};

    // Populate pair counts
    flow::zip(templ, flow::drop(templ, 1)).for_each([&pair_counts](auto p) { ++pair_counts[p]; });

    // Perform iteration
    pair_counts = flow::iota(0, steps).fold(
        [&freqs, &rules](pair_counts_t counts, int /*unused*/) {
            pair_counts_t next;

            for (auto [pair, count] : counts) {
                if (auto const iter = rules.find(pair); iter == rules.end()) {
                    // No replacement rule, just copy the count and continue
                    next[pair] = count;
                } else {
                    char const repl = iter->second;

                    freqs.at(repl - 'A') += count;
                    next[{pair.first, repl}] += count;
                    next[{repl, pair.second}] += count;
                }
            }

            return next;
        }, std::move(pair_counts));

    // Calculate result
    return flow::from(freqs)
            .filter(flow::pred::positive)
            .minmax()
            .map([](auto r) { return r.max - r.min; })
            .value();
};

constexpr auto part1 = [](std::string const& templ, const rules_t& rules)
{
    return calculate(templ, rules, 10);
};

constexpr auto part2 = [](std::string const& templ, const rules_t& rules)
{
    return calculate(templ, rules, 40);
};

constexpr std::string_view test_data =
R"(NNCB

CH -> B
HH -> N
CB -> H
NH -> C
HB -> C
HC -> B
HN -> C
NN -> C
BH -> H
NC -> B
NB -> B
BN -> B
BB -> N
BC -> B
CC -> N
CN -> C)";

int main(int argc, char** argv)
{
    {
        auto const [templ, rules] = parse_input(test_data);
        assert(part1(templ, rules) == 1588);
        assert(part2(templ, rules) == 2188189693529);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);
    auto const [templ, rules] = parse_input(input);

    fmt::print("Part 1: {}\n", part1(templ, rules));
    fmt::print("Part 2: {}\n", part2(templ, rules));
}