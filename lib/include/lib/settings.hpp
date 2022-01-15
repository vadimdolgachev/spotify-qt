#pragma once

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
#include "enum/spotifycontext.hpp"
#include "log.hpp"
#include "paths/paths.hpp"
#include "thirdparty/json.hpp"
#include "thirdparty/filesystem.hpp"
#include "settings/general.hpp"
#include "settings/account.hpp"
#include "settings/spotify.hpp"
#include "settings/qt.hpp"
#include "lib/json.hpp"

#include <mutex>

namespace lib
{
	/**
	 * Application settings
	 */
	class settings
	{
	public:
		/**
		 * Create new instance and attempt to load from file
		 */
		explicit settings(const paths &paths);

		/**
		 * Format settings in settings
		 * @return JSON
		 */
		auto to_json() const -> nlohmann::json;

		/**
		 * Load settings from json
		 * @param json JSON
		 */
		void from_json(const nlohmann::json &json);

		/**
		 * Save settings to file
		 */
		void save();

		/**
		 * Load settings from file
		 */
		void load();

		/**
		 * Account settings
		 */
		setting::account account;

		/**
		 * General settings
		 */
		setting::general general;

		/**
		 * Spotify settings
		 */
		setting::spotify spotify;

		/**
		 * Qt settings
		 */
		auto qt() -> setting::qt &;

		/**
		 * Reset client_id and client_secret properties
		 */
		void remove_client();

		/**
		 * Reset access_token and refresh_token properties
		 */
		void remove_tokens();

		/**
		 * Get if dark theme should be used
		 * @return Use dark theme
		 */
		auto get_dark_theme() const -> bool;

		/**
		 * Set if dark theme should be used
		 * @param value Dark theme should be used
		 */
		void set_dark_theme(bool value);

		/**
		 * Get config file name
		 * @return File name with path
		 */
		auto file_name() const -> std::string;

		/**
		 * Get path to config file name
		 * @return Path
		 */
		auto file_path() const -> std::string;

	private:
		/**
		 * paths implementation
		 */
		const paths &path;

		/**
		 * Mutex for making sure we don't write to settings from multiple
		 * threads
		 */
		std::mutex mutex;

		/**
		 * Qt settings
		 */
		std::unique_ptr<setting::qt> qt_settings;
	};
}
