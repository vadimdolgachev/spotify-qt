#include "util/style.hpp"

bool Style::darkBackground = false;

void Style::applyPalette(lib::palette palette)
{
	QPalette p;
	switch (palette)
	{
		case lib::palette::app:
			p = QApplication::palette();
			break;

		case lib::palette::style:
			p = QApplication::style()->standardPalette();
			break;

		case lib::palette::dark:
			p = DarkPalette();
			break;
	}

	QApplication::setPalette(p);
}

void Style::setDarkBackground(QWidget *widget)
{
	const auto bg = widget->palette().color(widget->backgroundRole());
	constexpr auto threshold = 128;
	darkBackground = ((bg.red() + bg.green() + bg.blue()) / 3) < threshold;
}

auto Style::getDarkBackground() -> bool
{
	return darkBackground;
}
