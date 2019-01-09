#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <regex>
#include <assert.h>

namespace aoc2018::puzzle_03
{
	struct claim
	{
		int id;
		int left;
		int top;
		int width;
		int height;
	};

	using claims = std::vector<claim>;

	static claims get_data(std::string_view path)
	{
		claims data;

		std::ifstream file(path.data());
		if (file.is_open())
		{
			std::regex rx(R"(#(\d+) @ (\d+),(\d+): (\d+)x(\d+))");
			std::smatch match;
			std::string line;
			while (std::getline(file, line))
			{
				if (std::regex_match(line, match, rx))
				{
					claim cl{
						std::stoi(match[1].str()),
						std::stoi(match[2].str()),
						std::stoi(match[3].str()),
						std::stoi(match[4].str()),
						std::stoi(match[5].str()) };

					data.push_back(cl);
				}
			}
		}

		return data;
	}

	static int find_overlapping_area(claims const & data, size_t const size)
	{
		std::vector<char> area(size*size, 0);

		for (auto const & cl : data)
		{
			for (int i = cl.top; i < cl.top + cl.height; ++i)
			{
				for (int j = cl.left; j < cl.left + cl.width; ++j)
				{
					auto pos = i * size + j;
					if (area[pos] == '\0') area[pos] = 'o';
					else if (area[pos] == 'o') area[pos] = 'x';
				}
			}
		}

		return static_cast<int>(std::count(std::cbegin(area), std::cend(area), 'x'));
	}

	static std::set<int> find_non_overlapping_claims(claims const & data, size_t const size)
	{
		std::vector<int> area(size*size, 0);
		std::set<int> ids;

		for (auto const & cl : data)
		{
			bool overlaps = false;
			for (int i = cl.top; i < cl.top + cl.height; ++i)
			{
				for (int j = cl.left; j < cl.left + cl.width; ++j)
				{
					auto pos = i * size + j;
					if (area[pos] == 0) area[pos] = cl.id;
					else if (area[pos] > 0)
					{
						ids.erase(area[pos]);
						area[pos] = -1;
						overlaps = true;
					}
					else overlaps = true;
				}
			}

			if (!overlaps) ids.insert(cl.id);
		}

		return ids;
	}

	static void solve()
	{
		assert(find_overlapping_area(get_data(R"(data\advent_03_test.txt)"), 1000) == 4);

		auto data = get_data(R"(data\advent_03.txt)");
		auto area = find_overlapping_area(data, 1000);
		std::cout << "part 1: area = " << area << '\n';

		auto ids = find_non_overlapping_claims(data, 1000);
		assert(ids.size() == 1);
		std::cout << "part 2: id = " << *ids.begin() << '\n';
	}
}