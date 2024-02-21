#include "QSerial.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QCoreApplication>
#include <QTranslator>

struct QSerialMenu {
	QSerialMenu(QCoreApplication *app)
	: m_app(app){
		if (m_app != nullptr) {
			m_translator = new QTranslator();
			if (m_translator->load(QLocale::system(), "QSerial", "_", "translations")) { //set directory of ts
				m_app->installTranslator(m_translator);
			}
		}

		m_serialMenu = new QMenu(m_app->translate("MainWindow", "&Serial"));

		m_actionConfigure = new QAction(m_app->translate("MainWindow", "Settings"), m_serialMenu);
		m_actionConfigure->setData(QVariant("QSerial"));
		m_serialMenu->addAction(m_actionConfigure);

		m_actionConnect = new QAction(app->translate("MainWindow", "Connect"), m_serialMenu);
		m_serialMenu->addAction(m_actionConnect);

		m_actionDisconnect = new QAction(app->translate("MainWindow", "Disconnect"), m_serialMenu);
		m_serialMenu->addAction(m_actionDisconnect);
	}

	QMenu* m_serialMenu = nullptr;
	QAction* m_actionConfigure = nullptr;
	QAction* m_actionConnect = nullptr;
	QAction* m_actionDisconnect = nullptr;
	QTranslator* m_translator = nullptr;

	QCoreApplication* m_app = nullptr;
};

static bool QSerial_register(Window* win, PluginsLoader* ld, QSerialMenu* ctx, Logger* log) {
	log->message("QSerial_register()");

	if (win == nullptr) {
		return false;
	}

	QMenu* windowMenu = win->findMenu(ctx->m_app->translate("MainWindow", "&Window"));
	if (windowMenu != nullptr) {
		win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_serialMenu);
	}
	
	QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, win, ctx] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", win);
		SettingsDialog* dialog = new SettingsDialog(win, nullptr, serial->settingsPath());
		win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Serial Port-Settings"));
	});
	
	QObject::connect(ctx->m_actionConnect, &QAction::triggered, [ld, win, ctx, log] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();
		if (io->isOpen() == false) {
			if (io->open() == true) {
				ctx->m_actionConnect->setEnabled(false);
				ctx->m_actionDisconnect->setEnabled(true);
				ctx->m_actionConfigure->setEnabled(false);
			}
		}
	});
	
	QObject::connect(ctx->m_actionDisconnect, &QAction::triggered, [ld, win, ctx, log] {
		if (ld->hasInstance("QSerial") == false) {
			return;
		}

		QSharedPointer<Plugin> serial = ld->instance("QSerial", win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();
		if (io->isOpen()) {
			io->close();
			ctx->m_actionDisconnect->setEnabled(false);
			ctx->m_actionConnect->setEnabled(true);
			ctx->m_actionConfigure->setEnabled(true);
		}
	});

	if (ld->hasInstance("QSerial") == true) {
		
	} else {
		ctx->m_actionConfigure->setEnabled(true);
		ctx->m_actionConnect->setEnabled(true);
		ctx->m_actionDisconnect->setEnabled(false);
	}

	return true;
}

static bool QSerial_unregister(Window* win, PluginsLoader* ld, QSerialMenu* ctx, Logger* log) {
	log->message("QSerial_unregister()");
	return true;
}

REGISTER_PLUGIN(
	QSerial,
	Plugin::Type::IODEVICE,
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"example plugin",
	QSerial_register,
	QSerial_unregister,
	QSerialMenu
)

QSerial::QSerial(const Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path)
	: IODevice(ld, plugins, parent, path),
	  m_serial(new QSerialPort(this)){
	emit message("QSerial::QSerial()", LoggerSeverity::LOG_DEBUG);

	Window* win = qobject_cast<Window*>(parent);

	const SettingsDialog::SerialSettings p(win->settings(), settingsPath());
	m_serial->setPortName(p.name);
	m_serial->setBaudRate(p.baudRate);
	m_serial->setDataBits(p.dataBits);
	m_serial->setParity(p.parity);
	m_serial->setStopBits(p.stopBits);
	m_serial->setFlowControl(p.flowControl);

	if (p.autoConnect) {
		emit message(QString("QSerial::QSerial serial port open: %1").arg(open(QString()) ? tr("Success") : tr("Failed")));
	}
}

bool QSerial::open(const QString& url) {
	emit message("QSerial::open()", LoggerSeverity::LOG_DEBUG);

	if (m_serial == nullptr) {
		return false;
	}

	if (m_serial->isOpen()) {
		return true;
	}

    if (m_serial->open(QIODevice::ReadWrite)) {
		emit message("QSerial::open: success");

        //showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
        //    .arg(p.name, QString::number(p.baudRate), QString::number(p.dataBits),
        //        QString::number(p.parity), QString::number(p.stopBits), QString::number(p.flowControl)), 0);
    } else {
		emit message(QString("QSerial::open: failed: %1").arg(m_serial->errorString()));
        //TODO: display status: probably using log
        //showStatusMessage(tr("Open error"));
    }

	//m_serial->op

	return true;
}

qint64 QSerial::write(const QString& data) {
	emit message("QSerial::write()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->write(data.toLocal8Bit()) : -1;
}

QString QSerial::read(qint64 maxLen) {
	emit message("QSerial::read()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->read(maxLen) : QString();
}

qint64 QSerial::bytesAvailable() {
	emit message("QSerial::bytesAvailable()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->bytesAvailable() : -1;
}

void QSerial::close() {
	emit message("QSerial::close()", LoggerSeverity::LOG_DEBUG);
	if (m_serial != nullptr) {
		m_serial->close();
	}
}

bool QSerial::isOpen() const {
	emit message("QSerial::isOpen()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->isOpen() : false;
}
