#include "trayicon.hpp"
#include "mainwindow.hpp"

#include <QWindow>
TrayIcon::TrayIcon(lib::spt::api &spotify, const lib::settings &settings,
	const lib::cache &cache, QWidget *parent)
	: QSystemTrayIcon(parent),
	spotify(spotify),
	settings(settings),
	cache(cache)
{
	callback = [this](const std::string &result)
	{
		message(QString::fromStdString(result));
	};

	contextMenu = new QMenu(parent);
	currentTrack = contextMenu->addAction("-");
	currentTrack->setEnabled(false);
	contextMenu->addSeparator();

	const auto previousIcon = Icon::get("media-skip-backward");
	previous = contextMenu->addAction(previousIcon, "Previous");
	QAction::connect(previous, &QAction::triggered,
		this, &TrayIcon::onPrevious);

	const auto playPauseIcon = Icon::get("media-playback-start");
	playPause = contextMenu->addAction(playPauseIcon, "Play");
	QAction::connect(playPause, &QAction::triggered,
		this, &TrayIcon::onPlayPause);

	next = contextMenu->addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(next, &QAction::triggered,
		this, &TrayIcon::onNext);

	contextMenu->addSeparator();
	auto *quit = contextMenu->addAction(Icon::get("application-exit"), "Quit");
	QAction::connect(quit, &QAction::triggered, QCoreApplication::quit);

	setDefaultPixmap();
	setContextMenu(contextMenu);
	show();

	QSystemTrayIcon::connect(this, &QSystemTrayIcon::activated, [this](ActivationReason reason)
	{
		if (reason == ActivationReason::MiddleClick) {
				playPause->trigger();
				return;
		}
		if (reason != ActivationReason::Trigger)
		{
			return;
		}
		auto *window = dynamic_cast<QWidget *>(this->parent());
		if (window != nullptr)
		{
			if (window->isVisible() && window->isActiveWindow())
			{
				window->hide();
			}
			else
			{
				if (!window->isVisible()) {
					window->windowHandle()->setPosition(window->windowHandle()->position());
				}
				if (!window->isActiveWindow()) {
					window->hide();
				}
				window->show();
				window->raise();
				QTimer::singleShot(0, [window] {window->windowHandle()->requestActivate();});
			}
		}
	});

	QMenu::connect(contextMenu, &QMenu::aboutToShow,
		this, &TrayIcon::onMenuAboutToShow);
}

TrayIcon::~TrayIcon()
{
	contextMenu->deleteLater();
}

void TrayIcon::message(const QString &message)
{
	if (message.isNull() || message.isEmpty())
	{
		return;
	}

	showMessage(APP_NAME, message);
}

void TrayIcon::message(const lib::spt::track &track, const QPixmap &pixmap)
{
	if (!track.is_valid())
	{
		return;
	}

	const auto title = QString::fromStdString(track.name);
	const auto artists = lib::spt::entity::combine_names(track.artists);
	const auto message = QString::fromStdString(artists);

	QIcon icon(pixmap);
	showMessage(title, message, icon, messageTrackTimeout);
}

auto TrayIcon::playback() -> lib::spt::playback
{
	auto *mainWindow = qobject_cast<MainWindow *>(this->parent());
	if (mainWindow == nullptr)
	{
		return {};
	}
	return mainWindow->currentPlayback();
}

void TrayIcon::setPixmap(const QPixmap &pixmap)
{
	setIcon(Image::mask(pixmap));
}

void TrayIcon::setDefaultPixmap()
{
	constexpr int iconSize = 64;

	setIcon(Icon::get(QString("logo:%1-symbolic-%2")
		.arg(APP_ICON)
		.arg(settings.general.tray_light_icon ? "light" : "dark"))
		.pixmap(iconSize, iconSize));
}

void TrayIcon::onPrevious(bool /*checked*/)
{
	spotify.previous(callback);
}

void TrayIcon::onPlayPause(bool /*checked*/)
{
	if (playback().is_playing)
	{
		spotify.pause(callback);
	}
	else
	{
		spotify.resume(callback);
	}
}

void TrayIcon::onNext(bool /*checked*/)
{
	spotify.next(callback);
}

void TrayIcon::onActivated(ActivationReason reason)
{
	if (reason != ActivationReason::Trigger)
	{
		return;
	}

	auto *parentWidget = qobject_cast<QWidget *>(parent());
	if (parentWidget != nullptr)
	{
		parentWidget->setVisible(!parentWidget->isVisible());
	}
}

void TrayIcon::onMenuAboutToShow()
{
	auto current = playback();
	auto isPlaying = current.is_playing;

	currentTrack->setText(QString::fromStdString(current.item.title()));

	playPause->setIcon(Icon::get(isPlaying
		? "media-playback-pause"
		: "media-playback-start"));

	playPause->setText(isPlaying
		? "Pause"
		: "Play");
}
