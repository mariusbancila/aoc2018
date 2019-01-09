#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>

namespace aoc2018::puzzle_06
{
	struct point
	{
		int x;
		int y;
	};

	inline int manhattan_distance(point const & p1, point const & p2) noexcept
	{
		return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
	}

	using points = std::vector<point>;

	static points get_data(std::string_view path)
	{
		points pts;

		std::ifstream file(path.data());
		if (file.is_open())
		{
			std::regex rx(R"((\d+),\s*(\d+))");
			std::smatch match;
			std::string line;
			while (std::getline(file, line))
			{
				if (std::regex_match(line, match, rx))
				{
					point pt
					{
						std::stoi(match[1].str()),
						std::stoi(match[2].str())
					};

					pts.push_back(pt);
				}
			}
		}

		return pts;
	}

	static std::pair<int, int> find_grid_size(points const & data)
	{
		int x = 0, y = 0;
		for (auto const & p : data)
		{
			if (p.x > x)
			{
				x = p.x;
			}
			if (p.y > y)
			{
				y = p.y;
			}
		}

		return { x, y };
	}

	static std::vector<std::pair<int, int>> build_voronoi(points const & data, int const cx, int const cy)
	{
		std::vector<std::pair<int, int>> voronoi(cx*cy, { 0, std::numeric_limits<int>::max() });

		for (size_t k = 1; k <= data.size(); ++k)
		{
			for (int i = 0; i < cy; ++i)
			{
				for (int j = 0; j < cx; ++j)
				{
					auto md = manhattan_distance({ j, i }, data[k - 1]);
					if (md < voronoi[i * cx + j].second)
					{
						voronoi[i * cx + j] = { static_cast<int>(k), md };
					}
					else if (md == voronoi[i * cx + j].second)
					{
						voronoi[i * cx + j].first = 0;
					}
				}
			}
		}

		return voronoi;
	}

	static int find_max_voronoi(std::vector<std::pair<int, int>> voronoi, int const cx, int const cy)
	{
		int area = 0;

		std::set<int> edges;

		for (int j = 0; j < cx; ++j)
		{
			edges.insert(voronoi[j].first);
			edges.insert(voronoi[(cy - 1) * cx + j].first);
		}

		for (int i = 0; i < cy; ++i)
		{
			edges.insert(voronoi[i*cx].first);
			edges.insert(voronoi[(i*cx) + cx - 1].first);
		}

		std::map<int, int> data;
		for (auto const & kvp : voronoi)
		{
			if (edges.find(kvp.first) == std::end(edges))
			{
				data[kvp.first]++;
			}
		}

		area = std::max_element(std::cbegin(data), std::cend(data), [](auto const & kvp1, auto const & kvp2) {return kvp1.second < kvp2.second; })->second;

		return area;
	}

	static std::vector<int> build_accumulated_manhattan(points const & data, int const cx, int const cy)
	{
		std::vector<int> result(cx*cy, 0);

		for (size_t k = 1; k <= data.size(); ++k)
		{
			for (int i = 0; i < cy; ++i)
			{
				for (int j = 0; j < cx; ++j)
				{
					auto md = manhattan_distance({ j, i }, data[k - 1]);
					result[i * cx + j] += md;
				}
			}
		}

		return result;
	}

	static void solve()
	{
		{
			auto data = get_data(R"(data\advent_06_test.txt)");
			auto [cx, cy] = find_grid_size(data);
			assert(cx == 8 && cy == 9);

			auto voronoi = build_voronoi(data, cx+1, cy+1);

			for (int i = 0; i < cy + 1; ++i)
			{
				for (int j = 0; j < cx + 1; j++)
				{
					if (voronoi[i*(cx + 1) + j].first == 0)
						std::cout << '.';
					else
						std::cout << static_cast<char>('a' - 1 + voronoi[i*(cx + 1) + j].first);
				}
				std::cout << '\n';
			}

			auto area = find_max_voronoi(voronoi, cx+1, cy+1);
			assert(area == 17);

			auto result = build_accumulated_manhattan(data, cx+1, cy+1);
			for (int i = 0; i < cy + 1; ++i)
			{
				for (int j = 0; j < cx + 1; j++)
				{
					if (result[i*(cx + 1) + j] < 32)
						std::cout << '#';
					else
						std::cout << '.';
				}
				std::cout << '\n';
			}

			auto size = std::count_if(std::begin(result), std::end(result), [](int const s) {return s < 32; });
			assert(size == 16);
		}
		
		{
			auto data = get_data(R"(data\advent_06.txt)");
			auto[cx, cy] = find_grid_size(data);

			auto voronoi = build_voronoi(data, cx + 1, cy + 1);
			auto area = find_max_voronoi(voronoi, cx + 1, cy + 1);
			
			std::cout << "part 1: area = " << area << '\n';

			auto result = build_accumulated_manhattan(data, cx + 1, cy + 1);
			auto size = std::count_if(std::begin(result), std::end(result), [](int const s) {return s < 10000; });

			std::cout << "part 2: area = " << size << '\n';
		}
		
	}
}