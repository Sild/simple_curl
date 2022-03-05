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
    Matcher sMatcher("1.*23.*");
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

int main() {
    test_begin_end();
    test_asterisk();
    printf("\033[0;32m");
    printf("matcher tests passed!\n");
    printf("\033[0m");
};