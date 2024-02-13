#include "QSerial.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QCoreApplication>
#include <QTranslator>

bool QSerial_register(Window* win, PluginsLoader* ld) {
	if (win == nullptr) {
		return false;
	}

	QCoreApplication* app = QApplication::instance();

	QTranslator* translator = new QTranslator();
	if (translator->load(QLocale::system(), "QSerial", "_", "translations")) { //set directory of ts
		app->installTranslator(translator);
	}

	QMenu* serialMenu = win->menuBar()->addMenu(app->translate("MainWindow", "&Serial"));
	QAction* m_actionConfigure = new QAction(app->translate("MainWindow","Settings"), win);
	m_actionConfigure->setData(QVariant("QSerial"));
	QObject::connect(m_actionConfigure, &QAction::triggered, [ld, win, app] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", win);
		SettingsDialog* dialog = new SettingsDialog(win, nullptr, serial->settingsPath());
		win->addSubWindow(dialog, app->translate("MainWindow", "Serial Port-Settings"));
	});
	serialMenu->addAction(m_actionConfigure);

	QAction* m_actionConnect = new QAction(app->translate("MainWindow", "Connect"), win);
	QObject::connect(m_actionConnect, &QAction::triggered, [ld, win] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();


	});
	serialMenu->addAction(m_actionConnect);

	QAction* m_actionDisconnect = new QAction(app->translate("MainWindow", "Disconnect"), win);
	QObject::connect(m_actionDisconnect, &QAction::triggered, [ld, win] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();
		

	});
	serialMenu->addAction(m_actionDisconnect);
	

	/*
	QAction* database = new QAction(Window::tr("Database"), win);
	database->setData(QVariant("QData"));
	QObject::connect(database, &QAction::triggered, win, &Window::createOrActivate);

	QMenu* menu = win->findMenu(QApplication::translate("MainWindow", "&File"));
	if (menu == nullptr) {
		if ((menu = win->menuBar()->addMenu(QApplication::translate("MainWindow", "&File"))) == nullptr) {
			return false;
		}
	}

	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions[1], database);
	*/
	
	return true;
}

bool QSerial_unregister(Window* win, PluginsLoader* ld) {
	return true;
}

REGISTER_PLUGIN(
	QSerial,
	Plugin::Type::IODEVICE,
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"example plugin",
	QSerial_register,
	QSerial_unregister
)

QSerial::QSerial(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path)
	: IODevice(ld, plugins, parent, path),
	  m_serial(new QSerialPort(this)){

	Window* win = qobject_cast<Window*>(parent);

	const SettingsDialog::SerialSettings p(win->settings(), settingsPath());
	m_serial->setPortName(p.name);
	m_serial->setBaudRate(p.baudRate);
	m_serial->setDataBits(p.dataBits);
	m_serial->setParity(p.parity);
	m_serial->setStopBits(p.stopBits);
	m_serial->setFlowControl(p.flowControl);

	if (p.autoConnect) {
		qDebug() << open(QString());
	}
}

bool QSerial::open(const QString& url) {
	if (m_serial == nullptr) {
		return false;
	}

	if (m_serial->isOpen()) {
		return true;
	}

    if (m_serial->open(QIODevice::ReadWrite)) {
        //m_console->setLocalEchoEnabled(p.localEchoEnabled);
        
        //m_actionConnect->setEnabled(false);
        //m_actionDisconnect->setEnabled(true);
        //m_actionConfigure->setEnabled(false);

        //showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
        //    .arg(p.name, QString::number(p.baudRate), QString::number(p.dataBits),
        //        QString::number(p.parity), QString::number(p.stopBits), QString::number(p.flowControl)), 0);
    } else {
        //TODO: get window pointer
        QMessageBox::critical(nullptr, tr("Serial port open error"), m_serial->errorString());

        //TODO: display status: probably using log
        //showStatusMessage(tr("Open error"));
    }

	//m_serial->op

	return true;
}

qint64 QSerial::write(const QString& data) {
	return (m_serial != nullptr) ? m_serial->write(data.toLocal8Bit()) : -1;
}

QString QSerial::read(qint64 maxLen) {
	return (m_serial != nullptr) ? m_serial->read(maxLen) : QString();
}

qint64 QSerial::bytesAvailable() {
	return (m_serial != nullptr) ? m_serial->bytesAvailable() : -1;
}

void QSerial::close() {
	if (m_serial != nullptr) {
		m_serial->close();
	}
}
