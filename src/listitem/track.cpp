#include "listitem/track.hpp"

ListItem::Track::Track(const QStringList &strings,
	const lib::spt::track &track,
	const QIcon &icon,
	int index)
	: QTreeWidgetItem(strings)
{
	setIcon(0, icon);

	auto addedAt = QDateTime::fromString(QString::fromStdString(track.added_at),
		Qt::DateFormat::ISODate);

	setData(0, static_cast<int>(DataRole::Track), QVariant::fromValue(track));
	setData(0, static_cast<int>(DataRole::Index), index);
	setData(0, static_cast<int>(DataRole::AddedDate), addedAt);
	setData(0, static_cast<int>(DataRole::Length), track.duration);

	if (track.is_local || !track.is_playable)
	{
		setDisabled(true);
		setToolTip(static_cast<int>(Column::Title), track.is_local
			? "Local track"
			: "Unavailable");
	}

	// Artist
	auto names = lib::spt::entity::combine_names(track.artists, "\n");
	setToolTip(static_cast<int>(Column::Artist), QString::fromStdString(names));

	// Title, album
	for (auto i = 1; i < strings.length() - 2; i++)
	{
		if (toolTip(i).isEmpty())
		{
			setToolTip(i, strings.at(i));
		}
	}

	// Length
	auto length = strings.at(strings.length() - 2).split(':');
	if (length.length() >= 2)
	{
		setToolTip(strings.length() - 2,
			QString("%1m %2s (%3s total)")
				.arg(length.at(0), length.at(1))
				.arg(track.duration / 1000));
	}

	// Added
	if (!DateTime::isEmpty(addedAt))
	{
		setToolTip(strings.length() - 1,
			QLocale().toString(addedAt.date()));
	}
}

auto ListItem::Track::operator<(const QTreeWidgetItem &item) const -> bool
{
	auto column = static_cast<Column>(treeWidget()->sortColumn());

	// Track number
	if (column == Column::Index)
	{
		return data(0, static_cast<int>(DataRole::Index)).toInt()
			< item.data(0, static_cast<int>(DataRole::Index)).toInt();
	}

	// Length
	if (column == Column::Length)
	{
		return data(0, static_cast<int>(DataRole::Length)).toInt()
			< item.data(0, static_cast<int>(DataRole::Length)).toInt();
	}

	// Added
	if (column == Column::Added)
	{
		return data(0, static_cast<int>(DataRole::AddedDate)).toDateTime()
			< item.data(0, static_cast<int>(DataRole::AddedDate)).toDateTime();
	}

	return removePrefix(text(static_cast<int>(column)))
		.compare(removePrefix(item.text(static_cast<int>(column))),
			Qt::CaseInsensitive) < 0;
}

auto ListItem::Track::removePrefix(const QString &str) -> QString
{
	return str.startsWith("The ", Qt::CaseInsensitive)
		? str.right(str.length() - 4)
		: str;
}
