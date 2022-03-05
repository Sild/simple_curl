#include <assert.h>
#include <stdio.h>

#include "../src/Matcher.hpp"

void test_star() {
    Matcher sMatcher("1*23*");
    assert(sMatcher.Complient("123"));
    assert(sMatcher.Complient("1xxx23"));
    assert(sMatcher.Complient("1xxx23"));
    assert(sMatcher.Complient("1xxx23re"));
    assert(sMatcher.Complient("123"));
    assert(sMatcher.Complient("1234"));
    assert(sMatcher.Complient("1x234"));
    assert(!sMatcher.Complient("23"));
    assert(!sMatcher.Complient("1zw2x3"));

    sMatcher.SetFilter("***123");
    assert(sMatcher.Complient("x123"));
    assert(sMatcher.Complient("xxx123"));
    assert(!sMatcher.Complient("xxx124"));

    sMatcher.SetFilter("***1**23");
    assert(sMatcher.Complient("x123"));
    assert(sMatcher.Complient("x1xxx23"));
    assert(!sMatcher.Complient("x1xxx2x3"));

    sMatcher.SetFilter("***1**2*3");
    assert(sMatcher.Complient("x123"));
    assert(sMatcher.Complient("x1xxx23"));
    assert(sMatcher.Complient("x1xxx2x3"));

    sMatcher.SetFilter("***1**2*3");
    assert(sMatcher.Complient("x1xxx2xxx3"));

    sMatcher.SetFilter("***1**2*3****");
    assert(sMatcher.Complient("x1xxx2xxx3xx"));
}

void test_question() {
    Matcher sMatcher("?1?23?");
    assert(sMatcher.Complient("x12234"));
    assert(sMatcher.Complient("v1x234"));
    assert(!sMatcher.Complient("v1x2x3"));
    assert(!sMatcher.Complient("b12x34"));
    assert(!sMatcher.Complient("n122345"));
    assert(!sMatcher.Complient("r1222345"));
    assert(!sMatcher.Complient("q1223"));
    assert(!sMatcher.Complient("12233"));
}

void test_set_filter() {
    Matcher sMatcher("123");
    assert(sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));
    sMatcher.SetFilter("456");
    assert(!sMatcher.Complient("123"));
    assert(sMatcher.Complient("456"));

    sMatcher.SetFilter("999");
    assert(!sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));

    sMatcher.SetFilter("123");
    assert(sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));

    sMatcher.SetFilter("123");
    assert(sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));
}

void test_some_cases() {
    Matcher sMatcher("5*7");
    assert(sMatcher.Complient("567"));

    sMatcher.SetFilter("5?7");
    assert(sMatcher.Complient("567"));

    sMatcher.SetFilter("2*");
    assert(sMatcher.Complient("234"));
    assert(!sMatcher.Complient("a234"));

    sMatcher.SetFilter("*2*");
    assert(sMatcher.Complient("234"));
    assert(sMatcher.Complient("a234"));

    sMatcher.SetFilter("*2");
    assert(sMatcher.Complient("2"));
    assert(sMatcher.Complient("a2"));
    assert(sMatcher.Complient("aa2"));
    assert(!sMatcher.Complient("234"));
    assert(!sMatcher.Complient("a234"));

    sMatcher.SetFilter("2*");
    assert(sMatcher.Complient("234"));
    assert(!sMatcher.Complient("a234"));

    sMatcher.SetFilter("4");
    assert(!sMatcher.Complient("234"));
    assert(!sMatcher.Complient("2346"));
    assert(!sMatcher.Complient("a234z"));
}

int main() {
    test_star();
    test_question();
    test_set_filter();
    test_some_cases();

    printf("\033[0;32m");
    printf("matcher tests passed!\n");
    printf("\033[0m");
};