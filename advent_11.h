#include <iostream>
#include <vector>
#include <numeric>
#include <tuple>
#include <assert.h>

namespace aoc2018::puzzle_11
{
	constexpr int compute_power_level(size_t const x, size_t const y, int const sn)
	{
		auto rackid = x + 10;
		auto power = rackid * (rackid * y + sn);
		auto digit = (power % 1000) / 100;
		return static_cast<int>(digit) - 5;
	}

	static std::vector<int> compute_power_grid(size_t const size, int const sn)
	{
		std::vector<int> grid(size*size, 0);

		for (size_t y = 0; y < size; ++y)
		{
			for (size_t x = 0; x < size; ++x)
			{
				grid[y * size + x] = compute_power_level(x + 1, y + 1, sn);
			}
		}

		return grid;
	}

	static std::tuple<size_t, size_t, int> find_max_subgrid(std::vector<int> const & grid, size_t const size, size_t const subsize)
	{
		int power = std::numeric_limits<int>::min();
		size_t x{ 0 };
		size_t y{ 0 };
		for (size_t i = 0; i < size - subsize; ++i)
		{
			for (size_t j = 0; j < size - subsize; ++j)
			{
				int lp = 0;
				for (size_t si = i; si < i + subsize; ++si)
				{
					for (size_t sj = j; sj < j + subsize; ++sj)
					{
						lp += grid[si * size + sj];
					}
				}

				if (lp > power)
				{
					power = lp;
					x = j;
					y = i;
				}
			}
		}

		return { x + 1, y + 1, power };
	}

	static std::tuple<size_t, size_t, int, size_t> find_max_subgrid(std::vector<int> const & grid, size_t const size)
	{
		int power = std::numeric_limits<int>::min();
		size_t x{ 0 };
		size_t y{ 0 };
		size_t gs{ 0 };

		for (size_t s = 1; s <= 300; ++s)
		{
			auto[lx, ly, p] = find_max_subgrid(grid, size, s);

			if (p > power)
			{
				power = p;
				x = lx;
				y = ly;
				gs = s;
			}
		}

		return { x, y, power, gs };
	}

	void solve()
	{
		assert(compute_power_level(3, 5, 8) == 4);
		assert(compute_power_level(122, 79, 57) == -5);
		assert(compute_power_level(217, 196, 39) == 0);
		assert(compute_power_level(101, 153, 71) == 4);

		auto g1 = compute_power_grid(300, 18);
		auto[x1, y1, p1] = find_max_subgrid(g1, 300, 3);
		assert(x1 == 33 && y1 == 45 && p1 == 29);

		auto g2 = compute_power_grid(300, 42);
		auto[x2, y2, p2] = find_max_subgrid(g2, 300, 3);
		assert(x2 == 21 && y2 == 61 && p2 == 30);

		auto g3 = compute_power_grid(300, 7315);
		auto[x3, y3, p3] = find_max_subgrid(g3, 300, 3);

		std::cout << "part 1: " << '(' << x3 << ',' << y3 << ")=" << p3 << '\n';

		auto[x4, y4, p4, s4] = find_max_subgrid(g1, 300);
		assert(x4 == 90 && y4 == 269 && p4 == 113 && s4 == 16);

		auto[x5, y5, p5, s5] = find_max_subgrid(g2, 300);
		assert(x5 == 232 && y5 == 251 && p5 == 119 && s5 == 12);

		auto[x6, y6, p6, s6] = find_max_subgrid(g3, 300);

		std::cout << "part 2: " << '(' << x6 << ',' << y6 << ',' << s6 << ")=" << p6 << '\n';
	}
}