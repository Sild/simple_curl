#include <assert.h>
#include <stdio.h>

#include "../src/Matcher.hpp"

void test_begin_end() {
    Matcher sMatcher("^123$");
    assert(sMatcher.Complient("123"));
    assert(!sMatcher.Complient("1123"));
    assert(!sMatcher.Complient("1233"));
    assert(!sMatcher.Complient("1"));
    assert(!sMatcher.Complient("12"));
    assert(!sMatcher.Complient("23"));
};

void test_asterisk() {
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

    sMatcher.SetFilter("");
    assert(sMatcher.Complient("123"));
    assert(sMatcher.Complient("456"));

    sMatcher.SetFilter(NULL);
    assert(sMatcher.Complient("123"));
    assert(sMatcher.Complient("456"));

    sMatcher.SetFilter("999");
    assert(!sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));

    sMatcher.SetFilter("123");
    assert(sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));

    sMatcher.SetFilter('\0');
    assert(sMatcher.Complient("123"));
    assert(sMatcher.Complient("456"));

    sMatcher.SetFilter("123");
    assert(sMatcher.Complient("123"));
    assert(!sMatcher.Complient("456"));
}

int main() {
    test_begin_end();
    test_asterisk();
    test_question();
    test_set_filter();

    printf("\033[0;32m");
    printf("matcher tests passed!\n");
    printf("\033[0m");
};