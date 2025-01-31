#include "lib/search/api.hpp"

lib::ddg::api::api(const lib::http_client &http_client)
	: http(http_client)
{
}

void lib::ddg::api::search(const lib::spt::artist &artist,
	lib::callback<lib::ddg::results> &callback)
{
	const auto url = lib::fmt::format("https://api.duckduckgo.com"
									  "?q={}"
									  "&format=json"
									  "&t=spotify-qt", artist.name);

	http.get(url, lib::headers(), [callback](const std::string &result)
	{
		callback(nlohmann::json::parse(result));
	});
}
