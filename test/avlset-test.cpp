#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "../include/avl-set.hpp"
#include "doctest.h"

int getRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1e9);
    return dist(gen);
}

std::string getRandomString() {
    char c[] = {'a', 'b', 'c',  'd', 'e', 'f', 'g', 'h', 'i', 'j',  'k', 'l',
                'm', 'n', 'o',  'p', 'q', 'r', 's', 't', 'u', 'v',  'w', 'x',
                'y', 'z', 'A',  'B', 'C', 'D', 'E', 'F', 'G', 'H',  'I', 'J',
                'K', 'L', 'M',  'N', 'O', 'P', 'Q', 'R', 'S', 'T',  'U', 'V',
                'W', 'X', 'Y',  'Z', '0', '1', '2', '3', '4', '5',  '6', '7',
                '8', '9', ' ',  '!', '"', '#', '$', '%', '&', '\'', '(', ')',
                '*', '+', ',',  '-', '.', '/', ':', ';', '<', '=',  '>', '?',
                '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}',  '~', '\0'};
    int n = getRandomNumber() % 1000;
    std::string res;
    for (int i = 0; i < n; ++i) {
        int ind = getRandomNumber() % 96;
        res += c[ind];
    }
    return res;
}

template <typename IterA, typename IterB>
bool bounds_equal(IterA it_a, IterA end_a, IterB it_b, IterB end_b) {
    if (it_a == end_a && it_b == end_b) {
        return true;  // оба end()
    }
    if (it_a == end_a || it_b == end_b) {
        return false;  // один end(), другой нет
    }
    return *it_a == *it_b;
}

using my_algorithms::AvlSet;

TEST_CASE("AvlSet basic operations") {
    AvlSet<int> a;
    CHECK(a.empty());
    a.insert(13);
    a.insert(7);
    a.insert(-12432);
    CHECK(a.size() == 3);
    a.insert(56);
    a.insert(23554);
    CHECK(a.size() == 5);
    CHECK(a.empty() == false);
    a.erase(-1);
    CHECK(a.size() == 5);
    a.erase(13);
    a.erase(7);
    a.erase(-12432);
    a.erase(56);
    a.erase(23554);
    CHECK(a.empty());
    CHECK(a.size() == 0);

    for (int i = 0; i < 10; ++i) {
        a.insert(getRandomNumber());
    }
    a.clear();
    CHECK(a.empty());
    CHECK(a.size() == 0);
}

TEST_CASE("AvlSet basic operations (compare with std::set)") {
    AvlSet<int> a;
    std::set<int> b;
    for (int i = 0; i < 1'000'000; ++i) {
        int val = getRandomNumber() % 300'000;
        a.insert(val);
        b.insert(val);
    }

    for (int i = 0; i < 100'000; ++i) {
        int val = getRandomNumber() % 600'000;
        a.erase(val);
        b.erase(val);
        // CHECK(a.size() == b.size());
    }

    std::vector<int> aa, bb;
    for (int i : a) {
        aa.push_back(i);
    }
    for (int i : b) {
        bb.push_back(i);
    }
    CHECK_EQ(aa, bb);
}

TEST_CASE("Check upper_bound functions") {
    AvlSet<int> a;
    std::set<int> b;
    for (int i = 0; i < 100'000; ++i) {
        int val = getRandomNumber() % 300'000;
        a.insert(val);
        b.insert(val);

        val = getRandomNumber() % 600'000;
        a.erase(val);
        b.erase(val);

        val = getRandomNumber() % 600'000;
        CHECK(bounds_equal(
            a.upper_bound(val), a.end(), b.upper_bound(val), b.end()
        ));
    }
}

TEST_CASE("Check lower_bound functions") {
    AvlSet<int> a;
    std::set<int> b;
    for (int i = 0; i < 100'000; ++i) {
        int val = getRandomNumber() % 300'000;
        a.insert(val);
        b.insert(val);

        val = getRandomNumber() % 600'000;
        a.erase(val);
        b.erase(val);

        val = getRandomNumber() % 600'000;
        CHECK(bounds_equal(
            a.lower_bound(val), a.end(), b.lower_bound(val), b.end()
        ));
    }
}

template <typename T>
struct EvilComparator {
    bool operator()(const T &a, const T &b) const {
        // Инвертируем обычный оператор <
        return b < a;
    }
};

TEST_CASE("Check key = std::string with castom compare") {
    AvlSet<std::string, EvilComparator<std::string>> a;
    std::set<std::string, EvilComparator<std::string>> b;
    for (int i = 0; i < 100'000; ++i) {
        std::string val = getRandomString();
        a.insert(val);
        b.insert(val);

        CHECK(bounds_equal(
            a.upper_bound(val), a.end(), b.upper_bound(val), b.end()
        ));
        CHECK(bounds_equal(
            a.lower_bound(val), a.end(), b.lower_bound(val), b.end()
        ));
    }
    std::vector<std::string> aa, bb;
    for (auto i : a) {
        aa.push_back(i);
    }
    for (auto i : b) {
        bb.push_back(i);
    }
    CHECK_EQ(aa, bb);
}

TEST_CASE("Check contains (compare with std::set)") {
    AvlSet<int> a;
    std::set<int> b;
    for (int i = 0; i < 100'000; ++i) {
        int val = getRandomNumber() % 100'000;
        a.insert(val);
        b.insert(val);
    }

    for (int i = 0; i < 10'000; ++i) {
        int val = getRandomNumber() % 200'000;
        CHECK(a.contains(val) == (b.find(val) != b.end()));
    }
}

TEST_CASE("Check clear method") {
    AvlSet<int> a;
    std::set<int> b;

    for (int i = 0; i < 50000; ++i) {
        int val = getRandomNumber() % 100000;
        a.insert(val);
        b.insert(val);
    }

    a.clear();
    b.clear();

    CHECK(a.empty());
    CHECK(b.empty());
    CHECK_EQ(a.size(), 0);
    CHECK_EQ(a.begin(), a.end());
}

TEST_CASE("Check insert duplicates (compare with std::set)") {
    AvlSet<int> a;
    std::set<int> b;

    for (int i = 0; i < 10000; ++i) {
        int val = getRandomNumber() % 1000;
        a.insert(val);
        b.insert(val);
    }

    std::vector<int> aa(a.begin(), a.end());
    std::vector<int> bb(b.begin(), b.end());
    CHECK_EQ(aa, bb);
}

TEST_CASE("Check erase correctness") {
    AvlSet<int> a;
    std::set<int> b;

    std::vector<int> inserted;
    for (int i = 0; i < 10000; ++i) {
        int val = getRandomNumber() % 20000;
        a.insert(val);
        b.insert(val);
        inserted.push_back(val);
    }

    for (int i = 0; i < 5000; ++i) {
        int val = inserted[getRandomNumber() % inserted.size()];
        a.erase(val);
        b.erase(val);
    }

    std::vector<int> aa(a.begin(), a.end());
    std::vector<int> bb(b.begin(), b.end());
    CHECK_EQ(aa, bb);
}

TEST_CASE("Check begin and end iterators") {
    AvlSet<int> a;
    std::set<int> b;

    for (int i = 0; i < 1000; ++i) {
        int val = getRandomNumber() % 10000;
        a.insert(val);
        b.insert(val);
    }

    if (!a.empty() && !b.empty()) {
        // сравнение минимального значения
        CHECK_EQ(*a.begin(), *b.begin());

        // сравнение максимального значения (через --end)
        auto it_a = a.end();
        --it_a;
        auto it_b = b.end();
        --it_b;
        CHECK_EQ(*it_a, *it_b);
    }
}

TEST_CASE("Check find method (compare with std::set)") {
    AvlSet<int> a;
    std::set<int> b;

    for (int i = 0; i < 20000; ++i) {
        int val = getRandomNumber() % 100000;
        a.insert(val);
        b.insert(val);
    }

    for (int i = 0; i < 5000; ++i) {
        int val = getRandomNumber() % 200000;
        CHECK(bounds_equal(a.find(val), a.end(), b.find(val), b.end()));
    }
}

TEST_CASE("Check empty and size through operations") {
    AvlSet<int> a;
    CHECK(a.empty());
    CHECK_EQ(a.size(), 0);

    for (int i = 0; i < 5000; ++i) {
        a.insert(i);
    }

    CHECK(!a.empty());
    CHECK_EQ(a.size(), 5000);

    for (int i = 0; i < 2500; ++i) {
        a.erase(i);
    }

    CHECK_EQ(a.size(), 2500);
}

TEST_CASE("Check iterator correctness and ordering") {
    AvlSet<int> a;
    std::set<int> b;

    for (int i = 0; i < 1000; ++i) {
        int val = getRandomNumber() % 5000;
        a.insert(val);
        b.insert(val);
    }

    auto it_a = a.begin();
    auto it_b = b.begin();
    while (it_a != a.end() && it_b != b.end()) {
        CHECK_EQ(*it_a, *it_b);
        ++it_a;
        ++it_b;
    }
    CHECK(it_a == a.end());
    CHECK(it_b == b.end());
}



TEST_CASE("Check swap method") {
    AvlSet<int> a, b;
    for (int i = 0; i < 100; ++i) {
        a.insert(i);
    }
    for (int i = 100; i < 200; ++i) {
        b.insert(i);
    }

    a.swap(b);

    for (int i = 0; i < 100; ++i) {
        CHECK(b.contains(i));
        CHECK(!a.contains(i));
    }

    for (int i = 100; i < 200; ++i) {
        CHECK(a.contains(i));
        CHECK(!b.contains(i));
    }
}

// TEST_CASE("Check reverse iterators rbegin and rend") {
//     AvlSet<int> a;
//     for (int i = 0; i < 1000; ++i) {
//         a.insert(i);
//     }

//     auto rit = a.rbegin();
//     int expected = 999;
//     while (rit != a.rend()) {
//         CHECK_EQ(*rit, expected--);
//         ++rit;
//     }
//     CHECK(expected == -1);
// }
