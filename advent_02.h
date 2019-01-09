#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>

namespace aoc2018::puzzle_02
{
	static std::vector<std::string> get_data(std::string_view path)
	{
		std::vector<std::string> ids;
		std::ifstream file(path.data());
		if (file.is_open())
		{
			std::copy(
				std::istream_iterator<std::string>(file),
				std::istream_iterator<std::string>(),
				std::back_inserter(ids));
		}

		return ids;
	}

	static std::pair<bool, bool> scan_box(std::string id)
	{
		std::map<char, int> m;
		for (auto const c : id)
			m[c]++;

		bool has2 = false, has3 = false;
		for (auto it = std::cbegin(m); 
			!(has2 && has3) && it != std::cend(m);
			++it)
		{
			has2 = has2 || it->second == 2;
			has3 = has3 || it->second == 3;
		}

		return { has2, has3 };
	}

	static int get_checksum(std::vector<std::string> const & ids)
	{ 
		int count2s = 0;
		int count3s = 0;
		for (auto const & id : ids)
		{
			auto[has2, has3] = scan_box(id);
			count2s += (has2 ? 1 : 0);
			count3s += (has3 ? 1 : 0);
		}

		return count2s * count3s;
	}

	static std::pair<int, std::string> diff_ids(std::string_view s1, std::string_view s2)
	{
		if (s1.size() != s2.size())
			throw std::runtime_error("ids must have equal size!");

		int count = 0;
		std::string diff;
		for (size_t i = 0; i < s1.size(); ++i)
			if (s1[i] != s2[i]) count++;
			else diff += s1[i];

		return { count, diff};
	}

	static std::string get_common_letters(std::vector<std::string> const & ids)
	{
		int mindiff = std::numeric_limits<int>::max();
		std::string diff;
		for (size_t i = 0; i < ids.size() - 1; ++i)
		{
			for (size_t j = i+1; j < ids.size(); ++j)
			{
				auto[count, d] = diff_ids(ids[i], ids[j]);
				if (count < mindiff)
				{
					mindiff = count;
					diff = d;
				}
			}
		}

		return diff;
	}

	static void solve()
	{
		assert(get_checksum(get_data(R"(data\advent_02_1_test.txt)")) == 12);

		auto ids1 = get_data(R"(data\advent_02_1.txt)");
		std::cout << "part 1: crc =" << get_checksum(ids1) << '\n';

		assert(get_common_letters(get_data(R"(data\advent_02_2_test.txt)")) == "fgij");

		auto ids2 = get_data(R"(data\advent_02_2.txt)");
		std::cout << "part 2: common =" << get_common_letters(ids2) << '\n';
	}
}