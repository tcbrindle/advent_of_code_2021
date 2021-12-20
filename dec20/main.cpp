
#include "../common.hpp"

#include <bitset>

struct coord {
    int x;
    int y;
};

struct image {
    std::vector<bool> pixels;
    int size = 0;
    bool background = false;

    auto operator[](coord const& c) const -> bool
    {
        if (c.x >= 0 && c.x < size && c.y >= 0 && c.y < size) {
            return pixels.at(c.x + c.y * size);
        } else {
            return background;
        }
    }
};

constexpr auto parse_input = [](std::string_view input) -> std::pair<std::bitset<512>, image>
{
    auto newline = input.find("\n\n");
    auto enh_string = [&]{
        std::string str(input.substr(0, newline));
        std::ranges::reverse(str);
        return std::bitset<512>(str, 0, newline, '.', '#');
    }();

    auto image_str = input.substr(newline + 2);

    image img{.size = (int) image_str.find("\n")};

    img.pixels = flow::filter(image_str, flow::pred::neq('\n'))
                    .map(flow::pred::eq('#'))
                    .to_vector();

    return std::pair(std::move(enh_string), std::move(img));
};

constexpr std::array<coord, 9> neighbours = {
    coord{-1, -1}, {0, -1}, {1, -1},
         {-1,  0}, {0,  0}, {1,  0},
         {-1,  1}, {0,  1}, {1,  1}
};

constexpr auto enhance = [](std::bitset<512> const& enh_string, image const& img) -> image
{
    image next{
        .size = img.size + 2,
        .background = img.background ? enh_string.test(511) : enh_string.test(0)
    };

    next.pixels.reserve(next.size * next.size);

    flow::cartesian_product_with(
        [&](int y, int x) {
            auto idx = flow::map(neighbours, [&](coord c) { return img[{c.x + x, c.y + y}]; })
                            .fold([&](uint16_t u, bool b) -> uint16_t {
                                return (u << 1) + b;
                            }, uint16_t{});
            return enh_string.test(idx);
        }, flow::iota(-1, img.size + 1), flow::iota(-1, img.size + 1))
        .output_to(std::back_inserter(next.pixels));

    return next;
};

constexpr auto repeat_enhance = [](int count, std::bitset<512> const& enh_str, image img) -> int
{
    for (int i = 0; i < count; i++) {
        img = enhance(enh_str, std::move(img));
    }
    return flow::count(img.pixels, true);
};

constexpr auto part1 = std::bind_front(repeat_enhance, 2);
constexpr auto part2 = std::bind_front(repeat_enhance, 50);

constexpr std::string_view test_input =
R"(..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..###..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###.######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#..#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#......#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#.....####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.......##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#

#..#.
#....
##..#
..#..
..###)";

int main(int argc, char** argv)
{
    {
        auto const [str, img] = parse_input(test_input);
        assert(part1(str, img) == 35);
        assert(part2(str, img) == 3351);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input :-(");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);

    auto const [str, img] = parse_input(input);

    fmt::print("Part 1: {}\n", part1(str, img));
    fmt::print("Part 2: {}\n", part2(str, img));
}