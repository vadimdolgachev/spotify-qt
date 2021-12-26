#include "dialog/deviceselect.hpp"

Dialog::DeviceSelect::DeviceSelect(const std::vector<lib::spt::device> &devices,
	QWidget *parent)
	: QDialog(parent),
	devices(devices)
{
	auto *layout = new QVBoxLayout();
	setLayout(layout);
	setWindowTitle("Select device");

	list = new QListWidget(this);
	for (const auto &device: devices)
	{
		list->addItem(QString::fromStdString(device.name));
	}
	layout->addWidget(list);

	QListWidget::connect(list, &QListWidget::itemDoubleClicked,
		[this](QListWidgetItem */*item*/)
		{
			accept();
		});

	auto *buttons = new QDialogButtonBox();
	auto *okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		accept();
	});

	auto *cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool /*checked*/)
	{
		reject();
	});
	layout->addWidget(buttons);
}

auto Dialog::DeviceSelect::selectedDevice() -> lib::spt::device
{
	if (list->selectedItems().isEmpty())
	{
		return {};
	}

	for (auto &device: devices)
	{
		if (device.name == list->selectedItems().first()->text().toStdString())
		{
			return device;
		}
	}

	return {};
}
