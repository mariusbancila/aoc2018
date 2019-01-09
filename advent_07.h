#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>

namespace aoc2018::puzzle_07
{
	using namespace std;

	using graph = std::multimap<char, char>;

	static graph get_data(std::string_view path)
	{
		graph g;

		std::ifstream file(path.data());
		if (file.is_open())
		{
			std::regex rx(R"(Step (\w) must be finished before step (\w) can begin.)");
			std::smatch match;
			std::string line;
			while (std::getline(file, line))
			{
				if (std::regex_match(line, match, rx))
				{
					char f = match[1].str().at(0);
					char s = match[2].str().at(0);

					g.insert({ s, f });
				}
			}
		}

		return g;
	}

	std::set<char> reduce_tasks(std::set<char> chars, graph const & g)
	{
		for (auto const & kvp : g) 
		{			
			chars.erase(kvp.first);
		}

		return chars;
	}

	graph reduce_graph(char const c, graph const & g)
	{
		graph newg;

		for (auto const & kvp : g) 
		{
			if (kvp.second != c)
				newg.insert(kvp);
		}

		return newg;
	}

	static std::string find_order(graph g)
	{
		std::string order;
		std::set<char> tasks;

		for (auto const & kvp : g)
		{
			tasks.insert(kvp.first);
			tasks.insert(kvp.second);
		}

		while (!tasks.empty())
		{
			auto available = reduce_tasks(tasks, g);

			auto task = *std::begin(available);
			order.push_back(task);
			g = reduce_graph(task, g);

			tasks.erase(task);
		}

		return order;
	}

	static void solve()
	{
		{
			auto data = get_data(R"(data\advent_07_test.txt)");
			auto order = find_order(data);
			assert(order == "CABDFE");
		}
		
		{
			auto data = get_data(R"(data\advent_07.txt)");
			auto order = find_order(data);
			
			std::cout << "part 1: " << order << '\n';
		}
	}
}