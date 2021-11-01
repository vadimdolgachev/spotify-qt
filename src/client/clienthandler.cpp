#include "clienthandler.hpp"
#include "mainwindow.hpp"

QList<QPair<QDateTime, QString>> spt::ClientHandler::log;

spt::ClientHandler::ClientHandler(const lib::settings &settings,
	const lib::paths &paths, QWidget *parent)
	: QObject(parent),
	parentWidget(parent),
	settings(settings),
	paths(paths)
{
	path = QString::fromStdString(settings.spotify.path);
	process = new QProcess(parent);
	clientType = spt::ClientHelper::getClientType(path);
}

spt::ClientHandler::~ClientHandler()
{
	if (process != nullptr)
	{
		process->close();
	}
}

auto spt::ClientHandler::start() -> QString
{
	// Don't start if already running
	if (!settings.spotify.always_start && isRunning())
	{
		return {};
	}

	// Check if empty
	if (clientType == lib::client_type::none)
	{
		return "path is empty or invalid";
	}

	// Check if path exists
	QFileInfo info(path);
	if (!info.exists())
	{
		return "file in path does not exist";
	}

	// If using global config, just start
	if (settings.spotify.global_config && clientType == lib::client_type::spotifyd)
	{
		process->start(path, QStringList());
		return {};
	}

	// Check if username exists
	auto username = QString::fromStdString(settings.spotify.username);
	if (username.isEmpty())
	{
		return "no username provided";
	}

	// Get password from keyring if set
	QString password;
#ifdef USE_DBUS
	KWallet keyring(username);
	if (settings.spotify.keyring_password && keyring.unlock())
	{
		password = keyring.readPassword();
	}
#endif

	// Ask for password
	if (password.isEmpty())
	{
		password = QInputDialog::getText(parentWidget,
			"Enter password",
			QString("Enter password for Spotify user \"%1\":").arg(username),
			QLineEdit::Password);

		if (password.isEmpty())
		{
			return "no password provided";
		}

#ifdef USE_DBUS
		if (settings.spotify.keyring_password)
		{
			keyring.writePassword(password);
		}
#endif
	}

	// Common arguments
	QStringList arguments({
		"--bitrate", QString::number(static_cast<int>(settings.spotify.bitrate)),
		"--username", username,
		"--password", password
	});

	// librespot specific
	if (clientType == lib::client_type::librespot)
	{
		arguments.append({
			"--name", "spotify-qt (librespot)",
			"--initial-volume", "100",
			"--autoplay",
			"--cache", QString::fromStdString(paths.cache() / "librespot"),
		});
	}
	else if (clientType == lib::client_type::spotifyd)
	{
		arguments.append({
			"--no-daemon",
			"--device-name", "spotify-qt (spotifyd)",
		});
	}

	auto backend = QString::fromStdString(settings.spotify.backend);
	if (backend.isEmpty() && supportsPulse())
	{
		backend = "pulseaudio";
	}

	if (!backend.isEmpty())
	{
		arguments.append({
			"--backend", backend
		});
	}

	if (clientType == lib::client_type::librespot && settings.spotify.disable_discovery)
	{
		arguments.append("--disable-discovery");
	}

    QProcess::connect(process, &QProcess::readyReadStandardOutput, this, &ClientHandler::readyRead);
    QProcess::connect(process, &QProcess::readyReadStandardError, this, &ClientHandler::readyError);
    QProcess::connect(process, &QProcess::stateChanged,
                      this, [this, arguments](QProcess::ProcessState state) {
                          if (state == QProcess::ProcessState::NotRunning) {
                              process->close();
                              QTimer::singleShot(1000, [=]{process->start(path, arguments);});
                          }
        });

	lib::log::debug("starting: {} {}", path.toStdString(),
		arguments.join(' ').toStdString());

	process->start(path, arguments);
	return {};
}

auto spt::ClientHandler::waitForStarted() const -> bool
{
	if (process == nullptr)
	{
		return false;
	}
	constexpr int timeout = 3 * 1000;
	return process->waitForStarted(timeout);
}

auto spt::ClientHandler::availableBackends() -> QStringList
{
	return spt::ClientHelper::availableBackends(path);
}

auto spt::ClientHandler::supportsPulse() -> bool
{
	return spt::ClientHelper::supportsPulse(path);
}

auto spt::ClientHandler::isRunning() const -> bool
{
	return process == nullptr
		? spt::ClientHelper::isRunning(path)
		: process->isOpen();
}

void spt::ClientHandler::logOutput(const QByteArray &output)
{
	for (auto &line: QString(output).split('\n'))
	{
		if (line.isEmpty())
		{
			continue;
		}
		log << QPair<QDateTime, QString>(QDateTime::currentDateTime(), line);
	}
}

void spt::ClientHandler::readyRead() const
{
	logOutput(process->readAllStandardOutput());
}

void spt::ClientHandler::readyError() const
{
	logOutput(process->readAllStandardError());
}

auto spt::ClientHandler::getLog() -> const QList<QPair<QDateTime, QString>> &
{
	return log;
}
