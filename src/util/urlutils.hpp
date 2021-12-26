#pragma once

#include "enum/linktype.hpp"
#include "dialog/openlink.hpp"

#include <QString>
#include <QWidget>
#include <QDesktopServices>

class UrlUtils
{
public:
	/** Open URL of specific type */
	static void open(const QString &url, LinkType linkType, QWidget *parent);

	/** Open URL as std::string of specific type */
	static void open(const std::string &url, LinkType linkType, QWidget *parent);

private:
	UrlUtils() = default;
};
