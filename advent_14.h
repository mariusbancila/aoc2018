#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

namespace aoc2018::puzzle_14
{
   static void add_score(std::vector<int> & scores, int const score1, int const score2)
   {
      auto rs = score1 + score2;
      if (rs > 9)
      {
         scores.push_back(rs / 10);
         scores.push_back(rs % 10);
      }
      else
         scores.push_back(rs);
   }

   static std::string find_score(int score1, int score2, int const count)
   {
      std::vector<int> scores {score1, score2};

      add_score(scores, score1, score2);

      auto s1i = 0;
      auto s2i = 1;

      while (scores.size() < count + 10)
      {
         s1i += score1 + 1;
         if (s1i >= scores.size()) s1i %= scores.size();

         s2i += score2 + 1;
         if (s2i >= scores.size()) s2i %= scores.size();

         score1 = scores[s1i];
         score2 = scores[s2i];

         add_score(scores, score1, score2);
      }

      std::string result;
      for (int i = count; i < count + 10; ++i)
         result.append(std::to_string(scores[i]));

      return result;
   }

   static size_t find_recipies(int score1, int score2, std::string input)
   {
      std::vector<int> result;
      for (auto const c : input) result.push_back(c - '0');

      std::vector<int> scores{ score1, score2 };

      add_score(scores, score1, score2);

      auto s1i = 0;
      auto s2i = 1;

      int i = 0;
      while (true)
      {
         s1i += score1 + 1;
         if (s1i >= scores.size()) s1i %= scores.size();

         s2i += score2 + 1;
         if (s2i >= scores.size()) s2i %= scores.size();

         score1 = scores[s1i];
         score2 = scores[s2i];

         add_score(scores, score1, score2);

         for (; i < scores.size() - result.size(); ++i)
         {
            if (std::equal(&scores[i], &scores[i] + result.size(), &result.front()))
               return i;
         }
      }
   }

	static void solve()
	{
      assert("5158916779" == find_score(3, 7, 9));
      assert("0124515891" == find_score(3, 7, 5));
      assert("9251071085" == find_score(3, 7, 18));
      assert("5941429882" == find_score(3, 7, 2018));

      std::cout << "part 1: " << find_score(3, 7, 47801) << '\n';
      
      assert(9 == find_recipies(3, 7, "51589"));
      assert(5 == find_recipies(3, 7, "01245"));
      assert(18 == find_recipies(3, 7, "92510"));
      assert(2018 == find_recipies(3, 7, "59414"));

      std::cout << "part 2: " << find_recipies(3, 7, "047801") << '\n';
	}
}