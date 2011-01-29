#include "MailCheckerDlg.h"
#include "Setting.h"
#include <QMenu>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QProcess>

MailCheckerDlg::MailCheckerDlg(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);
	ui.labelIcon->setPixmap(QPixmap(":/MailChecker/Images/Mail.png").scaled(64, 64));

	setting = UserSetting::getInstance();
	loadSettings();

	model = new QSqlTableModel(this);
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("Accounts");
	model->select();

	mapper = new QDataWidgetMapper(this);
	mapper->setModel(model);
	mapper->addMapping(ui.leName,      NAME);
	mapper->addMapping(ui.cbProtocol,  PROTOCOL);
	mapper->addMapping(ui.leHost,      HOST);
	mapper->addMapping(ui.leUser,      USER);
	mapper->addMapping(ui.lePassword,  PASSWORD);
	mapper->addMapping(ui.sbPort,      PORT);
	mapper->addMapping(ui.checkSSL,    SSL);
	mapper->addMapping(ui.checkEnable, ENABLE);
	mapper->toFirst();

	ui.listView->setModel(model);

	connect(ui.buttonBoxGeneral, SIGNAL(accepted()), this, SLOT(onOK()));
	connect(ui.buttonBoxGeneral, SIGNAL(rejected()), this, SLOT(onCancel()));
	connect(ui.btAdd,            SIGNAL(clicked()),  this, SLOT(onAdd()));
	connect(ui.btDel,            SIGNAL(clicked()),  this, SLOT(onDel()));
	connect(ui.btSave,           SIGNAL(clicked()),  model, SLOT(submitAll()));
	connect(ui.btCancel,         SIGNAL(clicked()),  model, SLOT(revertAll()));
	connect(ui.listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
			this, SLOT(onSelectAccount(QModelIndex)));
	connect(ui.listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
			mapper, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui.cbProtocol, SIGNAL(currentIndexChanged(QString)), this, SLOT(onProtocolChanged(QString)));
}

MailCheckerDlg::~MailCheckerDlg() {
	model->submitAll();
}

void MailCheckerDlg::closeEvent(QCloseEvent* event)
{
	hide();
	event->ignore();
}

void MailCheckerDlg::onCancel()
{
	hide();
	model->revertAll();
}

void MailCheckerDlg::onOK()
{
	hide();
	model->submitAll();

	setting->setValue("Interval",         ui.sbInterval->value());
	setting->setValue("Timeout",          ui.sbTimeout->value());
	setting->setValue("ShowNotification", ui.sbNotification->value());
	setting->setValue("Popup",            ui.cbPopup->currentText() == tr("Yes"));
	setting->setValue("Sound",            ui.cbSound->currentText());
	setting->setValue("Application",      ui.cbApplication->currentText());
	setting->setValue("SoundFiles",       ui.cbSound->getFiles().join(";"));
	setting->setValue("ApplicationFiles", ui.cbApplication->getFiles().join(";"));
}

void MailCheckerDlg::loadSettings()
{
	ui.sbInterval    ->setValue(setting->value("Interval").toInt());
	ui.sbTimeout     ->setValue(setting->value("Timeout") .toInt());
	ui.sbNotification->setValue(setting->value("ShowNotification").toInt());
	ui.cbPopup->setCurrentIndex(setting->value("Popup").toBool() ? 0 : 1);

	QString soundFiles = setting->value("SoundFiles").toString();
	if(!soundFiles.isEmpty())
		ui.cbSound->setFiles(soundFiles.split(";"));
	QString applicationFiles = setting->value("ApplicationFiles").toString();
	if(!soundFiles.isEmpty())
		ui.cbApplication->setFiles(applicationFiles.split(";"));

	ui.cbSound      ->setCurrentText(setting->value("Sound").toString());
	ui.cbApplication->setCurrentText(setting->value("Application").toString());
	ui.checkSSL     ->setChecked    (setting->value("SSL").toBool());
}

void MailCheckerDlg::onAdd()
{
	int lastRow = model->rowCount();
	model->insertRow(lastRow);
	model->setData(model->index(lastRow, NAME),     tr("New Account"));
	model->setData(model->index(lastRow, PROTOCOL), "IMAP");
	model->setData(model->index(lastRow, PORT),     993);
	model->setData(model->index(lastRow, SSL),      true);
	model->setData(model->index(lastRow, ENABLE),   true);
	QModelIndex idx = model->index(lastRow, 0);
	ui.listView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
	onSelectAccount(idx);
	mapper->setCurrentModelIndex(idx);
	ui.leName->selectAll();
	ui.leName->setFocus();
}

void MailCheckerDlg::onDel()
{
	model->removeRow(currentRow);
	model->submitAll();
	ui.groupConfiguration->setEnabled(model->rowCount() > 0);
}

void MailCheckerDlg::onSelectAccount(const QModelIndex& idx)
{
	bool valid = idx.isValid();
	ui.groupConfiguration->setEnabled(valid);
	ui.btDel->setEnabled(valid);
	currentRow = valid ? idx.row() : -1;
	disconnect(ui.cbProtocol, SIGNAL(currentIndexChanged(QString)), this, SLOT(onProtocolChanged(QString)));
	ui.cbProtocol->setCurrentText(model->data(model->index(currentRow, PROTOCOL)).toString());
	connect(ui.cbProtocol, SIGNAL(currentIndexChanged(QString)), this, SLOT(onProtocolChanged(QString)));
}

void MailCheckerDlg::onProtocolChanged(const QString& protocol)
{
	model->setData(model->index(currentRow, PROTOCOL), protocol);
	ui.sbPort->setValue(protocol == "IMAP" ? 993 : 110);
}