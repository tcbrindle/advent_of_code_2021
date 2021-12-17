
#include "../common.hpp"

#include <bitset>


struct packet {
    uint8_t version;
    uint8_t type;
    uint64_t literal_value = 0;
    std::vector<packet> subpackets;
};

class bit_reader {
    std::string str_;
    std::size_t pos_ = 0;

    static constexpr auto read_char(char c) -> std::string_view
    {
        switch (c) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: throw std::runtime_error(fmt::format("Unexpected character '{}' in input string", c));
        }
    }

public:
    explicit bit_reader(std::string_view input)
    {
        for (char c : input) {
            str_.append(read_char(c));
        }
    }

    template <std::size_t N>
    constexpr auto read() -> std::bitset<N>
    {
        auto bits = std::bitset<N>(str_, pos_);
        pos_ += N;
        return bits;
    }

    constexpr auto bits_read() const
    {
        return pos_;
    }
};

auto parse_literal(bit_reader& reader) -> uint64_t
{
    uint64_t val = 0;
    bool continue_ = true;

    while (continue_) {
        auto bits = reader.read<5>();
        continue_ = bits[4];
        bits[4] = false;

        val <<= 4;
        val += bits.to_ulong();
    }

    return val;
}

auto parse_packet(bit_reader& reader) -> std::pair<packet, std::size_t>
{
    auto start = reader.bits_read();
    packet p;

    p.version = reader.read<3>().to_ulong();
    p.type = reader.read<3>().to_ulong();

    if (p.type == 4) {
        p.literal_value = parse_literal(reader);
    } else {
        uint8_t len_type_id = reader.read<1>().to_ulong();

        if (len_type_id == 0) {
            auto bits_remaining = reader.read<15>().to_ulong();

            while (bits_remaining > 0) {
                auto [s, b] = parse_packet(reader);
                p.subpackets.push_back(std::move(s));
                bits_remaining -= b;
            }
        } else {
            int n_subs = reader.read<11>().to_ulong();
            for (int i = 0; i < n_subs; i++) {
                p.subpackets.push_back(parse_packet(reader).first);
            }
        }

    }

    return {p, reader.bits_read() - start};
}

auto sum_versions(packet const& p) -> int
{
    return p.version + flow::map(p.subpackets, &sum_versions).sum();
}

auto calculate_value(packet const& p) -> uint64_t
{
    switch (p.type) {
    case 0: return flow::map(p.subpackets, &calculate_value).sum();
    case 1: return flow::map(p.subpackets, &calculate_value).product();
    case 2: return flow::map(p.subpackets, &calculate_value).min().value();
    case 3: return flow::map(p.subpackets, &calculate_value).max().value();
    case 4: return p.literal_value;
    case 5: return calculate_value(p.subpackets.at(0)) > calculate_value(p.subpackets.at(1));
    case 6: return calculate_value(p.subpackets.at(0)) < calculate_value(p.subpackets.at(1));
    case 7: return calculate_value(p.subpackets.at(0)) == calculate_value(p.subpackets.at(1));
    default: throw std::runtime_error(fmt::format("Unexpected packet type id {}", p.type));
    }
}

auto part1(std::string_view input) -> int
{
    auto reader = bit_reader(input);
    auto packet = parse_packet(reader).first;
    return sum_versions(packet);
}

auto part2(std::string_view input) -> uint64_t
{
    auto reader = bit_reader(input);
    auto packet = parse_packet(reader).first;
    return calculate_value(packet);
}

int main(int argc, char** argv)
{
    {
        assert(part1("8A004A801A8002F478") == 16);
        assert(part1("620080001611562C8802118E34") == 12);
        assert(part1("C0015000016115A2E0802F182340") == 23);
        assert(part1("A0016C880162017C3686B18A3D4780") == 31);

        assert(part2("C200B40A82") == 3);
        assert(part2("04005AC33890") == 54);
        assert(part2("880086C3E88112") == 7);
        assert(part2("CE00C43D881120") == 9);
        assert(part2("D8005AC2A8F0") == 1);
        assert(part2("F600BC2D8F") == 0);
        assert(part2("9C005AC2F8F0") == 0);
        assert(part2("9C0141080250320F1802104A08") == 1);
    }

    if (argc < 2) {
        fmt::print(stderr, "No input\n");
        return -1;
    }

    auto const input = [&] {
        auto str = aoc::string_from_file(argv[1]);
        if (str.back() == '\n') {
            str.pop_back();
        }
        return str;
    }();

    fmt::print("Part 1: {}\n", part1(input));
    fmt::print("Part 2: {}\n", part2(input));
}