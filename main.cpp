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
#include <fmt/format.h>
#include <fmt/color.h>
#include <stdexcept>
#include <random>

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

enum class Status : uint8_t {
	invalid,
	green,
	yellow,
	grey
};

class CompareResult {
private:
	uint32_t bin = 0;
	
public:
	auto operator<=>(const CompareResult& right) const = default;
	auto get_status(int index) const {
		return (Status)((bin >> (8 - index * 2) ) & 0b11);
	}

	auto set_status(Status status, int index) {
		bin |= (uint32_t)status << (8 - index * 2);
	}
	auto get_blob() const {
		return bin;
	}
};

namespace std {
	template <>
	struct hash<CompareResult> {
		std::size_t operator()(const CompareResult& k) const {
			return std::hash<uint32_t>()(k.get_blob());
		}
	};
}



int splitCount(const vector<String>& words, const String& splitter) {
	std::unordered_map<CompareResult, int> results;
	for (const auto& word : words) {
		uint32_t tester = 0;
		for (auto ch : word) {
			tester |= (1 << (ch - 'a'));
		}
		CompareResult key;
		for (int i = 0; i < word.size(); ++i) {
			Status status;
			auto ch = word[i];
			auto ref_ch = splitter[i];
			if (ch == ref_ch) {
				status = Status::green;
			}
			else if ((1 << (ref_ch - 'a')) & tester) {
				status = Status::yellow;
			}
			else {
				status = Status::grey;
			}
			key.set_status(status, i);
		}
		results[key]++;
	}
	int max_v = 0;
	for (auto [k, v] : results) {
		max_v = std::max(max_v, v);
	}
	return max_v;
}

std::unordered_map<CompareResult, std::vector<String>> split(const vector<String>& words, const String& splitter) {

	std::unordered_map<CompareResult, std::vector<String>> results;

	for (const auto& word : words) {
		uint32_t tester = 0;
		for (auto ch : word) {
			tester |= (1 << (ch - 'a'));
		}
		CompareResult key;
		for (int i = 0; i < word.size(); ++i) {
			Status status;
			auto ch = word[i];
			auto ref_ch = splitter[i];
			if (ch == ref_ch) {
				status = Status::green;
			}
			else if ((1 << (ref_ch - 'a')) & tester) {
				status = Status::yellow;
			}
			else {
				status = Status::grey;
			}
			key.set_status(status, i);
		}
		results[key].push_back(word);
	}
	return results;
}

struct Node {
	vector<String> words;
	std::optional<String> splitter;
	std::map<CompareResult, std::unique_ptr<Node>> children;
};


std::unique_ptr<Node> construct(vector<String> words, const vector<String>& split_words) {
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

	if (split_words.size() != words.size()) {
		for (const auto& word : split_words) {
			auto local_max_size = splitCount(words, word);
			if (local_max_size < min_split_size) {
				min_split_size = local_max_size;
				min_split_word = word;
			}
		}
	}

	auto min_split_result = split(words, min_split_word);
	auto node = std::make_unique<Node>();
	node->words = std::move(words);
	node->splitter = min_split_word;

	for (auto& [k, vec] : min_split_result) {
		auto sub_node = construct(std::move(vec), split_words);
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
	cout << endl << words.size() << endl;
	return words;
}
class DecisionNode {
	std::string word;
};

String wrap(char ch, Status status) {
	fmt::text_style style;
	switch (status)
	{
	case Status::green:
		style = fmt::bg(fmt::color::green);
		break;
	case Status::yellow:
		style = fmt::bg(fmt::color::yellow);
		break;
	case Status::grey:
		style = fmt::bg(fmt::color::gray);
		break;
	default:
		throw std::logic_error("wtf");
	}
	style = style | fmt::fg(fmt::color::black);
	return fmt::format(style, FMT_STRING("{}"), ch);
}
class Printer {
public:
	Printer(int count, std::ostream& out) : count(count), out(out) {}

	void print_with_color(const String& prefix, const Node& node, const String& last_word) {
		assert(!node.words.empty());
		if (node.words.size() == 1) {
			auto word = node.words[0];
			String tail;
			if (last_word != word) {
				tail = "->";
				tail += fmt::format(fmt::bg(fmt::color::green) | fmt::fg(fmt::color::black), FMT_STRING("{}"), word);
			}
			++count;
			out << count << ": " << prefix << tail << endl;
			return;
		}
		assert(node.splitter);
		auto split_word = node.splitter.value();
		assert(split_word.size() == 5);
		for (auto& [k, v] : node.children) {
			String colored_output;
			assert(v);
			for (auto i = 0; i < 5; ++i) {
				auto status = k.get_status(i);
				colored_output += wrap(split_word[i], status);
			}
			print_with_color(fmt::format(FMT_STRING("{}->{}"), prefix, colored_output), *v, split_word);
		}
		return;
	}
private:
	int count = 0;
	std::ostream& out;
};

int main() {
	// std::ifstream fin("");
	cout << "\033[1;31mloading data\033[0m\n" << endl;
	auto words = load();
	// std::default_random_engine e(42);
	// std::shuffle(words.begin(), words.end(), e);
	// vector<String> words = { "lores","voles", "puses"};
	// words.resize(100);
	cout << "\033[1;31mloaded data\033[0m\n" << endl;
	auto node = construct(words, words);
	Printer(0, cout).print_with_color("$", *node, "");
	return 0;
}
