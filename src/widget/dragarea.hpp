#pragma once

#include <QAction>
#include <QMouseEvent>
#include <QMainWindow>

class DragArea: public QWidget
{
public:
	explicit DragArea(QWidget *parent);

	DragArea(QWidget *target, QWidget *parent);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
	QWidget *target;
	QPoint dragPosition;

	void menu(const QPoint &pos);
	auto isWindowMaximized() -> bool;

	void onMaximize(bool checked);
};
