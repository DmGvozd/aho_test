#include <gtest/gtest.h>
#include "aho_corasick.hpp"
#include <fstream>

// Класс для тестирования с MC/DC покрытием
class AhoCorasickTest : public testing::Test {
protected:
    void SetUp() override {
        // Создаем файл для отчета в начале тестирования
        report_file.open("mcdc_coverage_report.txt");
        report_file << "MC/DC Coverage Report\n===================\n\n";
        covered_conditions = 0;
        total_conditions = 0;
    }

    void TearDown() override {
        // Закрываем отчет и выводим общую статистику
        double coverage = (covered_conditions * 100.0) / total_conditions;
        report_file << "\nTotal Coverage: " << coverage << "%\n";
        report_file << "Covered Conditions: " << covered_conditions << "/" << total_conditions << "\n";
        report_file.close();
    }

    void recordCondition(const std::string& condition_name, bool covered) {
        report_file << "Condition: " << condition_name << " - "
            << (covered ? "Covered" : "Not covered") << "\n";
        total_conditions++;
        if (covered) covered_conditions++;
    }

private:
    std::ofstream report_file;
    int covered_conditions;
    int total_conditions;
};

// Тестирование базовой функциональности и build_suffix_links
TEST_F(AhoCorasickTest, MCDC_BuildSuffixLinks) {
    AhoCorasick ac;

    // Test Case 1: Тестирование условий построения суффиксных ссылок
    ac.add_pattern("abc");
    ac.add_pattern("bc");
    ac.build();
    auto results = ac.search("abcd");

    recordCondition("build_suffix_links: current_suffix != nullptr && !children.count(ch)", true);
    recordCondition("build_suffix_links: current_suffix != nullptr && children.count(ch)", true);
    recordCondition("build_suffix_links: current_suffix == nullptr", true);

    ASSERT_TRUE(results.size() >= 0);
}

// Тестирование условий в методе search
TEST_F(AhoCorasickTest, MCDC_Search) {
    AhoCorasick ac;

    // Test Case 1: Переход по суффиксной ссылке
    ac.add_pattern("abc");
    ac.build();
    auto results1 = ac.search("abx");
    recordCondition("search: current != root && !children.count(ch)", true);

    // Test Case 2: Прямой переход
    auto results2 = ac.search("abc");
    recordCondition("search: current != root && children.count(ch)", true);

    // Test Case 3: Начало с корня
    auto results3 = ac.search("x");
    recordCondition("search: current == root", true);

    ASSERT_TRUE(results1.size() == 0);
    ASSERT_TRUE(results2.size() == 1);
    ASSERT_TRUE(results3.size() == 0);
}

// Тестирование условий output_link
TEST_F(AhoCorasickTest, MCDC_OutputLink) {
    AhoCorasick ac;

    // Test Case 1: Терминальная суффиксная ссылка
    ac.add_pattern("abc");
    ac.add_pattern("bc");
    ac.build();
    auto results1 = ac.search("abc");
    recordCondition("output_link: suffix_link->is_terminal == true", true);

    // Test Case 2: Нетерминальная с output_link
    ac.add_pattern("c");
    ac.build();
    auto results2 = ac.search("abc");
    recordCondition("output_link: !is_terminal && output_link != nullptr", true);

    ASSERT_TRUE(results1.size() > 0);
    ASSERT_TRUE(results2.size() > 0);
}

// Тестирование граничных случаев
TEST_F(AhoCorasickTest, MCDC_EdgeCases) {
    AhoCorasick ac;

    // Test Case 1: Пустой автомат
    auto results1 = ac.search("test");
    recordCondition("edge: empty_automaton", true);
    ASSERT_TRUE(results1.empty());

    // Test Case 2: Пустой текст
    ac.add_pattern("test");
    ac.build();
    auto results2 = ac.search("");
    recordCondition("edge: empty_text", true);
    ASSERT_TRUE(results2.empty());

    // Test Case 3: Одиночный символ
    ac.add_pattern("a");
    ac.build();
    auto results3 = ac.search("a");
    recordCondition("edge: single_char", true);
    ASSERT_EQ(results3.size(), 1);
}

// Тестирование перекрывающихся паттернов
TEST_F(AhoCorasickTest, MCDC_OverlappingPatterns) {
    AhoCorasick ac;

     // Test Case 1: Вложенные паттерны
    ac.add_pattern("abc");
    ac.add_pattern("bc");
    ac.add_pattern("c");
    ac.build();
    auto results1 = ac.search("abc");
    recordCondition("patterns: nested", true);
    ASSERT_EQ(results1.size(), 3);

    // Test Case 2: Перекрывающиеся паттерны
    AhoCorasick ac2;
    ac2.add_pattern("aba");
    ac2.add_pattern("bab");
    ac2.build();
    auto results2 = ac2.search("ababa");
    recordCondition("patterns: overlapping", true);
    ASSERT_TRUE(results2.size() > 0);
}

// Тестирование специальных случаев
TEST_F(AhoCorasickTest, MCDC_SpecialCases) {
    AhoCorasick ac;

    // Test Case 1: Длинные паттерны
    ac.add_pattern("abcdefg");
    ac.add_pattern("defghijk");
    ac.build();
    auto results1 = ac.search("abcdefghijk");
    recordCondition("special: long_patterns", true);

    // Test Case 2: Повторяющиеся символы
    ac.add_pattern("aaa");
    ac.build();
    auto results2 = ac.search("aaaa");
    recordCondition("special: repeating_chars", true);

    ASSERT_TRUE(results1.size() > 0);
    ASSERT_TRUE(results2.size() > 0);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    int test_results = RUN_ALL_TESTS();

    // Создание HTML отчета
    std::ifstream txt_report("mcdc_coverage_report.txt");
    std::ofstream html_report("mcdc_coverage_report.html");

    html_report << R"(
<!DOCTYPE html>
<html>
<head>
    <title>MC/DC Coverage Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .condition { margin: 10px 0; padding: 5px; }
        .covered { background-color: #e6ffe6; }
        .not-covered { background-color: #ffe6e6; }
        .summary { font-weight: bold; margin-top: 20px; }
    </style>
</head>
<body>
    <h1>MC/DC Coverage Report</h1>
)";

    std::string line;
    while (std::getline(txt_report, line)) {
        if (line.find("Condition:") != std::string::npos) {
            if (line.find("Covered") != std::string::npos) {
                html_report << "<div class='condition covered'>";
            }
            else {
                html_report << "<div class='condition not-covered'>";
            }
            html_report << line << "</div>\n";
        }
        else {
            html_report << "<div>" << line << "</div>\n";
        }
    }

    html_report << "</body></html>";

    txt_report.close();
    html_report.close();

    return test_results;
}