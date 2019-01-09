#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <algorithm>

namespace aoc2018::puzzle_05
{
	static std::string get_data(std::string_view path)
	{
		std::string text;
		std::ifstream file(path.data(), std::ios::ate | std::ios::binary);
		if (file.is_open())
		{
			auto length = file.tellg();
			text.resize(static_cast<size_t>(length));
			file.seekg(std::ios::beg);
			file.read(&text.front(), length);
		}

		return text;
	}

	static size_t polymer_size(std::string text)
	{
		auto reduced = false;
		do
		{
			reduced = false;
			for (size_t i = 0; i < text.size() - 2;)
			{
				if (tolower(text[i]) == tolower(text[i + 1]) && text[i] != text[i + 1])
				{
					text[i] = '*';
					text[i + 1] = '*';
					i += 2;
					reduced = true;
				}
				else i++;
			}

			text.erase(
				std::remove_if(
					text.begin(), text.end(),
					[](char const & c) {return c == '*'; }),
				text.end());
		} while (reduced);

		return text.size();
	}

	static size_t part_1(std::string_view text)
	{
		return polymer_size(text.data());
	}

	static size_t part_2(std::string_view text)
	{
		auto length = text.size();
		for (char unit = 'a'; unit <= 'z'; ++unit)
		{
			std::string copy = text.data();

			copy.erase(
				std::remove_if(
					copy.begin(), copy.end(),
					[unit](char const & c) {return tolower(c) == unit; }),
				copy.end());

			length = std::min(length, polymer_size(copy));
		}

		return length;
	}

	static void solve()
	{
		auto text = get_data("data\\advent_05.txt");

		std::cout << "part 1: length =" << part_1(text) << '\n';
	
		std::cout << "part 2: length =" << part_2(text) << '\n';
	}
}