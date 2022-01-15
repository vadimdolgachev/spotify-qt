#include "lib/log.hpp"

std::vector<lib::log_message> lib::log::messages = std::vector<log_message>();

bool lib::log::log_to_stdout = true;

void lib::log::message(log_type log_type, const std::string &message)
{
	log_message msg(log_type, message);
	messages.push_back(msg);

	if (!log_to_stdout)
	{
		return;
	}

	if (log_type == log_type::information || log_type == log_type::verbose)
	{
		std::cout << msg.to_string() << std::endl;
	}
	else
	{
		std::cerr << msg.to_string() << std::endl;
	}
}

auto lib::log::get_messages() -> const std::vector<log_message> &
{
	return messages;
}

void lib::log::clear()
{
	messages.clear();
}

void lib::log::set_log_to_stdout(bool value)
{
	log_to_stdout = value;
}
