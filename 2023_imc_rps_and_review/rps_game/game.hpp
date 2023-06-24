#pragma once

#include <string>
#include <unordered_map>

namespace rps {
    enum class Action;
    enum class GameResultForUser;

    class Game {
        public:
            void run(uint32_t roundsCount);
        private:
            bool runSingleGame();
            std::unordered_map<GameResultForUser, uint32_t> gamesStat;
            

    };

    std::optional<Action> parseAction(const std::string& actionStr);
    Action readUserAction();
    GameResultForUser calcUserResult(Action userInput, Action botInput);
};