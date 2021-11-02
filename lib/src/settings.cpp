#include "lib/settings.hpp"

using namespace lib;

settings::settings(const paths &paths)
	: path(paths)
{
	load();
}

auto settings::file_name() const -> std::string
{
	return path.config_file();
}

auto settings::file_path() const -> std::string
{
	return ghc::filesystem::path(file_name()).parent_path().string();
}

//region qt

void lib::setting::to_json(nlohmann::json &j, const qt &q)
{
	j = nlohmann::json{
		{"system_title_bar", q.system_title_bar},
		{"custom_font", q.custom_font},
	};
}

void lib::setting::from_json(const nlohmann::json &j, qt &q)
{
	if (!j.is_object())
	{
		return;
	}

	lib::json::get(j, "system_title_bar", q.system_title_bar);
	lib::json::get(j, "custom_font", q.custom_font);
}

auto lib::settings::qt() -> setting::qt &
{
	return qt_settings;
}

auto lib::settings::qt_const() const -> const setting::qt &
{
	return qt_settings;
}

//endregion

void settings::from_json(const nlohmann::json &json)
{
	auto a = json.at("Account");
	auto g = json.at("General");
	auto s = json.at("Spotify");

	// Account
	setValue(a, "access_token", account.access_token);
	setValue(a, "client_id", account.client_id);
	setValue(a, "client_secret", account.client_secret);
	setValue(a, "last_refresh", account.last_refresh);
	setValue(a, "refresh_token", account.refresh_token);

	// General
	setValue(g, "custom_playlist_order", general.custom_playlist_order);
	setValue(g, "fallback_icons", general.fallback_icons);
	setValue(g, "fixed_width_time", general.fixed_width_time);
	setValue(g, "hidden_song_headers", general.hidden_song_headers);
	setValue(g, "last_device", general.last_device);
	setValue(g, "last_playlist", general.last_playlist);
	setValue(g, "last_version", general.last_version);
	setValue(g, "last_volume", general.last_volume);
	setValue(g, "media_controller", general.media_controller);
	setValue(g, "notify_track_change", general.notify_track_change);
	setValue(g, "playlist_order", general.playlist_order);
	setValue(g, "pulse_volume", general.pulse_volume);
	setValue(g, "refresh_interval", general.refresh_interval);
	setValue(g, "relative_added", general.relative_added);
	setValue(g, "show_changelog", general.show_changelog);
	setValue(g, "song_header_sort_by", general.song_header_sort_by);
	setValue(g, "style", general.style);
	setValue(g, "style_palette", general.style_palette);
	setValue(g, "track_list_resize_mode", general.track_list_resize_mode);
	setValue(g, "track_numbers", general.track_numbers);
	setValue(g, "tray_album_art", general.tray_album_art);
	setValue(g, "tray_icon", general.tray_icon);
	setValue(g, "tray_light_icon", general.tray_light_icon);

	// Spotify
	setValue(s, "always_start", spotify.always_start);
	setValue(s, "backend", spotify.backend);
	setValue(s, "bitrate", spotify.bitrate);
	setValue(s, "disable_discovery", spotify.disable_discovery);
	setValue(s, "global_config", spotify.global_config);
	setValue(s, "keyring_password", spotify.keyring_password);
	setValue(s, "max_queue", spotify.max_queue);
	setValue(s, "path", spotify.path);
	setValue(s, "start_client", spotify.start_client);
	setValue(s, "username", spotify.username);

	// Qt widgets
	// TODO: This should be loaded dynamically for non-Qt clients
	lib::json::get(json, "Qt", qt_settings);
}

void settings::load()
{
	auto name = file_name();

	std::ifstream file(name);
	if (!file.is_open())
	{
		log::warn("Failed to load settings: \"{}\" not found", name);
		return;
	}

	try
	{
		nlohmann::json json;
		file >> json;
		from_json(json);
	}
	catch (const nlohmann::json::exception &e)
	{
		log::error("Failed to load settings: {}", e.what());
	}

	file.close();
}

auto settings::to_json() const -> nlohmann::json
{
	return {
		{"Account", {
			{"access_token", account.access_token},
			{"client_id", account.client_id},
			{"client_secret", account.client_secret},
			{"last_refresh", account.last_refresh},
			{"refresh_token", account.refresh_token},
		}},
		{"General", {
			{"custom_playlist_order", general.custom_playlist_order},
			{"fallback_icons", general.fallback_icons},
			{"fixed_width_time", general.fixed_width_time},
			{"hidden_song_headers", general.hidden_song_headers},
			{"last_device", general.last_device},
			{"last_playlist", general.last_playlist},
			{"last_version", general.last_version},
			{"last_volume", general.last_volume},
			{"media_controller", general.media_controller},
			{"notify_track_change", general.notify_track_change},
			{"playlist_order", general.playlist_order},
			{"pulse_volume", general.pulse_volume},
			{"refresh_interval", general.refresh_interval},
			{"relative_added", general.relative_added},
			{"show_changelog", general.show_changelog},
			{"song_header_sort_by", general.song_header_sort_by},
			{"style", general.style},
			{"style_palette", general.style_palette},
			{"track_list_resize_mode", general.track_list_resize_mode},
			{"track_numbers", general.track_numbers},
			{"tray_album_art", general.tray_album_art},
			{"tray_icon", general.tray_icon},
			{"tray_light_icon", general.tray_light_icon},
		}},
		{"Spotify", {
			{"always_start", spotify.always_start},
			{"backend", spotify.backend},
			{"bitrate", spotify.bitrate},
			{"disable_discovery", spotify.disable_discovery},
			{"global_config", spotify.global_config},
			{"keyring_password", spotify.keyring_password},
			{"max_queue", spotify.max_queue},
			{"path", spotify.path},
			{"start_client", spotify.start_client},
			{"username", spotify.username},
		}},
		{"Qt", qt_settings},
	};
}

void settings::save()
{
	std::lock_guard<std::mutex> lock(mutex);

	auto file_dir = file_path();
	if (!ghc::filesystem::exists(file_dir))
	{
		ghc::filesystem::create_directories(file_dir);
	}

	std::ofstream file(file_name());
	file << std::setw(4) << to_json();
	file.close();
}

void settings::remove_client()
{
	account.client_id = std::string();
	account.client_secret = std::string();
}

void settings::remove_tokens()
{
	account.access_token = std::string();
	account.refresh_token = std::string();
}

auto settings::get_dark_theme() const -> bool
{
	return general.style_palette == lib::palette::dark;
}

void settings::set_dark_theme(bool value)
{
	general.style_palette = value ? lib::palette::dark : lib::palette::app;
}
