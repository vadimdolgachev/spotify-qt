#include "trayicon.hpp"

#include <QWindow>

TrayIcon::TrayIcon(spt::Spotify *spotify, const lib::settings &settings, QWidget *parent)
	: QSystemTrayIcon(parent),
	spotify(spotify),
	settings(settings)
{
	callback = [this](const std::string &result)
	{
		this->message(QString::fromStdString(result));
	};

	contextMenu = new QMenu(parent);
	currentTrack = contextMenu->addAction("-");
	currentTrack->setEnabled(false);
	contextMenu->addSeparator();

	auto *previous = contextMenu->addAction(Icon::get("media-skip-backward"),
		"Previous");
	QAction::connect(previous, &QAction::triggered, [this, spotify]()
	{
		spotify->previous(this->callback);
	});

	playPause = contextMenu->addAction(Icon::get("media-playback-start"),
		"Play");
	QAction::connect(playPause, &QAction::triggered, [this](bool /*checked*/)
	{
		if (playback().is_playing)
		{
			this->spotify->pause(this->callback);
		}
		else
		{
			this->spotify->resume(this->callback);
		}
	});

	auto *next = contextMenu->addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(next, &QAction::triggered, [this, spotify]()
	{
		spotify->next(this->callback);
	});

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

	QMenu::connect(contextMenu, &QMenu::aboutToShow, [this]()
	{
		auto current = playback();
		currentTrack->setText(QString::fromStdString(current.item.title()));
		auto isPlaying = current.is_playing;
		playPause->setIcon(Icon::get(isPlaying ? "media-playback-pause" : "media-playback-start"));
		playPause->setText(isPlaying ? "Pause" : "Play");
	});
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

	showMessage("spotify-qt", message);
}

auto TrayIcon::playback() -> lib::spt::playback
{
	auto *mainWindow = dynamic_cast<MainWindow *>(this->parent());
	if (mainWindow == nullptr)
	{
		return lib::spt::playback();
	}
	return mainWindow->currentPlayback();
}

void TrayIcon::setPixmap(const QPixmap &pixmap)
{
	setIcon(ImageUtils::mask(pixmap));
}

void TrayIcon::setDefaultPixmap()
{
	constexpr int iconSize = 64;

	setIcon(Icon::get(QString("logo:spotify-qt-symbolic-%1")
		.arg(settings.general.tray_light_icon ? "light" : "dark"))
		.pixmap(iconSize, iconSize));
}
