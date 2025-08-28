#pragma once

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace Utils
{
	inline auto join_str_vec(const std::vector< std::string >& vec)
	{
		auto ret = std::string();
		std::ranges::for_each(vec, [&](const auto& line) { ret += line; });

		return ret;
	}

	constexpr auto is_number(const std::string& input)
	{
		return std::ranges::find_if(input, [](const auto& ch)
			{
				return !std::isdigit(ch);
			}
		) == input.end();
	}

	inline void remove_non_printable_chars_from_string(std::string& str)
	{
		str.erase(
			std::remove_if(
				str.begin(),
				str.end(),
				[](unsigned char c) { return !std::isprint(c); }
			),
			str.end()
		);
	}

	constexpr auto write_as_file = [](const std::string& file_name, const std::vector< std::string >& content)
	{
		std::ofstream out_file(file_name);

		if (out_file.bad())
			return false;

		std::ostream_iterator< std::string > it(out_file, "\n");

		std::ranges::copy(content, it);

		out_file.close();

		return true;
	};

	[[nodiscard]] inline auto str_to_ul(const std::string& input)
	{
		return input.contains("0x") ? std::stoull(input, nullptr, 16) : std::stoull(input, nullptr, 10);
	}

	[[nodiscard]] inline static auto is_immediate(const std::string& input)
	{
		static std::regex imm_regex("(0[xX]([a-fA-F0-9]){1,16})|(\\d+)|\\d+\\.\\d+");

		return std::regex_match(input, imm_regex);
	}

	inline const auto convert_array_str_to_bytes(const std::string& pattern)
	{
		std::vector< uint8_t > m_vecPattern;

		std::string Temp = std::string();

		std::string strPattern = pattern;
		std::erase_if(strPattern, isspace);

		// Convert string pattern to byte pattern
		for (const auto& pattern_byte : strPattern)
		{
			if (pattern_byte == '?')
			{
				m_vecPattern.emplace_back(0xCC);
				continue;
			}

			if (Temp.length() != 2)
				Temp += pattern_byte;

			if (Temp.length() == 2)
			{
				std::erase_if(Temp, isspace);
				auto converted_pattern_byte = strtol(Temp.c_str(), nullptr, 16) & 0xFFF;
				m_vecPattern.emplace_back(converted_pattern_byte);
				Temp.clear();
			}
		}

		return m_vecPattern;
	}

	inline const auto convert_bytes_vector_to_str(const std::vector< uint8_t >& bytes)
	{
		auto ret = std::string();

		for (const auto& bt : bytes)
			ret += std::format("{:02X}", bt);

		return ret;
	}

	inline const void sleep(const uint32_t miliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(420));
	}
}