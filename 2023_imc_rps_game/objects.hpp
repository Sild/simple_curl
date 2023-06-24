#pragma once

#include <string>
#include <vector>

namespace rps
{
    namespace tools
    {
        template <typename T>
        constexpr auto toUnderlying(const T &val)
        {
            return static_cast<std::underlying_type_t<T>>(val);
        }
    }

    enum class Action : int
    {
        Paper = 1,
        Rock,
        Scissors,
        Exit,
        MaxNum,
    };
    const std::string &actionsToStr(Action act)
    {
        const static auto strings = std::vector<std::string>{"", "paper", "rock", "scissors", "exit", "undefined"};
        return strings[tools::toUnderlying(act)];
    }
    Action genRandomAction() {
        return Action(rand() % 3 + 1);
    }

    enum class GameResultForUser : int
    {
        Loose = 0,
        Win,
        Draw,
        MaxNum
    };
    const std::string &gameResultForUserToStr(GameResultForUser act)
    {
        const static auto strings = std::vector<std::string>{"loose", "win", "have a draw", "undefined"};
        return strings[tools::toUnderlying(act)];
    }
}