#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <compare>
#include <map>
#include <unordered_map>
#include <optional>
#include <cassert>
#include <memory>

#define STR(x) #x
#ifndef DICTIONARY_FILE
#error fuck
#else
const char* file_location = DICTIONARY_FILE;
#endif // !DICTIONARY_FILE

#define fucker asdfdsa:/asdfc

#define strify_(x) # x
#define strify(x) strify_(x)



using std::cout;
using std::cin;
using std::endl;
using std::vector;
using String = std::string;

class CompareResult {
public:
	uint32_t bin;
public:
	auto operator<=>(const CompareResult& right) const = default;
};

namespace std {
	template <>
	struct hash<CompareResult> {
		std::size_t operator()(const CompareResult& k) const {
			return std::hash<uint32_t>()(k.bin);
		}
	};
}

enum class Status : uint8_t {
	invalid,
	green,
	yello,
	grey
};

int splitCount(const vector<String>& words, const String& splitter) {
	uint32_t tester = 0;
	for (auto ch : splitter) {
		tester |= (1 << (ch - 'a'));
	}
	std::unordered_map<CompareResult, int> results;

	for (const auto& word : words) {
		uint32_t cmp_result = 0;
		for (int i = 0; i < word.size(); ++i) {
			Status status;
			auto ch = word[i];
			if (ch == splitter[i]) {
				status = Status::green;
			}
			else if ((1 << (ch - 'a')) & tester) {
				status = Status::yello;
			}
			else {
				status = Status::grey;
			}
			cmp_result |= (uint32_t)status << (i * 2);
		}
		auto key = CompareResult{ cmp_result };
		results[key]++;
	}
	int max_v = 0;
	for (auto [k, v] : results) {
		max_v = std::max(max_v, v);
	}
	return max_v;
}

std::unordered_map<CompareResult, std::vector<String>> split(const vector<String>& words, const String& splitter) {
	uint32_t tester = 0;
	for (auto ch : splitter) {
		tester |= (1 << (ch - 'a'));
	}
	std::unordered_map<CompareResult, std::vector<String>> results;

	for (const auto& word : words) {
		uint32_t cmp_result = 0;
		for (int i = 0; i < word.size(); ++i) {
			Status status;
			auto ch = word[i];
			if (ch == splitter[i]) {
				status = Status::green;
			}
			else if ((1 << (ch - 'a')) & tester) {
				status = Status::yello;
			}
			else {
				status = Status::grey;
			}
			cmp_result |= (uint32_t)status << (i * 2);
		}
		auto key = CompareResult{ cmp_result };
		results[key].push_back(word);
	}
	return results;
}

struct Node {
	vector<String> words;
	std::optional<String> splitter;
	std::unordered_map<CompareResult, std::unique_ptr<Node>> children;	
};


std::unique_ptr<Node> construct(vector<String> words) {
	assert(words.size() > 0);

	if (words.size() == 1) {
		auto node = std::make_unique<Node>();
		node->words = words;
		return node;
	}

	size_t min_split_size = std::numeric_limits<size_t>::max();
	String min_split_word;
	
	for (const auto& word : words) {
		auto local_max_size = splitCount(words, word);
		if (local_max_size < min_split_size) {
			min_split_size = local_max_size;
			min_split_word = word;
		}
	}
	auto min_split_result = split(words, min_split_word);
	auto node = std::make_unique<Node>();
	node->words = std::move(words);
	node->splitter = min_split_word;

	for (auto& [k, vec] : min_split_result) {
		auto sub_node = construct(std::move(vec));
		node->children[k] = std::move(sub_node);
	}

	return node;
}

std::vector<std::string> load() {
	std::ifstream fin(file_location);
	vector<std::string> words;
	std::string line;
	while (std::getline(fin, line)) {
		if (line.size() != 5) {
			continue;
		}
		auto is_word = std::all_of(line.begin(), line.end(), [](char ch) {
			return 'a' <= ch && ch <= 'z';
			});
		if (!is_word) {
			continue;
		}
		words.emplace_back(line);
	}
	for (auto& word : words) {
		cout << word << " ";
	}
	cout << endl << words.size() << endl;
	return words;
}
class DecisionNode {
	std::string word;
};

int main() {
	// std::ifstream fin("");
	cout << strify(fucker);
	auto words = load();
	auto node = construct(words);
	return 0;
}
