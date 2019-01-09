#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <iterator>
#include <string_view>
#include <set>

namespace aoc2018::puzzle_01
{
	static std::vector<int> get_data(std::string_view path)
	{
		std::vector<int> freq;
		std::ifstream file(path.data());
		if (file.is_open())
		{
			std::copy(
				std::istream_iterator<int>(file), 
				std::istream_iterator<int>(), 
				std::back_inserter(freq));
		}

		return freq;
	}

	static int part1(std::vector<int> const & freq)
	{
		return std::accumulate(std::cbegin(freq), std::cend(freq), 0);
	}

	static int part2(std::vector<int> const & freq)
	{
		std::set<int> reached;
		size_t i = 0;
		int sum = 0;
		bool found = false;
		while (!found)
		{
			for (size_t i = 0; i < freq.size(); ++i)
			{
				sum += freq[i];
				if (reached.find(sum) != reached.end())
				{
					found = true;
					break;
				}
				else
					reached.insert(sum);
			}
		}

		return sum;
	}

	static void solve()
	{
		auto freq = get_data("data\\advent_01.txt");

		auto f1 = part1(freq);
		std::cout << "part 1: freq=" << f1 << "\n";

		auto f2 = part2(freq);
		std::cout << "part 2: freq=" << f2 << "\n";
	}
}