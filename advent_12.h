#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <assert.h>

namespace aoc2018::puzzle_12
{
   constexpr char PlantPot = '#';
   constexpr char EmptyPot = '.';

   struct rule
   {
      std::string pattern;
      char        result;
   };

   using rules_list = std::vector<rule>;

   static std::pair<std::string, rules_list> get_data(std::string_view path)
   {
      std::string initial_state;
      rules_list  rules;

      std::ifstream file(path.data());
      if (file.is_open())
      {
         std::string line;
         std::getline(file, line);

         assert(line.find("initial state: ") == 0);
         initial_state = line.substr(15, line.length() - 15);

         std::getline(file, line);
         assert(line.empty());

         std::regex rx(R"(([\.\#]{5}) => ([\.\#]))");
         std::smatch match;
         while (std::getline(file, line))
         {
            if (std::regex_match(line, match, rx))
            {
               rule r {match[1].str(), match[2].str().at(0)};
               rules.push_back(r);
            }
         }
      }

      return {initial_state, rules};
   }

   static long long pots_score(std::string_view pots, long long const offset)
   {
      auto sum = 0LL;
      for (int i = 0; i < static_cast<int>(pots.size()); ++i)
      {
         if (pots[i] == PlantPot)
            sum += (i - offset);
      }

      return sum;
   }

   static long long pots_score_1(std::string pots, rules_list const & rules, long long const generations)
   {
      long long total_left_added = 0;

      size_t maxLeftEmptyPots = 0;
      size_t maxRightEmptyPots = 0;

      for (auto const & r : rules)
      {
         size_t pos = r.pattern.find_first_of(PlantPot, 0);
         if (pos == std::string::npos)
            maxLeftEmptyPots = r.pattern.length();
         else if (pos > maxLeftEmptyPots)
            maxLeftEmptyPots = pos;

         pos = r.pattern.find_last_of(PlantPot);
         if (pos == std::string::npos)
            maxRightEmptyPots = r.pattern.length();
         else if (r.pattern.length() - pos - 1 > maxRightEmptyPots)
            maxRightEmptyPots = r.pattern.length() - pos - 1;
      }

      for (long long g = 1; g <= generations; ++g)
      {
         size_t const firstPlantPos = pots.find_first_of(PlantPot, 0);
         if (firstPlantPos < maxLeftEmptyPots)
         {
            pots = std::string(maxLeftEmptyPots - firstPlantPos, EmptyPot) + pots;
            total_left_added += static_cast<long long>(maxLeftEmptyPots - firstPlantPos);
         }

         size_t const lastPlantPos = pots.find_last_of(PlantPot);
         if (pots.length() - lastPlantPos - 1 < maxRightEmptyPots)
            pots += std::string(maxRightEmptyPots - (pots.length() - lastPlantPos - 1), EmptyPot);

         std::string next_generation = pots.substr(0, 2);

         for (size_t i = 2; i < pots.length() - 2; ++i)
         {
            bool found = false;
            for (auto const & r : rules)
            {
               if (r.pattern == pots.substr(i - 2, 5))
               {
                  next_generation.append(1, r.result);
                  found = true;
                  break;
               }
               
            }

            if(!found)
               next_generation.append(1, EmptyPot);
         }

         next_generation += pots.substr(pots.length() - 2, 2);

         pots = next_generation;                  
      }

      return pots_score(pots, total_left_added);
   }

   static bool is_stable(std::vector<long long> const & score_inc, size_t const count = 3)
   {
      return 
         score_inc.size() >= count &&
         std::all_of(
            std::crbegin(score_inc), std::crbegin(score_inc) + count,
            [v = score_inc.back()](auto const e){return e == v; });
   }

   static size_t find_stable_generation(std::vector<long long> const & score_inc)
   {
      auto pos = std::find_if(std::rbegin(score_inc), std::rend(score_inc), [v = score_inc.back()](auto const e){return e == v; });
      return score_inc.size() - std::distance(pos, std::rend(score_inc));
   }

   // a stable increase is reached after N generations
   // the total score after G generations is therefore:
   // total = (G - N) * inc + gen_n_score
   static long long pots_score_2(std::string pots, rules_list const & rules, long long const generations)
   {
      size_t maxLeftEmptyPots = 0;
      size_t maxRightEmptyPots = 0;
      long long total_left_added = 0;

      std::vector<long long> score_inc;
      std::vector<long long> scores;
      
      for (auto const & r : rules)
      {
         size_t pos = r.pattern.find_first_of(PlantPot, 0);
         if (pos == std::string::npos)
            maxLeftEmptyPots = r.pattern.length();
         else if (pos > maxLeftEmptyPots)
            maxLeftEmptyPots = pos;

         pos = r.pattern.find_last_of(PlantPot);
         if (pos == std::string::npos)
            maxRightEmptyPots = r.pattern.length();
         else if (r.pattern.length() - pos - 1 > maxRightEmptyPots)
            maxRightEmptyPots = r.pattern.length() - pos - 1;
      }      
      
      auto last_gen_score = pots_score(pots, 0);

      for (long long g = 1; g <= generations; ++g)
      {
         size_t const firstPlantPos = pots.find_first_of(PlantPot, 0);
         if (firstPlantPos < maxLeftEmptyPots)
         {
            pots = std::string(maxLeftEmptyPots - firstPlantPos, EmptyPot) + pots;
            total_left_added += static_cast<long long>(maxLeftEmptyPots - firstPlantPos);
         }

         size_t const lastPlantPos = pots.find_last_of(PlantPot);
         if (pots.length() - lastPlantPos - 1 < maxRightEmptyPots)
            pots += std::string(maxRightEmptyPots - (pots.length() - lastPlantPos - 1), EmptyPot);         

         std::string next_generation = pots.substr(0, 2);

         for (size_t i = 2; i < pots.length() - 2; ++i)
         {
            bool found = false;
            for (auto const & r : rules)
            {
               if (r.pattern == pots.substr(i - 2, 5))
               {
                  next_generation.append(1, r.result);
                  found = true;
                  break;
               }

            }

            if (!found)
               next_generation.append(1, EmptyPot);
         }

         next_generation += pots.substr(pots.length() - 2, 2);

         pots = next_generation;

         auto gen_score = pots_score(pots, total_left_added);
         scores.push_back(gen_score);
         score_inc.push_back(gen_score - last_gen_score);
         last_gen_score = gen_score;

         if (is_stable(score_inc, 10))
            break;
      }

      auto generation = find_stable_generation(score_inc);

      auto total = (generations - score_inc.size()) * score_inc.back() + last_gen_score;

      return total;
   }

   static void solve()
   {
      {
         auto [pots, rules] = get_data(R"(data\advent_12_test.txt)");

         assert(pots_score_1(pots, rules, 20) == 325);
      }

      {
         auto[pots, rules] = get_data(R"(data\advent_12.txt)");

         auto sum1 = pots_score_1(pots, rules, 20);
         
         std::cout << "part 1: sum = " << sum1 << '\n';

         auto sum2 = pots_score_2(pots, rules, 50000000000);

         std::cout << "part 2: sum = " << sum2 << '\n';
      }
   }
}