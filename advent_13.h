#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <algorithm>
#include <assert.h>

namespace aoc2018::puzzle_13
{
   enum class cart_direction : char
   {
      none = '\0', up = '^', down = 'v', left = '<', right = '>'
   };

   enum class turn_direction
   {
      left,
      straight,
      right
   };

   struct position
   {
      int x;
      int y;
   };

   constexpr bool operator==(position const & p1, position const & p2) noexcept
   {
      return p1.x == p2.x && p1.y == p2.y;
   }

   constexpr bool operator<(position const & p1, position const & p2) noexcept
   {
      return (p1.y < p2.y) || (p1.y == p2.y && p1.x < p2.x);
   }

   struct cart
   {
      position       location;
      cart_direction direction;
      turn_direction next_turn;
   };

   constexpr bool cart_sort(cart const & c1, cart const & c2) noexcept
   {
      return
         (c1.location < c2.location) ||
         (c1.location == c2.location && c1.direction < c2.direction) ||
         (c1.location == c2.location && c1.direction == c2.direction && c1.next_turn < c2.next_turn);
   }

   constexpr bool operator==(cart const & c1, cart const & c2) noexcept
   {
      return c1.location == c2.location && c1.direction == c2.direction && c1.next_turn == c2.next_turn;
   }

   inline void update_turn_direction(turn_direction & dir) noexcept
   {
      if (dir == turn_direction::left)
         dir = turn_direction::straight;
      else if (dir == turn_direction::straight)
         dir = turn_direction::right;
      else
         dir = turn_direction::left;
   }

   using tracking = std::vector<std::string>;
   using carts_list = std::list<cart>;

   static std::pair<tracking, carts_list> get_data(std::string_view path)
   {
      tracking tracks;
      carts_list carts;

      std::ifstream file(path.data());
      if (file.is_open())
      {
         std::string line;

         while (std::getline(file, line))
         {
            for (int j = 0; j < line.length(); ++j)
            {
               // assumes carts are always on straight paths (- |) and never on curves (/ \)

               if (line[j] == static_cast<char>(cart_direction::left) ||
                  line[j] == static_cast<char>(cart_direction::right))
               {
                  carts.push_back(
                     { {j, static_cast<int>(tracks.size())},
                        static_cast<cart_direction>(line[j]),
                        turn_direction::left });
                  line[j] = '-';
               }
               else if (line[j] == static_cast<char>(cart_direction::up) ||
                  line[j] == static_cast<char>(cart_direction::down))
               {
                  carts.push_back(
                     { {j, static_cast<int>(tracks.size())},
                        static_cast<cart_direction>(line[j]),
                        turn_direction::left });
                  line[j] = '|';
               }
            }
            tracks.push_back(line);
         }
      }

      return { tracks, carts };
   }

   static auto find_cart_at(carts_list const & carts, position const & pos)
   {
      return std::find_if(std::cbegin(carts), std::cend(carts),
         [pos](cart const & c) {return c.location == pos; });
   }

   static bool has_cart(carts_list const & carts, position const & pos)
   {
      return find_cart_at(carts, pos) != std::cend(carts);
   }

   static position find_first_crash(tracking const & tracks, carts_list carts)
   {
      size_t const height = tracks.size();
      size_t const width = tracks[0].size();

      while (true)
      {
         carts_list next_carts;
         carts.sort(cart_sort);

         while (!carts.empty())
         {
            auto c = carts.front();
            carts.pop_front();

            switch (c.direction)
            {
            case cart_direction::right: c.location.x++; break;
            case cart_direction::left: c.location.x--; break;
            case cart_direction::up: c.location.y--; break;
            case cart_direction::down: c.location.y++; break;
            }

            if (has_cart(carts, c.location) || has_cart(next_carts, c.location))
            {
               return c.location;
            }

            switch (c.direction)
            {
            case cart_direction::right:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::up;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::down;
               }
               break;
               case '-':
               {
                  c.direction = cart_direction::right;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::up; break;
                  case turn_direction::straight: c.direction = cart_direction::right; break;
                  case turn_direction::right: c.direction = cart_direction::down; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;

            case cart_direction::left:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::down;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::up;
               }
               break;
               case '-':
               {
                  c.direction = cart_direction::left;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::down; break;
                  case turn_direction::straight: c.direction = cart_direction::left; break;
                  case turn_direction::right: c.direction = cart_direction::right; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;

            case cart_direction::up:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::right;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::left;
               }
               break;
               case '|':
               {
                  c.direction = cart_direction::up;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::left; break;
                  case turn_direction::straight: c.direction = cart_direction::up; break;
                  case turn_direction::right: c.direction = cart_direction::right; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;

            case cart_direction::down:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::left;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::right;
               }
               break;
               case '|':
               {
                  c.direction = cart_direction::down;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::right; break;
                  case turn_direction::straight: c.direction = cart_direction::down; break;
                  case turn_direction::right: c.direction = cart_direction::left; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;
            }

            next_carts.push_back(c);
         }

         carts.swap(next_carts);
      }
   }

   static position find_last_cart(tracking const & tracks, carts_list carts)
   {
      size_t const height = tracks.size();
      size_t const width = tracks[0].size();

      while (carts.size() > 1)
      {
         carts_list next_carts;
         carts.sort(cart_sort);

         while (!carts.empty())
         {
            auto c = carts.front();
            carts.pop_front();

            switch (c.direction)
            {
            case cart_direction::right: c.location.x++; break;
            case cart_direction::left: c.location.x--; break;
            case cart_direction::up: c.location.y--; break;
            case cart_direction::down: c.location.y++; break;
            }
       
            auto it1 = find_cart_at(carts, c.location);
            if (it1 != std::cend(carts))
            {
               carts.erase(it1);
               continue;
            }
            else
            {
               auto it2 = find_cart_at(next_carts, c.location);
               if (it2 != std::cend(next_carts))
               {
                  next_carts.erase(it2);
                  continue;
               }
            }

            switch (c.direction)
            {
            case cart_direction::right:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::up;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::down;
               }
               break;
               case '-':
               {
                  c.direction = cart_direction::right;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::up; break;
                  case turn_direction::straight: c.direction = cart_direction::right; break;
                  case turn_direction::right: c.direction = cart_direction::down; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;

            case cart_direction::left:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::down;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::up;
               }
               break;
               case '-':
               {
                  c.direction = cart_direction::left;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::down; break;
                  case turn_direction::straight: c.direction = cart_direction::left; break;
                  case turn_direction::right: c.direction = cart_direction::right; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;

            case cart_direction::up:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::right;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::left;
               }
               break;
               case '|':
               {
                  c.direction = cart_direction::up;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::left; break;
                  case turn_direction::straight: c.direction = cart_direction::up; break;
                  case turn_direction::right: c.direction = cart_direction::right; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;

            case cart_direction::down:
            {
               switch (tracks[c.location.y][c.location.x])
               {
               case '/':
               {
                  c.direction = cart_direction::left;
               }
               break;
               case '\\':
               {
                  c.direction = cart_direction::right;
               }
               break;
               case '|':
               {
                  c.direction = cart_direction::down;
               }
               break;
               case '+':
               {
                  switch (c.next_turn)
                  {
                  case turn_direction::left: c.direction = cart_direction::right; break;
                  case turn_direction::straight: c.direction = cart_direction::down; break;
                  case turn_direction::right: c.direction = cart_direction::left; break;
                  }
                  update_turn_direction(c.next_turn);
               }
               break;
               }
            }
            break;
            }

            next_carts.push_back(c);
         }

         carts.swap(next_carts);
      }

      return carts.front().location;
   }

   static void solve()
   {
      {
         auto[tracks, carts] = get_data(R"(data\advent_13_test.txt)");

         auto pos = find_first_crash(tracks, carts);

         assert(pos.x == 7 && pos.y == 3);
      }

      {
         auto[tracks, carts] = get_data(R"(data\advent_13_2_test.txt)");

         auto pos = find_last_cart(tracks, carts);

         assert(pos.x == 6 && pos.y == 4);
      }

      {
         auto[tracks, carts] = get_data(R"(data\advent_13.txt)");

         auto pos1 = find_first_crash(tracks, carts);

         std::cout << "part 1: pos = " << pos1.x << "," << pos1.y << '\n';

         auto pos2 = find_last_cart(tracks, carts);

         std::cout << "part 2: pos = " << pos2.x << "," << pos2.y << '\n';
      }
   }
}