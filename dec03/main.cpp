
#include "../common.hpp"

using namespace std::string_view_literals;

constexpr auto parse_input = [](const char* path)
{
    auto file = std::ifstream(path);
    return aoc::lines(file).to_vector();
};

constexpr auto bstring_to_uint = [](std::string_view str)
{
    std::uint32_t u = 0;
    for (char c : str) {
        u *= 2;
        u += c == '1' ? 1 : 0;
    }
    return u;
};

template <int Bits>
constexpr auto part1 = [](auto const& input) {
    uint32_t gamma = 0;
    uint32_t epsilon = 0;

    for (int b = 0; b < Bits; b++) {
        gamma *= 2;
        epsilon *= 2;

        auto n_ones = flow::count_if(input, [b] (std::string_view val) {
            return  val[b] == '1';
        });

        if (n_ones >= input.size()/2) {
            gamma += 1;
        } else {
            epsilon += 1;
        }
    }

    return gamma * epsilon;
};

template <int Bits>
constexpr auto part2 = [](auto input) {

    auto sub = std::ranges::subrange(input);

    for (int b = 0; b < Bits; b++) {
        if (sub.size() == 1) {
            break;
        }

        auto check1 = [b] (std::string_view val) { return val[b] == '1'; };

        // Count the 1s
        auto n_ones = flow::count_if(sub, check1);

        if (n_ones >= (sub.size() - n_ones)) {
            auto out = std::ranges::partition(sub, check1);
            sub = std::ranges::subrange(sub.begin(), out.begin());
        } else {
            auto out = std::ranges::partition(sub, std::not_fn(check1));
            sub = std::ranges::subrange(sub.begin(), out.begin());
        }
    }

    auto oxygen = bstring_to_uint(sub[0]);

    /// and again
    sub = std::ranges::subrange(input);

    for (int b = 0; b < Bits; b++) {
        if (sub.size() == 1) {
            break;
        }

        auto check1 = [b] (std::string_view val) { return val[b] == '1'; };

        // Count the 1s
        auto n_ones = flow::count_if(sub, check1);

        if (n_ones >= (sub.size() - n_ones)) {
            auto out = std::ranges::partition(sub, std::not_fn(check1));
            sub = std::ranges::subrange(sub.begin(), out.begin());
        } else {
            auto out = std::ranges::partition(sub, check1);
            sub = std::ranges::subrange(sub.begin(), out.begin());
        }
    }

    auto co2 = bstring_to_uint(sub[0]);

    return oxygen * co2;
};

constexpr std::array test_data = {
"00100"sv,
"11110"sv,
"10110"sv,
"10111"sv,
"10101"sv,
"01111"sv,
"00111"sv,
"11100"sv,
"10000"sv,
"11001"sv,
"00010"sv,
"01010"sv};

static_assert(part1<5>(test_data) == 198);
static_assert(part2<5>(test_data) == 230);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    const auto input = parse_input(argv[1]);

    fmt::print("Part 1: {}\n", part1<12>(input));
    fmt::print("Part 2: {}\n", part2<12>(input));
}