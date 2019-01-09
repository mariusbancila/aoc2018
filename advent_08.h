#include <iostream>
#include <fstream>
#include <vector>
#include <string_view>
#include <numeric>
#include <assert.h>

namespace aoc2018::puzzle_08
{
   static int sum_metadata(std::ifstream& file)
   {
      int sum = 0;

      int childrenCount, metadataCount;
      file >> childrenCount >> metadataCount;

      for (int i = 0; i < childrenCount; ++i)
      {
         sum += sum_metadata(file);
      }

      for (int i = 0; i < metadataCount; ++i)
      {
         int metadata;
         file >> metadata;
         sum += metadata;
      }

      return sum;
   }

   static int sum_metadata(std::string_view path)
   {
      std::ifstream file(path.data());
      if (file.is_open())
      {
         return sum_metadata(file);
      }

      return 0;
   }

   static int find_node_value(std::ifstream& file)
   {
      int value = 0;

      int childrenCount, metadataCount;
      file >> childrenCount >> metadataCount;

      std::vector<int> child_values(childrenCount, 0);
      std::vector<int> metadata(metadataCount, 0);
      for (int i = 0; i < childrenCount; ++i)
      {
         child_values[i] = find_node_value(file);
      }

      for (int i = 0; i < metadataCount; ++i)
      {
         file >> metadata[i];
      }

      if (childrenCount == 0)
      {
         value = std::accumulate(std::begin(metadata), std::end(metadata), 0);
      }
      else
      {
         for (int i = 0; i < metadataCount; ++i)
         {
            if (metadata[i] > 0 && metadata[i] <= childrenCount)
               value += child_values[metadata[i] - 1];
         }
      }

      return value;
   }

   static int find_root_value(std::string_view path)
   {
      std::ifstream file(path.data());
      if (file.is_open())
      {
         return find_node_value(file);
      }

      return 0;
   }

   static void solve()
   {
      {
         auto sum_test = sum_metadata(R"(data\advent_08_test.txt)");
         assert(sum_test == 138);

         auto root_value = find_root_value(R"(data\advent_08_test.txt)");
         assert(root_value == 66);
      }

      {
         auto sum_test = sum_metadata(R"(data\advent_08.txt)");

         std::cout << "part 1: sum = " << sum_test << '\n';

         auto root_value = find_root_value(R"(data\advent_08.txt)");

         std::cout << "part 1: value = " << root_value << '\n';
      }
   }
}