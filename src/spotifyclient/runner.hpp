#pragma once

#include "lib/enum/clienttype.hpp"
#include "lib/settings.hpp"
#include "lib/logmessage.hpp"

#include "spotifyclient/helper.hpp"
#include "keyring/kwallet.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <QList>
#include <QProcess>
#include <QStringList>

namespace SpotifyClient
{
	/**
	 * Manages any running spotify client instance
	 */
	class Runner: public QObject
	{
	Q_OBJECT

	public:
		Runner(const lib::settings &settings, const lib::paths &paths, QWidget *parent);
		~Runner() override;

		auto start() -> QString;
		auto waitForStarted() const -> bool;

		/** Wrapper for spt::ClientHelper::availableBackends */
		auto availableBackends() -> QStringList;

		static auto getLog() -> const std::vector<lib::log_message> &;
		auto isRunning() const -> bool;

    signals:
        void started();

	private:
		QProcess *process = nullptr;
		QWidget *parentWidget = nullptr;
		QString path;
		static std::vector<lib::log_message> log;
		const lib::settings &settings;
		const lib::paths &paths;
		lib::client_type clientType;

		void readyRead() const;
		void readyError() const;
		static void logOutput(const QByteArray &output, lib::log_type logType);
	};
}
