#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <regex>
#include <map>
#include <array>
#include <assert.h>

namespace aoc2018::puzzle_04
{
	enum class shift_event
	{
		starts_shift,
		falls_asleep,
		wakes_up
	};

	struct shift_record
	{
		tm			time;
		int			guard_id;
		shift_event event;
	};

	inline bool operator==(shift_record const & r1, shift_record const & r2) noexcept
	{
		return
			r1.time.tm_year == r2.time.tm_year &&
			r1.time.tm_mon == r2.time.tm_mon &&
			r1.time.tm_mday == r2.time.tm_mday &&
			r1.time.tm_hour == r2.time.tm_hour &&
			r1.time.tm_min == r2.time.tm_min &&
			r1.guard_id == r1.guard_id &&
			r1.event == r2.event;
	}

	inline bool operator !=(shift_record const & r1, shift_record const & r2) noexcept
	{
		return !(r1 == r2);
	}

	struct shift_chrono_order
	{
		bool operator()(shift_record const & r1, shift_record const & r2) noexcept
		{
			return
				(r1.time.tm_year < r2.time.tm_year) ||
				(r1.time.tm_year == r2.time.tm_year && r1.time.tm_mon < r2.time.tm_mon) ||
				(r1.time.tm_year == r2.time.tm_year && r1.time.tm_mon == r2.time.tm_mon && r1.time.tm_mday < r2.time.tm_mday) ||
				(r1.time.tm_year == r2.time.tm_year && r1.time.tm_mon == r2.time.tm_mon && r1.time.tm_mday == r2.time.tm_mday && r1.time.tm_hour < r2.time.tm_hour) ||
				(r1.time.tm_year == r2.time.tm_year && r1.time.tm_mon == r2.time.tm_mon && r1.time.tm_mday == r2.time.tm_mday && r1.time.tm_hour == r2.time.tm_hour && r1.time.tm_min < r2.time.tm_min);
		}
	};

	using shift_records = std::vector<shift_record>;

	static shift_records get_data(std::string_view path)
	{
		shift_records records;

		std::ifstream file(path.data());
		if (file.is_open())
		{
			std::regex rx(R"(\[(\d{4})\-(\d{2})\-(\d{2}) (\d{2}):(\d{2})\] (.+))");
			std::smatch match;
			std::string line;
			while (std::getline(file, line))
			{
				if (std::regex_match(line, match, rx))
				{
					tm time{0};
					time.tm_year = std::stoi(match[1].str());
					time.tm_mon = std::stoi(match[2].str());
					time.tm_mday = std::stoi(match[3].str());
					time.tm_hour = std::stoi(match[4].str());
					time.tm_min = std::stoi(match[5].str());

					auto action = match[6].str();
					if (action == "falls asleep")
					{
						records.push_back(shift_record{ time, 0, shift_event::falls_asleep });
					}
					else if (action == "wakes up")
					{
						records.push_back(shift_record{ time, 0, shift_event::wakes_up});
					}
					else
					{
						std::regex rx2(R"(Guard #(\d+) begins shift)");
						std::smatch match2;
						if (std::regex_match(action, match2, rx2))
						{
							records.push_back(shift_record{ time, std::stoi(match2[1].str()), shift_event::starts_shift });
						}
					}
				}
			}
		}
		
		return records;
	}

	static std::map<int, std::array<int, 60>> build_shift_data(shift_records const & records)
	{
		std::map<int, std::array<int, 60>> data;

		int last_guard_id{ 0 };
		int start_sleep{ 0 };
		for (auto const & record : records)
		{
			switch (record.event)
			{
			case shift_event::starts_shift:
				last_guard_id = record.guard_id;
				break;
			case shift_event::falls_asleep:
				assert(record.time.tm_hour == 0);
				start_sleep = record.time.tm_min;
				break;
			case shift_event::wakes_up:
				{
					assert(record.time.tm_hour == 0);
					auto pos = data.find(last_guard_id);
					if (pos == std::end(data))
					{
						std::array<int, 60> mins{ 0 };
						for (int i = start_sleep; i < record.time.tm_min; ++i) mins[i] = 1;
						data.insert_or_assign(last_guard_id, mins);
					}
					else
					{
						std::array<int, 60>& mins = pos->second;
						for (int i = start_sleep; i < record.time.tm_min; ++i) mins[i]++;
					}
				}
				break;
			}
		}

		return data;
	}
	
	static std::pair<int, int> find_optimum_window_2(std::map<int, std::array<int, 60>> const & data)
	{
		int guard_id{ 0 };
		int minute{ 0 };
		int count{ 0 };
		for (auto const & kvp : data)
		{
			for (int i = 0; i < 60; ++i)
			{
				if (kvp.second[i] > count)
				{
					minute = i;
					guard_id = kvp.first;
					count = kvp.second[i];
				}
			}
		}

		return { guard_id, minute };
	}	

	static std::pair<int, int> find_optimum_window_1(std::map<int, std::array<int, 60>> const & data)
	{		
		int guard_id{ 0 };
		int count{ 0 };

		for (auto const & kvp : data)
		{
			auto total = std::accumulate(std::cbegin(kvp.second), std::cend(kvp.second), 0);
			if (total > count)
			{
				guard_id = kvp.first;
				count = total;
			}
		}

		auto it = data.find(guard_id);		
		int minute = 0;
		count = 0;
		for (int i = 0; i < 60; ++i)
		{
			if (it->second[i] > count)
			{
				minute = i;
				count = it->second[i];
			}
		}

		return { guard_id, minute };
	}

	static void solve()
	{
		{
			auto test_data = get_data(R"(data\advent_04_test.txt)");
			std::sort(std::begin(test_data), std::end(test_data), shift_chrono_order());
			{
				auto[id, minute] = find_optimum_window_1(build_shift_data(test_data));
				assert(id*minute == 240);
			}
			{
				auto[id, minute] = find_optimum_window_2(build_shift_data(test_data));
				assert(id*minute == 4455);
			}
		}

		{
			auto data = get_data(R"(data\advent_04.txt)");
			std::sort(std::begin(data), std::end(data), shift_chrono_order());
			{
				auto[id, minute] = find_optimum_window_1(build_shift_data(data));
				std::cout << "part 1:" << id << " * " << minute << " = " << id * minute << '\n';
			}
			{
				auto[id, minute] = find_optimum_window_2(build_shift_data(data));
				std::cout << "part 2:" << id << " * " << minute << " = " << id * minute << '\n';
			}
		}
	}
}