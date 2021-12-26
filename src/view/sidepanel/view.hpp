#pragma once

#include "lib/spotify/track.hpp"
#include "view/artist/view.hpp"
#include "view/search/view.hpp"
#include "view/audiofeatures.hpp"
#include "view/lyricsview.hpp"
#include "view/sidepanel/title.hpp"
#include "enum/sidepaneltype.hpp"

#include <QTabWidget>
#include <QStackedWidget>

namespace SidePanel
{
	class View: public QDockWidget
	{
	Q_OBJECT

	public:
		View(lib::spt::api &spotify, const lib::settings &settings, lib::cache &cache,
			const lib::http_client &httpClient, QWidget *parent);

		void openArtist(const std::string &artistId);
		void openAudioFeatures(const lib::spt::track &track);
		void openLyrics(const lib::spt::track &track);

		void openSearch();
		void closeSearch();

		void addTab(QWidget *widget, const QString &icon, const QString &tabTitle,
			SidePanelType type, const QString &name);

		void removeTab(int index);

		void setTabText(QWidget *widget, const QString &text);

	private:
		SidePanel::Title *title = nullptr;
		QStackedWidget *stack = nullptr;

		QWidget *searchView = nullptr;

		lib::spt::api &spotify;
		const lib::settings &settings;
		lib::cache &cache;
		const lib::http_client &httpClient;

		void setCurrentIndex(int index);
		void setCurrentWidget(QWidget *widget);

		auto findTab(SidePanelType type, const QString &name) -> QWidget *;

		template<typename T>
		auto find(const QString &name = QString()) -> T
		{
			return stack->findChild<T>(name, Qt::FindDirectChildrenOnly);
		}

		void onTabMoved(int from, int to);
	};
}
