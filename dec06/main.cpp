
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

constexpr auto process = [](const auto& input, int days)
{
    std::array<int64_t, 9> counts{};

    for (int i : input) {
        ++counts[i];
    }

    flow::ints(0, days).for_each([&](int i) {
        counts[(i + 7) % 9] += counts[i % 9];
    });

    return flow::sum(counts);
};

constexpr std::array test_data = { 3, 4, 3, 1, 2 };

static_assert(process(test_data, 80) == 5934);
static_assert(process(test_data, 256) == 26984457539);

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const input = read_vector(argv[1]);

    fmt::print("Part 1: {}\n", process(input, 80));
    fmt::print("Part 2: {}\n", process(input, 256));
}