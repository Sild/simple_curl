#include "game.hpp"

#include <charconv>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>

#include "objects.hpp"

void rps::Game::run(uint32_t roundsCount) {
    std::srand((unsigned) time(NULL));

    // welcome message
    std::cout << "Paper-Rock-Scissors game!\n";
    if (roundsCount > 0) {
        std::cout << "Expected rounds count: " << roundsCount << "\n";
    }
    std::cout << "Choose your action:\n";
    for (int i = tools::toUnderlying(Action::Paper); i < tools::toUnderlying(Action::MaxNum); ++i) {
        std::cout << i << " for " << actionsToStr(Action(i)) << "\n";
    }
    std::cout << "===Game begin!===" << std::endl;

    if (roundsCount > 0) {
        while (roundsCount-- && runSingleGame());
    } else {
        while (runSingleGame());
    }

    // buy message
    std::cout << "Final stats: ";
    for (int i = tools::toUnderlying(GameResultForUser::Win); i < tools::toUnderlying(GameResultForUser::MaxNum); ++i) {
        const auto resType = GameResultForUser(i);
        std::cout << gameResultForUserToStr(resType) << "=" << gamesStat[resType] << "; ";
    }
    std::cout << "\nBuy!" << std::endl;
}

bool rps::Game::runSingleGame() {
    const auto userAction = readUserAction();
    std::cout << "Your action: " << actionsToStr(userAction) << "\n";

    if (userAction == Action::Exit) {
        return false;
    }
    const auto botAction = genRandomAction();
    std::cout << "Bot  action: " << actionsToStr(botAction) << "\n";

    const auto gameResultForUser = calcUserResult(userAction, botAction);
    gamesStat[gameResultForUser]++;

    std::cout << "You " << gameResultForUserToStr(gameResultForUser) << "! Next round?\n" << std::endl;
    return true;
}

rps::GameResultForUser rps::calcUserResult(Action userInput, Action botInput) {
    if (userInput == botInput) return GameResultForUser::Draw;
    return GameResultForUser((tools::toUnderlying(userInput) % 3) == tools::toUnderlying(botInput) - 1);
}

rps::Action rps::readUserAction() {
    std::string userActionStr;
    while (true) {
        std::cout << "Your input: ";
        std::getline(std::cin, userActionStr);
        if (std::cin.eof()==1) {
            std::cin.clear();
            std::cin.ignore();
            return Action::Exit;
        }
        const auto userAction = parseAction(userActionStr);
        if (userAction.has_value()) {
            return userAction.value();
        }
        std::cout << "Incorrect input: '" << userActionStr << "', try again" << std::endl; 
    }
    std::cerr << "error: broken logic" << std::endl;
    return Action::Exit;
}

std::optional<rps::Action> rps::parseAction(const std::string& actionStr) {
    int parsed = 0;
    const auto end = actionStr.data() + actionStr.size();
    const auto [ptr, err] = std::from_chars(actionStr.data(), end, parsed);
    // ensure we correctly parsed all string
    if (err != std::errc() || ptr != end) {
        return std::nullopt;
    }
    // and value is valid
    if (parsed < tools::toUnderlying(Action::Paper) || parsed >= tools::toUnderlying(Action::MaxNum)) {
        return std::nullopt;
    }
    return Action(parsed);
}




