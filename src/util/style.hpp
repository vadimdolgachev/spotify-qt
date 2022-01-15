#pragma once

#include "lib/enum/palette.hpp"
#include "darkpalette.hpp"

#include <QPalette>
#include <QApplication>
#include <QWidget>
#include <QStyle>

class Style
{
public:
	/** Apply specified palette to current application */
	static void applyPalette(lib::palette palette);

	/** Determine if widget has a dark background */
	static void setDarkBackground(QWidget *widget);

	/** Get if window has been set as having a dark background */
	static auto getDarkBackground() -> bool;

private:
	Style() = default;

	static bool darkBackground;
};
