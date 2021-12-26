#pragma once

#include "enum/logtype.hpp"

#include "thirdparty/json.hpp"
#include "thirdparty/filesystem.hpp"

#include <string>

namespace lib
{
	/**
	 * Formatting utilities
	 */
	class fmt
	{
	public:
		/**
		 * Format a string as a string
		 * @param fmt string
		 * @return string
		 */
		template<typename Format>
		static auto format(const Format &fmt) -> std::string
		{
			return fmt;
		}

		/**
		 * Format a string with specified arguments as {} in the string
		 * @param fmt string
		 * @param arg Argument to to_string
		 * @param args Arguments to to_string
		 * @return Formatted string
		 */
		template<typename Format, typename Arg, typename... Args>
		static auto format(const Format &fmt, const Arg &arg, Args &&... args) -> std::string
		{
			return format(collect(fmt, arg), args...);
		}

		/**
		 * Format time as M:SS
		 * @param ms Milliseconds
		 */
		static auto time(int ms) -> std::string;

		/**
		 * Format size as B, kB, MB or GB (bytes)
		 * @param bytes Bytes
		 */
		static auto size(unsigned int bytes) -> std::string;

		/**
		 * Format as k or M
		 * @param count Amount
		 */
		static auto count(unsigned int count) -> std::string;

	private:
		/**
		 * Format string into another string
		 * @param fmt String with {} to replace
		 * @param arg String to replace {} with
		 * @return Formatted string
		 */
		template<typename Format>
		static auto collect(const Format &fmt, const std::string &arg) -> std::string
		{
			auto str = std::string(fmt);
			auto index = str.find("{}");
			if (index == std::string::npos)
				return str;

			return str.replace(index, 2, arg);
		}

		/**
		 * Format value into a string
		 * @param fmt String with {} to replace
		 * @param arg Value to replace {} with
		 * @return Formatted string
		 */
		template<typename Format, typename Arg>
		static auto collect(const Format &fmt, const Arg &arg) -> std::string
		{
			return collect(fmt, std::to_string(arg));
		}

		/**
		 * Format C-style string into a string
		 * @param fmt String with {} to replace
		 * @param arg C-style string to replace {} with
		 * @return Formatted string
		 */
		template<typename Format>
		static auto collect(const Format &fmt, const char *arg) -> std::string
		{
			return collect(fmt, std::string(arg));
		}

		/**
		 * Format bool as "true" or "false"
		 * @param fmt String with {} to replace
		 * @param arg Value to replace {} with
		 * @return Formatted string
		 */
		template<typename Format>
		static auto collect(const Format &fmt, bool arg) -> std::string
		{
			return collect(fmt, arg ? "true" : "false");
		}

		/**
		 * Dump JSON as a string
		 * @param fmt String with {} to replace
		 * @param arg JSON to dump
		 * @return Formatted string
		 */
		template<typename Format>
		static auto collect(const Format &fmt, const nlohmann::json &arg) -> std::string
		{
			return collect(fmt, arg.dump());
		}
	};
}
