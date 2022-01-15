#include "createplaylist.hpp"
#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QMessageBox>

Dialog::CreatePlaylist::CreatePlaylist(std::string trackId, lib::spt::api &spotify, QWidget *parent)
	: Base(parent),
	trackId(std::move(trackId)),
	spotify(spotify)
{
	setTitle("Create playlist");

	auto *layout = Base::layout<QVBoxLayout>();
	layout->addWidget(new QLabel("Playlist name", this));

	playlistNameEdit = new QLineEdit(this);
	layout->addWidget(playlistNameEdit);

	addAction(DialogAction::Ok);
	addAction(DialogAction::Cancel);
}

void Dialog::CreatePlaylist::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		defaultName = QString("Playlist #%1")
			.arg(mainWindow->getPlaylistItemCount() + 1);

		playlistNameEdit->setPlaceholderText(defaultName);
	}
}

void Dialog::CreatePlaylist::onOk(bool /*checked*/)
{
	okButton()->setEnabled(false);

	const auto name = getPlaylistName().toStdString();
	if (name.empty())
	{
		okButton()->setEnabled(true);
		return;
	}

	const auto description = lib::optional<std::string>();
	const auto isPublic = lib::optional<bool>();
	const auto isCollaborative = lib::optional<bool>();

	spotify.create_playlist(name, description, isPublic, isCollaborative,
		[this](const lib::spt::playlist &playlist)
		{
			if (playlist.is_null())
			{
				showError(QStringLiteral("Failed to create playlist"));
				return;
			}

			const auto trackUri = lib::spt::api::to_uri("track", trackId);
			const auto playlistName = QString::fromStdString(playlist.name);

			spotify.add_to_playlist(playlist.id, trackUri,
				[this, playlistName](const std::string &result)
				{
					if (!result.empty())
					{
						showError(QString::fromStdString(result));
						return;
					}

					auto *mainWindow = MainWindow::find(parentWidget());
					mainWindow->refreshPlaylists();
					StatusMessage::info(QString("Added to %1").arg(playlistName));
					Base::onOk({});
				});
		});
}

auto Dialog::CreatePlaylist::getPlaylistName() const -> QString
{
	auto playlistText = playlistNameEdit->text();
	if (!playlistText.isEmpty())
	{
		return playlistText;
	}

	return defaultName;
}

void Dialog::CreatePlaylist::showError(const QString &message)
{
	okButton()->setEnabled(true);
	QMessageBox::critical(this, QStringLiteral("Error"), message);
}
