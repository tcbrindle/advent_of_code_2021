
#include "../common.hpp"

constexpr auto read_vector = [](const char* filename) -> std::vector<int>
{
    std::vector<int> vec;
    std::ifstream file(filename);

    int i;
    while (file >> i) {
        vec.push_back(i);
        file.ignore(); // comma
    }

    return vec;
};

constexpr auto part1 = [](auto input)
{
    // The target distance should (I think?) be the median of the input elements
    // We can find this using std::nth_element
    // (Open question: is copy + nth_element cheaper than just brute-force trying all
    //  values in the range [input_min, input_max]?

    std::ranges::nth_element(input, input.begin() + input.size()/2);
    const int median = input[input.size()/2];

    return flow::map(input, [&](int val) { return std::abs(median - val); }).sum();
};

constexpr auto part2 = [](const auto& input)
{
    // Hypothesis: target distance is somewhere around the mean, so let's just try
    // a couple of values either side
    const int mean = flow::sum(input)/input.size();

    return flow::ints(mean - 2, mean + 3)
        .map([&input](int i) {
            return flow::map(input, [i] (int val) {
                const int abs = std::abs(i - val);
                return abs * (1 + abs)/2; // sum of [1, 2, ..., abs]
            }).sum();
        })
        .min().value();
};

constexpr std::array test_data = {16,1,2,0,4,2,7,1,2,14};

static_assert(part1(test_data) == 37);
static_assert(part2(test_data) == 168);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    const auto input = read_vector(argv[1]);

    fmt::print("Part 1: {}\n", part1(input));
    fmt::print("Part 2: {}\n", part2(input));
}
