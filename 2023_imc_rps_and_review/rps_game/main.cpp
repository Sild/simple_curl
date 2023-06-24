#include <charconv>
#include <iostream>

#include "game.hpp"

int main(int argc, char* argv[]) {
    auto game = rps::Game();
    uint32_t roundsCount = 0;
    if (argc > 1) {
        const auto argVal = argv[1];
        const auto end = argVal + strlen(argVal);
        const auto [ptr, err] = std::from_chars(argVal, end, roundsCount);
        if (err != std::errc() || ptr != end) {
            std::cout << "usage: " << argv[0] << " {rounds_count} // default = 0, means infinity" << std::endl;
            return 1;
        }
    }
    game.run(roundsCount);
}
