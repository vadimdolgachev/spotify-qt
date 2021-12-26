#include "lib/settings/spotify.hpp"

void lib::setting::to_json(nlohmann::json &j, const spotify &s)
{
	j = nlohmann::json{
		{"always_start", s.always_start},
		{"backend", s.backend},
		{"bitrate", s.bitrate},
		{"disable_discovery", s.disable_discovery},
		{"global_config", s.global_config},
		{"keyring_password", s.keyring_password},
		{"max_queue", s.max_queue},
		{"path", s.path},
		{"start_client", s.start_client},
		{"username", s.username},
	};
}

void lib::setting::from_json(const nlohmann::json &j, spotify &s)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "always_start", s.always_start);
	lib::json::get(j, "backend", s.backend);
	lib::json::get(j, "bitrate", s.bitrate);
	lib::json::get(j, "disable_discovery", s.disable_discovery);
	lib::json::get(j, "global_config", s.global_config);
	lib::json::get(j, "keyring_password", s.keyring_password);
	lib::json::get(j, "max_queue", s.max_queue);
	lib::json::get(j, "path", s.path);
	lib::json::get(j, "start_client", s.start_client);
	lib::json::get(j, "username", s.username);
}
