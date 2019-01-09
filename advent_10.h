#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <regex>

namespace aoc2018::puzzle_10
{
   struct point
   {
      int x;
      int y;
   };

   inline bool operator==(point const & p1, point const & p2) noexcept
   {
      return p1.x == p2.x && p1.y == p2.y;
   }

   inline bool operator<(point const & p1, point const & p2) noexcept
   {
      return p1.y < p2.y || p1.x < p2.x;
   }

   struct particle
   {
      point p;
      point v;
   };

   using particles = std::vector<particle>;

   static std::tuple<particles, int, int> get_data(std::string_view path)
   {
      particles data;

      int tmax{ 0 }, tmin{ std::numeric_limits<int>::max() };

      std::ifstream file(path.data());
      if (file.is_open())
      {
         std::regex rx(R"(position=<([\s-\d]+), ([\s-\d]+)> velocity=<([\s-\d]+), ([\s-\d]+)>)");
         std::smatch match;
         std::string line;
         while (std::getline(file, line))
         {
            if (std::regex_match(line, match, rx))
            {
               particle p
               { 
                  {std::stoi(match[1].str()), std::stoi(match[2].str())}, 
                  {std::stoi(match[3].str()), std::stoi(match[4].str())}
               };

               data.push_back(p);

               if (p.v.y != 0) 
               {
                  int ty = std::abs(p.p.y / p.v.y);
                  tmin = std::min(tmin, ty);
                  tmax = std::max(tmax, ty);
               }
               if (p.v.x != 0) 
               {
                  int tx = std::abs(p.p.x / p.v.x);
                  tmin = std::min(tmin, tx);
                  tmax = std::max(tmax, tx);
               }
            }
         }
      }

      return { data, tmin, tmax };
   }

   struct surface
   {
      long long left{ std::numeric_limits<int>::max() };
      long long right{ std::numeric_limits<int>::min() };
      long long top{ std::numeric_limits<int>::max() };
      long long bottom{ std::numeric_limits<int>::min() };

      long long area() const noexcept { return (right - left)*(bottom - top); }
   };

   static surface find_surface(particles const & data)
   {
      int left = std::numeric_limits<int>::max();
      int right = std::numeric_limits<int>::min();
      int top = std::numeric_limits<int>::max();
      int bottom = std::numeric_limits<int>::min();

      for (auto const & e : data)
      {
         if (e.p.x < left) { left = e.p.x; }
         if (e.p.x > right) { right = e.p.x; }
         if (e.p.y < top) { top = e.p.y; }
         if (e.p.y > bottom) { bottom = e.p.y; }
      }

      return {left, right, top, bottom};
   }
   
   static int find_time(particles data, int const tmin, int const tmax)
   {
      int time = tmin;

      std::for_each(std::begin(data), std::end(data), [time](particle & p) {
         p.p.x += p.v.x * time;
         p.p.y += p.v.y * time;
      });

      surface s = find_surface(data);

      for (int t = tmin+1; t < tmax; t++)
      {
         for (auto & e : data)
         {
            e.p.x += e.v.x;
            e.p.y += e.v.y;
         }

         auto ns = find_surface(data);

         if (ns.area() < s.area())
         {
            time = t;
            s = ns;
         }
      }

      return time;
   }

   static void show_frame(particles data, int const time)
   {
      for (size_t i = 0; i < data.size(); ++i)
      {
         data[i].p.x += time * data[i].v.x;
         data[i].p.y += time * data[i].v.y;
      }

      surface s = find_surface(data);

      int const width = static_cast<int>(s.right - s.left + 1);
      int const height = static_cast<int>(s.bottom - s.top + 1);

      std::vector<char> frame(width * height, ' ');
      for (auto const & e : data)
      {
         auto nx = e.p.x - s.left;
         auto ny = e.p.y - s.top;

         frame[ny * width + nx] = '#';
      }

      for (int i = 0; i < height; ++i)
      {
         for (int j = 0; j < width; ++j)
         {
            std::cout << frame[i * width + j];
         }

         std::cout << '\n';
      }
   }

   static void solve()
   {
      {
         auto [data, tmin, tmax] = get_data(R"(data\advent_10_test.txt)");

         auto time = find_time(data, tmin, tmax);

         show_frame(data, time);
      }
      
      {
         auto[data, tmin, tmax] = get_data(R"(data\advent_10.txt)");

         auto time = find_time(data, tmin, tmax);

         show_frame(data, time);

         std::cout << "time = " << time << '\n';
      }      
   }
}
