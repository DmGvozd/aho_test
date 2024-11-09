#include <gtest/gtest.h>
#include "aho_corasick.hpp"

class AhoCorasickTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};


TEST_F(AhoCorasickTest, BasicFunctionality) {
    AhoCorasick ac;
    ac.add_pattern("he");
    ac.add_pattern("she");
    ac.add_pattern("his");
    ac.add_pattern("hers");
    ac.build();

    auto results = ac.search("shers");
    ASSERT_EQ(results.size(), 3);

    bool found_he = false, found_she = false, found_hers = false;
    for (const auto& match : results) {
        if (match.second == "he") found_he = true;
        if (match.second == "she") found_she = true;
        if (match.second == "hers") found_hers = true;
    }

    ASSERT_TRUE(found_he);
    ASSERT_TRUE(found_she);
    ASSERT_TRUE(found_hers);
}

TEST_F(AhoCorasickTest, EmptyPattern) {
    AhoCorasick ac;
    ac.build();
    auto results = ac.search("test");
    ASSERT_TRUE(results.empty());
}

TEST_F(AhoCorasickTest, EmptyText) {
    AhoCorasick ac;
    ac.add_pattern("test");
    ac.build();
    auto results = ac.search("");
    ASSERT_TRUE(results.empty());
}

TEST_F(AhoCorasickTest, SingleCharPattern) {
    AhoCorasick ac;
    ac.add_pattern("a");
    ac.build();
    auto results = ac.search("a");
    ASSERT_EQ(results.size(), 1);
    ASSERT_EQ(results[0].first, 0);
    ASSERT_EQ(results[0].second, "a");
}

TEST_F(AhoCorasickTest, OverlappingPatterns) {
    AhoCorasick ac;
    ac.add_pattern("abc");
    ac.add_pattern("bc");
    ac.add_pattern("c");
    ac.build();

    auto results = ac.search("abc");
    ASSERT_EQ(results.size(), 3);
}

TEST_F(AhoCorasickTest, SuffixLinkTraversal) {
    AhoCorasick ac;
    ac.add_pattern("abc");
    ac.add_pattern("bc");
    ac.add_pattern("c");
    ac.build();

    auto results = ac.search("dabc");
    ASSERT_EQ(results.size(), 3);
}

TEST_F(AhoCorasickTest, OutputLinkTraversal) {
    AhoCorasick ac;
    ac.add_pattern("abc");
    ac.add_pattern("bc");
    ac.add_pattern("c");
    ac.build();

    auto results = ac.search("xabcd");
    ASSERT_EQ(results.size(), 3);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}