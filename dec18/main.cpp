
#include "../common.hpp"

namespace {

namespace rng = std::ranges;

constexpr int8_t open_br = -1;
constexpr int8_t close_br = -2;
constexpr int8_t comma = -3;

using number = std::vector<int8_t>;

constexpr auto parse_number = [](std::string_view str) -> number
{
    return flow::from(str)
            .map([](char c) -> int8_t {
                switch (c) {
                case '[': return open_br;
                case ']': return close_br;
                case ',': return comma;
                default: return (int8_t) c - '0';
                }
            })
            .to_vector();
};

constexpr auto to_string = [](number const& n) -> std::string
{
    std::string str;
    for (int8_t i : n) {
        switch (i) {
        case open_br: str.push_back('['); break;
        case close_br: str.push_back(']'); break;
        case comma: str.push_back(','); break;
        default: str.append(fmt::format("{}", i));
        }
    }
    return str;
};

constexpr auto explode = [](number& n) -> bool
{
    // Scan from the front of the vector to find the start of a pair with depth 5
    auto iter = n.begin();

    {
        int depth = 0;

        while (iter != n.end()) {
            if (*iter == open_br) {
                ++depth;
            } else if (*iter == close_br) {
                --depth;
            }
            if (depth == 5) {
                break;
            }
            ++iter;
        }
    }

    // If we didn't find one, then there are no explosions to process
    if (iter == n.end()) {
        return false;
    }

    // Grab the first and second values of the pair
    int8_t first = *rng::next(iter);
    int8_t second = *rng::next(iter, 3);

    // Scan backwards from the start of the pair to find the previous value
    // If it exists, adjust it
    {
        auto prev = rng::find_if(std::reverse_iterator(iter), n.rend(), flow::pred::geq(0));
        if (prev != n.rend()) {
            *prev += first;
        }
    }

    // Scan forwards from the end of the pair to find the next value
    // If it exists, adjust it
    {
        auto next = rng::find_if(iter + 5, n.end(), flow::pred::geq(0));
        if (next != n.end()) {
            *next += second;
        }
    }

    // Finally, remove the offending pair and replace it with a zero value
    iter = n.erase(iter, iter + 5);
    n.insert(iter, 0);

    return true;
};

constexpr auto split = [](number& n) -> bool
{
    auto iter = rng::find_if(n, flow::pred::geq(10));
    if (iter == n.end()) {
        return false;
    }

    int8_t left = std::floor(*iter/2.0);
    int8_t right = std::ceil(*iter/2.0);

    iter = n.erase(iter);
    n.insert(iter, {open_br, left, comma, right, close_br});

    return true;
};

constexpr auto reduce = [](number n) -> number
{
    while (true) {
        if (explode(n)) {
            continue;
        }
        if (split(n)) {
            continue;
        }
        break;
    }

    return n;
};

constexpr auto add = [](number const& lhs, number const& rhs) -> number
{
    number n;
    n.reserve(3 + lhs.size() + rhs.size());
    n.push_back(open_br);
    n.insert(n.end(), lhs.begin(), lhs.end());
    n.push_back(comma);
    n.insert(n.end(), rhs.begin(), rhs.end());
    n.push_back(close_br);

    return reduce(std::move(n));
};

constexpr auto magnitude = [](number const& n) -> int
{
    auto iter = n.begin();
    auto const last = n.end();

    // Sneaky C++ recursive lambda trick
    auto impl = [&] (auto& self) -> int {
        assert(iter != last);

        if (*iter == open_br) {
            ++iter; // [
            int left = self(self);
            ++iter; // ,
            int right = self(self);
            ++iter; // ]
            return 3 * left + 2 * right;
        } else {
            return *iter++;
        }
    };

    return impl(impl);
};

constexpr auto part1 = [](std::string_view input) -> int
{
    return aoc::split_string(input, "\n")
            .map(parse_number)
            .fold_first(add)
            .map(magnitude)
            .value();
};

constexpr auto part2 = [](std::string_view input) -> int
{
    const auto nums = aoc::split_string(input, "\n").map(parse_number).to_vector();

    return flow::cartesian_product_with(add, nums, nums).map(magnitude).max().value();
};

constexpr std::string_view test_assignment =
R"([[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
[[[5,[2,8]],4],[5,[[9,9],0]]]
[6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
[[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
[[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
[[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
[[[[5,4],[7,7]],8],[[8,3],8]]
[[9,3],[[9,9],[6,[4,9]]]]
[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
[[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]])";

} // namespace

int main(int argc, char** argv)
{
    assert(part1(test_assignment) == 4140);
    assert(part2(test_assignment) == 3993);

    if (argc < 2) {
        fmt::print(stderr, "No input :-(\n");
        return -1;
    }

    const auto input = aoc::string_from_file(argv[1]);

    fmt::print("Part 1: {}\n", part1(input));
    fmt::print("Part 2: {}\n", part2(input));
}