
#include "../common.hpp"

// Since we know our cave system will contain at least "start" and "end",
// we'll add them with fixed ids of 0 and 1 respectively
constexpr std::size_t start_idx = 0;
constexpr std::size_t end_idx = 1;

struct cave_system {
    std::vector<std::vector<std::size_t>> links{ {}, {} };
    std::vector<bool> is_small{true, true};
};

constexpr auto parse_input = [](std::string_view input)
{
    cave_system caves{};
    std::map<std::string_view, int> id_map{ {"start", start_idx}, {"end", end_idx} };

    aoc::split_string(input, "\n")
        .for_each([&](auto line) {
            auto hyphen = line.find('-');
            auto from = line.substr(0, hyphen);
            auto to = line.substr(hyphen+1);

            if (!id_map.contains(from)) {
                id_map[from] = caves.links.size();
                caves.links.push_back({});
                caves.is_small.push_back(::islower(from[0]));
            }
            if (!id_map.contains(to)) {
                id_map[to] = caves.links.size();
                caves.links.push_back({});
                caves.is_small.push_back(::islower(to[0]));
            }

            caves.links[id_map.at(to)].push_back(id_map.at(from));
            caves.links[id_map.at(from)].push_back(id_map.at(to));
        });

    return caves;
};

template <bool HaveUsedSingle = true>
static int count_paths_to_end(cave_system const& caves, std::size_t from_idx,
                              std::vector<bool> const& visited)
{
    return flow::map(caves.links[from_idx], [&](std::size_t dest_idx) {
        if (dest_idx == end_idx) {
            return 1;
        } else if (dest_idx == start_idx) {
            return 0;
        } else if (caves.is_small[dest_idx] && visited[dest_idx]) {
            if constexpr (HaveUsedSingle) {
                return 0;
            } else {
                auto visited_copy = visited;
                visited_copy[from_idx] = true;
                return count_paths_to_end<true>(caves, dest_idx, std::move(visited_copy));
            }
        } else {
            auto visited_copy = visited;
            visited_copy[from_idx] = true;
            return count_paths_to_end<HaveUsedSingle>(caves, dest_idx, std::move(visited_copy));
        }
    }).sum();
}

constexpr auto part1 = [](cave_system const& caves)
{
    return count_paths_to_end(caves, start_idx, std::vector<bool>(caves.links.size()));
};

constexpr auto part2 = [](cave_system const& caves)
{
    return count_paths_to_end<false>(caves, start_idx, std::vector<bool>(caves.links.size()));
};

constexpr std::string_view test_data1 =
R"(start-A
start-b
A-c
A-b
b-d
A-end
b-end)";

constexpr std::string_view test_data2 =
R"(dc-end
HN-start
start-kj
dc-start
dc-HN
LN-dc
HN-end
kj-sa
kj-HN
kj-dc)";

constexpr std::string_view test_data3 =
R"(fs-end
he-DX
fs-he
start-DX
pj-DX
end-zg
zg-sl
zg-pj
pj-he
RW-he
fs-DX
pj-RW
zg-RW
start-pj
he-WI
zg-he
pj-fs
start-RW)";

int main(int argc, char** argv)
{
    {
        auto const test1 = parse_input(test_data1);
        auto const test2 = parse_input(test_data2);
        auto const test3 = parse_input(test_data3);


        assert(part1(test1) == 10);
        assert(part1(test2) == 19);
        assert(part1(test3) == 226);

        assert(part2(test1) == 36);
        assert(part2(test2) == 103);
        assert(part2(test3) == 3509);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const caves = parse_input(aoc::string_from_file(argv[1]));

    fmt::print("Part 1: {}\n", part1(caves));
    fmt::print("Part 2: {}\n", part2(caves));
}