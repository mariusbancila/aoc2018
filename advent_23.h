#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>

namespace aoc2018::puzzle_23
{
   struct point
   {
      int x;
      int y;
      int z;
   };

   constexpr bool operator==(point const & p1, point const & p2) noexcept
   {
      return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
   }

   struct nanobot
   {
      point pos;
      int   range;
   };

   constexpr bool operator==(nanobot const & nb1, nanobot const & nb2) noexcept
   {
      return nb1.pos == nb2.pos && nb1.range == nb2.range;
   }

   inline int manhattan_distance(point const & p1, point const & p2) noexcept
   {
      return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y) + std::abs(p1.z - p2.z);
   }

   using nanobots = std::vector<nanobot>;

   static nanobots get_data(std::string_view path)
   {
      nanobots bots;

      std::ifstream file(path.data());
      if (file.is_open())
      {
         std::regex rx(R"(pos=<(\-*\d+),(\-*\d+),(\-*\d+)>, r=(\d+))");
         std::smatch match;
         std::string line;
         while (std::getline(file, line))
         {
            if (std::regex_match(line, match, rx))
            {
               point pt
               {
                  std::stoi(match[1].str()),
                  std::stoi(match[2].str()),
                  std::stoi(match[3].str())
               };

               nanobot nb { pt, std::stoi(match[4].str()) };

               bots.push_back(nb);
            }
         }
      }

      return bots;
   }

   static nanobot find_strongest_nanobot(nanobots const & bots)
   {
      return *std::max_element(std::cbegin(bots), std::cend(bots),
         [](nanobot const & nb1, nanobot const & nb2) {return nb1.range < nb2.range; });
   }

   static int find_nanobots_in_range(nanobots const & bots, nanobot const strongest)
   {
      return static_cast<int>(std::count_if(
         std::cbegin(bots), std::cend(bots),
         [strongest](nanobot const & bot) { 
         return manhattan_distance(bot.pos, strongest.pos) <= strongest.range;
      }));
   }

   static void solve()
   {
      {
         auto bots = get_data(R"(data\advent_23_test.txt)");

         auto sb = find_strongest_nanobot(bots);

         assert(sb.range == 4 && sb.pos.x == 0 && sb.pos.y == 0 && sb.pos.z == 0);

         assert(7 == find_nanobots_in_range(bots, sb));
      }

      {
         auto bots = get_data(R"(data\advent_23.txt)");

         auto sb = find_strongest_nanobot(bots);

         auto count = find_nanobots_in_range(bots, sb);

         std::cout << "part 1: count = " << count << '\n';
      }
   }
}