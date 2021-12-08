
#include "../common.hpp"

struct display {
    std::array<std::string, 10> patterns;
    std::array<std::string, 4> output;
};

constexpr auto parse_input = [](std::string_view input) -> std::vector<display> {
    return aoc::split_string(input, "\n").map([](std::string_view line) {
        display d{};

        auto parts = aoc::split_string(line, " | ");

        auto part1 = parts.next().value();
        aoc::split_string(part1, " ").take(10)
            .map([](std::string_view sv) {
                std::string s(sv);
                std::ranges::sort(s);
                return s;
            })
            .output_to(d.patterns.begin());

        auto part2 = parts.next().value();
        aoc::split_string(part2, " ").take(4)
            .map([](std::string_view sv) {
                std::string s(sv);
                std::ranges::sort(s);
                return s;
            })
            .output_to(d.output.begin());

        return d;
    }).to_vector();
};

constexpr auto part1 = [](std::span<display const> displays) -> int
{
    return flow::flat_map(displays, &display::output)
            .map(&std::string::size)
            .count_if(flow::pred::in(2u, 3u, 4u, 7u));
};


constexpr auto find_index_if = [](auto const& arr, auto constraint)
{
    auto iter = std::ranges::find_if(arr, std::move(constraint));
    assert(iter != arr.end());
    return iter - arr.begin();
};

constexpr auto calculate_mapping = [](std::array<std::string, 10> const& patterns)
{
    std::array<int, 10> mapping; mapping.fill(-1);

    auto length_is = [](std::size_t sz) {
        return flow::pred::detail::predicate([sz](std::string_view sv) { return sv.size() == sz; });
    };

    auto pattern_of = [&](std::size_t idx) { return patterns[mapping[idx]]; };

    auto has_subset = [](std::string_view sub) {
        return flow::pred::detail::predicate([sub](std::string_view sv) {
            return std::ranges::includes(sv, sub);
        });
    };

    auto is_subset_of = [](std::string_view sv) {
        return flow::pred::detail::predicate([sv](std::string_view sub) {
            return std::ranges::includes(sv, sub);
        });
    };

    [[maybe_unused]] auto print_mapping = [&] {
        fmt::print("Mapping:\n");
        for (int i = 0; i < 10; i++) {
            fmt::print("{}: {}\n", i, mapping[i] == -1 ? "<unknown>" : patterns.at(mapping.at(i)));
        }
        fmt::print("\n");
    };

    // The length 2 entry must correspond to the digit 1
    mapping[1] = find_index_if(patterns, length_is(2));

    // The length 3 entry must correspond to the digit 7
    mapping[7] = find_index_if(patterns, length_is(3));

    // The length 4 entry must correspond to the digit 4
    mapping[4] = find_index_if(patterns, length_is(4));

    // The length 7 entry must correspond to the digit 8
    mapping[8] = find_index_if(patterns, length_is(7));

    // Now things get tricker!
    // For the entries of length 6 (three of them), exactly one will contain "4" digit
    // as a subset. This corresponds to the digit 9
    mapping[9] = find_index_if(patterns, length_is(6) and has_subset(pattern_of(4)));

    // One of the length-six entries will *not* contain a 1 digit as a subset. This
    // corresponds to the digit 6
    mapping[6] = find_index_if(patterns, length_is(6) and not has_subset(pattern_of(1)));

    // The digit zero is length-6, contains a 1 as a subset but *not* a 4
    // (Yes, this is the only length-6 entry we haven't assigned yet)
    mapping[0] = find_index_if(patterns,
                        length_is(6) and has_subset(pattern_of(1)) and not has_subset(pattern_of(4)));

    // Nearly there now! On with the length-5 mappings. These are the digits 2, 3, and 5
    // Exactly one of them will have a 1 digit as a subset -- this is the digit 3.
    mapping[3] = find_index_if(patterns, length_is(5) and has_subset(pattern_of(1)));

    // The digit 5 is a subset of the digit 6, but 2 (and 3) are not
    mapping[5] = find_index_if(patterns, length_is(5) and is_subset_of(pattern_of(6)));

    // The digit 2 has length 5, is not a subset of 6, and does not have a 1 as a subset
    mapping[2] = find_index_if(patterns,
                        length_is(5) and not is_subset_of(pattern_of(6)) and not has_subset(pattern_of(1)));


    return mapping;
};

constexpr auto decode_output = [](display const& display, auto const& mapping) -> int
{
    int out = 0;
    for (int i = 0; i < 4; i++) {
        auto idx = find_index_if(display.patterns, flow::pred::eq(display.output[i]));
        auto digit = find_index_if(mapping, flow::pred::eq(idx));
        //fmt::print("{}\n", digit);
        out *= 10;
        out += digit;
    }

    return out;
};

constexpr auto part2 = [](std::span<display const> displays) -> int
{
    return flow::from(displays)
        .map([](display const& disp) {
            auto mapping = calculate_mapping(disp.patterns);
            return decode_output(disp, mapping);
        })
        .sum();
};

constexpr std::string_view test_data1 =
"acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf";

constexpr std::string_view test_data2 =
R"(be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe
edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec | fcgedb cgb dgebacf gc
fgaebd cg bdaec gdafb agbcfd gdcbef bgcad gfac gcb cdgabef | cg cg fdcagb cbg
fbegcd cbd adcefb dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec cb
aecbfdg fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga | gecf egdcabf bgf bfgea
fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf | gebdcfa ecba ca fadegcb
dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc dacgb gdcebf gf | cefg dcbef fcge gbcadfe
bdfegc cbegaf gecbf dfcage bdacg ed bedf ced adcbefg gebcd | ed bcgafe cdgba cbgef
egadfb cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg | gbdfcae bgc cg cgb
gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc | fgae cfgab fg bagce)";

int main(int argc, char** argv)
{
    {
        auto const test_displays = parse_input(test_data1);
        assert(part1(test_displays) == 0);
        assert(part2(test_displays) == 5353);
    }

    {
        auto const test_displays = parse_input(test_data2);
        assert(part1(test_displays) == 26);
        assert(part2(test_displays) == 61229);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    std::string const input = aoc::string_from_file(argv[1]);
    auto const displays = parse_input(input);

    fmt::print("Part 1: {}\n", part1(displays));

    aoc::timer t;
    int p2 = part2(displays);
    auto e = t.elapsed();

    fmt::print("Part 2: {} ({})\n", p2, e);
}