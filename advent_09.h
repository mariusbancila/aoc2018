#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>

namespace aoc2018::puzzle_09
{
   struct game_record
   {
      int players;
      int last_marble;
      long long score;
   };

   // ----------- solution 1 ----------------

   using game_records = std::vector<game_record>;

   static game_records get_data(std::string_view path)
   {
      game_records records;

      std::ifstream file(path.data());
      if (file.is_open())
      {
         std::regex rx(R"((\d+) players; last marble is worth (\d+) points(: high score is (\d+))*)");
         std::smatch match;
         std::string line;
         while (std::getline(file, line))
         {
            if (std::regex_match(line, match, rx))
            {
               game_record record;
               record.players = std::stoi(match[1].str());
               record.last_marble = std::stoi(match[2].str());
               if (match.size() > 4 && match[4].matched)
                  record.score = std::stoll(match[4].str());

               records.push_back(record);
            }
         }
      }

      return records;
   }

   static long long play_marbles(int const players, int const lastmarble)
   {
      std::vector<long long> player_scores(players, 0);
      std::vector<int> marbles;

      int current_marble = 0;

      marbles.push_back(current_marble++);
      marbles.push_back(current_marble++);

      size_t current_index = 1;
      int current_player = 2;

      while (current_marble < lastmarble)
      {
         if ((current_marble % 23) != 0)
         {
            if (current_index + 2 > marbles.size())
            {
               current_index = 1; 
            }
            else 
            {
               current_index += 2;
            }
            marbles.insert(std::begin(marbles) + current_index, current_marble);
         }
         else 
         {
            size_t erase_index = current_index > 7 ? current_index - 7 : marbles.size() + current_index - 7;

            player_scores[current_player - 1] += current_marble;
            player_scores[current_player - 1] += marbles[erase_index];
            marbles.erase(std::begin(marbles) + erase_index);
            current_index = erase_index;
         }

         current_player = (current_player + 1) % (players + 1);
         if (current_player == 0) current_player = 1;

         current_marble++;
      }

      return *std::max_element(std::cbegin(player_scores), std::cend(player_scores));
   }

   // ----------- solution 2 ----------------

   struct circular_list_node
   {
      int                 value;
      circular_list_node* prev;
      circular_list_node* next;
   };

   struct circular_list
   {
      void insert(int const value, int offset)
      {
         if (count == 0)
         {
            current = new circular_list_node();
            current->value = value;
            current->next = current;
            current->prev = current;
         }
         else if (count == 1)
         {
            auto node = new circular_list_node();
            node->value = value;
            node->next = current;
            node->prev = current;

            current->next = node;
            current->prev = node;

            current = node;
         }
         else
         {
            circular_list_node* node = current;

            if (offset >= 0)
            {
               while (offset-- > 0) node = node->next;
            }
            else
            {
               while (offset++ < 0) node = node->prev;
            }

            auto newnode = new circular_list_node();
            newnode->value = value;
            newnode->next = node->next;
            newnode->prev = node;

            node->next->prev = newnode;
            node->next = newnode;

            current = newnode;
         }

         count++;
      }

      int erase(int offset)
      {
         int value = 0;

         if (count == 1)
         {
            value = current->value;
            delete current;
            current = nullptr;
            count--;
         }
         else if(count > 1)
         {
            circular_list_node* node = current;

            if (offset > 0)
            {
               while (offset-- > 0) node = node->next;
            }
            else
            {
               while (offset++ < 0) node = node->prev;
            }

            node->prev->next = node->next;
            node->next->prev = node->prev;

            value = node->value;
            current = node->next;

            delete node;
            count--;
         }

         return value;
      }

      int current_value() const noexcept { return current != nullptr ? current->value : 0; }
      size_t size() const noexcept { return count; }
      bool empty() const noexcept { return count == 0; }

      ~circular_list()
      {
         auto node = current;
         while (node != current)
         {
            auto temp = node;
            node = temp->next;
            delete temp;
         }

         delete current;
      }

   private:
      circular_list_node* current{ nullptr };
      size_t count{ 0 };
   };

   static long long find_max_score(int const players, int const lastmarble)
   {
      std::vector<long long> player_scores(players, 0);

      int current_marble = 0;

      circular_list marbles;
      marbles.insert(current_marble++, 0);
      marbles.insert(current_marble++, 1);

      int current_player = 2;

      while (current_marble <= lastmarble)
      {
         if ((current_marble % 23) != 0)
         {
            marbles.insert(current_marble, 1);
         }
         else
         {
            player_scores[current_player - 1] += current_marble;
            player_scores[current_player - 1] += marbles.erase(-7);
         }

         current_player = (current_player + 1) % (players + 1);
         if (current_player == 0) current_player = 1;

         current_marble++;
      }

      return *std::max_element(std::cbegin(player_scores), std::cend(player_scores));
   }

   static void solve()
   {
      // ----------- solution 1 ----------------
      /*
      {
         auto data = get_data(R"(data\advent_09_test.txt)");
         for (auto const & r : data)
         {
            auto score = play_marbles(r.players, r.last_marble);
            assert(score == r.score);
         }
      }
      
      {
         auto data = get_data(R"(data\advent_09.txt)");

         auto score = play_marbles(data[0].players, data[0].last_marble);

         std::cout << "part 1: score = " << score << '\n';
      }      
      */
      
      // ----------- solution 2 ----------------
      {
         auto data = get_data(R"(data\advent_09_test.txt)");
         for (auto const & r : data)
         {
            auto score = find_max_score(r.players, r.last_marble);
            assert(score == r.score);
         }
      }
      
      {
         auto data = get_data(R"(data\advent_09.txt)");

         auto score = play_marbles(data[0].players, data[0].last_marble * 100);

         std::cout << "part 2: score = " << score << '\n';
      }
   }
}
