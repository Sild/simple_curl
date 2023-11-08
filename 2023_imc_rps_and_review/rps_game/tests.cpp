
#include <iostream>
#include <random>

#include "game.hpp"
#include "objects.hpp"

using namespace rps;

namespace {
    enum class T1 {
        Foo = 1,
        Bar = 2,
        Low = 5
    };
}

void testToUnderlying() {
    auto a = T1::Foo;
    assert(tools::toUnderlying(T1::Foo) == 1);
    assert(tools::toUnderlying(T1::Bar) == 2);
    assert(tools::toUnderlying(T1::Low) == 5);
}

void testObjectsSerialization() {
    // it's more for corner-cases if new values appear, then real for tests
    auto act = Action::Paper;
    assert(actionsToStr(act) == std::string("paper"));
    act = Action::Exit;
    assert(actionsToStr(act) == std::string("exit"));

    auto gr = GameResultForUser::Loose;
    assert(gameResultForUserToStr(gr) == std::string("loose"));
    gr = GameResultForUser::Draw;
    assert(gameResultForUserToStr(gr) == std::string("have a draw"));
}

void testRandValues() {
    std::srand((unsigned) time(NULL));

    for (int i = 0; i < 3000; ++i) {
        auto action = genRandomAction();
        if (
            tools::toUnderlying(action) < tools::toUnderlying(Action::Paper)
            || tools::toUnderlying(action) > tools::toUnderlying(Action::Scissors))
        {
            assert(false);
        }
    }
}

void testParseAction() {
    std::string input;
    std::optional<Action> act;

    // positive cases
    input = "1";
    act = parseAction(input);
    assert(act.has_value());
    assert(act.value() == Action::Paper);

    input = "2";
    act = parseAction(input);
    assert(act.has_value());
    assert(act.value() == Action::Rock);

    input = "3";
    act = parseAction(input);
    assert(act.has_value());
    assert(act.value() == Action::Scissors);

    input = "4";
    act = parseAction(input);
    assert(act.has_value());
    assert(act.value() == Action::Exit);

    input = "+3"; // Suddenly doesn't work. Generally it's fine, just nail down this behaviour
    act = parseAction(input);
    assert(!act.has_value());

    // negative cases
    auto negativeCases = std::vector{"-10", "-1", "0", "5", "a", "12av", "a12", "9999999999999999999", "*l42", "+12", "(_);", "cat /etc/hosts"};
    for (const auto& str: negativeCases) {
        act = parseAction(str);
        assert(!act.has_value());
    }

}

void testCalcUserResult() {
    assert(GameResultForUser::Draw == calcUserResult(Action::Paper, Action::Paper));
    assert(GameResultForUser::Win == calcUserResult(Action::Paper, Action::Rock));
    assert(GameResultForUser::Loose == calcUserResult(Action::Paper, Action::Scissors));

    assert(GameResultForUser::Loose == calcUserResult(Action::Rock, Action::Paper));
    assert(GameResultForUser::Draw == calcUserResult(Action::Rock, Action::Rock));
    assert(GameResultForUser::Win == calcUserResult(Action::Rock, Action::Scissors));

    assert(GameResultForUser::Win == calcUserResult(Action::Scissors, Action::Paper));
    assert(GameResultForUser::Loose == calcUserResult(Action::Scissors, Action::Rock));
    assert(GameResultForUser::Draw == calcUserResult(Action::Scissors, Action::Scissors));
}


int main() {
    testToUnderlying();
    testObjectsSerialization();
    testRandValues();
    testParseAction();
    testCalcUserResult();
}
