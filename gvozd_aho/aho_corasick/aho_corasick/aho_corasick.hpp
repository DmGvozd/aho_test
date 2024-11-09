#pragma once

#include <vector>
#include <queue>
#include <string>
#include <map>
#include <memory>

class AhoCorasick {
private:
    struct Node {
        std::map<char, std::shared_ptr<Node>> children;
        std::shared_ptr<Node> suffix_link;
        std::shared_ptr<Node> output_link;
        bool is_terminal;
        std::string pattern;

        Node() : suffix_link(nullptr), output_link(nullptr), is_terminal(false) {}
    };

    std::shared_ptr<Node> root;

    void build_suffix_links() {
        std::queue<std::shared_ptr<Node>> q;

        for (auto& p : root->children) {
            p.second->suffix_link = root;
            q.push(p.second);
        }

        while (!q.empty()) {
            auto current = q.front();
            q.pop();

            for (auto& p : current->children) {
                char ch = p.first;
                auto child = p.second;

                auto current_suffix = current->suffix_link;
                while (current_suffix && !current_suffix->children.count(ch)) {
                    current_suffix = current_suffix->suffix_link;
                }

                child->suffix_link = current_suffix ?
                    current_suffix->children[ch] : root;

                child->output_link = child->suffix_link->is_terminal ?
                    child->suffix_link : child->suffix_link->output_link;

                q.push(child);
            }
        }
    }

public:
    AhoCorasick() : root(std::make_shared<Node>()) {}

    void add_pattern(const std::string& pattern) {
        auto current = root;

        for (char ch : pattern) {
            if (!current->children.count(ch)) {
                current->children[ch] = std::make_shared<Node>();
            }
            current = current->children[ch];
        }

        current->is_terminal = true;
        current->pattern = pattern;
    }

    void build() {
        build_suffix_links();
    }

    std::vector<std::pair<int, std::string>> search(const std::string& text) {
        std::vector<std::pair<int, std::string>> results;
        auto current = root;

        for (int i = 0; i < text.length(); ++i) {
            char ch = text[i];

            while (current != root && !current->children.count(ch)) {
                current = current->suffix_link;
            }

            if (current->children.count(ch)) {
                current = current->children[ch];

                if (current->is_terminal) {
                    results.emplace_back(i - current->pattern.length() + 1,
                        current->pattern);
                }

                auto output = current->output_link;
                while (output) {
                    results.emplace_back(i - output->pattern.length() + 1,
                        output->pattern);
                    output = output->output_link;
                }
            }
        }

        return results;
    }
};