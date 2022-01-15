#include "lib/spotify/api.hpp"

void lib::spt::api::search(const std::string &query,
	lib::callback<lib::spt::search_results> &callback)
{
	get(lib::fmt::format("search"
						 "?q={}"
						 "&type=album,artist,playlist,track,show"
						 "&limit=50"
						 "&market=from_token",
		query), callback);
}
