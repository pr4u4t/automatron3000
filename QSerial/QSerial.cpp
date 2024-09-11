#include "QSerial.h"
#include "settingsdialog.h"
#include <QApplication>
#include <QCoreApplication>
#include <QTranslator>

#include "../core/core.h"

struct QSerialMenu {
	QSerialMenu(QCoreApplication *app)
	: m_app(app){
		if (m_app != nullptr && Settings::localeNeeded()) {
			m_translator = new QTranslator();
			if (m_translator->load(QLocale::system(), "QSerial", "_", "translations")) { //set directory of ts
				m_app->installTranslator(m_translator);
			}
		}

		//m_serialMenu = new QMenu(m_app->translate("MainWindow", "&Serial"));
		m_actionConfigure = new QAction(m_app->translate("MainWindow", "Settings"));
		m_actionConfigure->setData(QVariant("QSerial"));
		//m_serialMenu->addAction(m_actionConfigure);

		m_actionConnect = new QAction(app->translate("MainWindow", "Connect"));
		//m_serialMenu->addAction(m_actionConnect);

		m_actionDisconnect = new QAction(app->translate("MainWindow", "Disconnect"));
		//m_serialMenu->addAction(m_actionDisconnect);
	}

	QMenu* m_serialMenu = nullptr;
	QAction* m_actionConfigure = nullptr;
	QAction* m_actionConnect = nullptr;
	QAction* m_actionDisconnect = nullptr;
	QTranslator* m_translator = nullptr;

	QCoreApplication* m_app = nullptr;
};

static bool QSerial_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QSerialMenu* ctx, Logger* log) {
	log->message("QSerial_register()");

	GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
	if (gtx == nullptr) {
		log->message("PluginList_register(): application is non gui not registering");
		return false;
	}

	if (gtx->m_win == nullptr) {
		return false;
	}
	ctx->m_serialMenu = new QMenu(ctx->m_app->translate("MainWindow", "&Serial"), gtx->m_win->menuBar());
	ctx->m_serialMenu->addAction(ctx->m_actionConnect);
	ctx->m_serialMenu->addAction(ctx->m_actionDisconnect);
	ctx->m_serialMenu->addAction(ctx->m_actionConfigure);
	
	QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
	if (windowMenu != nullptr) {
		gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_serialMenu);
	}
	
	QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, gtx, ctx] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", gtx->m_win);
		SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, serial->settingsPath());
		QObject::connect(dialog, &SettingsDialog::settingsUpdated, serial.dynamicCast<QSerial>().data(), &QSerial::settingsChanged);
		gtx->m_win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Serial Port-Settings"));
	});
	
	QObject::connect(ctx->m_actionConnect, &QAction::triggered, [ld, gtx, ctx, log] {
		QSharedPointer<Plugin> serial = ld->instance("QSerial", gtx->m_win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();
		if (io->isOpen() == false) {
			if (io->open() == true) {
				ctx->m_actionConnect->setEnabled(false);
				ctx->m_actionDisconnect->setEnabled(true);
				ctx->m_actionConfigure->setEnabled(false);
			}
		}
	});
	
	QObject::connect(ctx->m_actionDisconnect, &QAction::triggered, [ld, gtx, ctx, log] {
		if (ld->hasInstance("QSerial") == false) {
			return;
		}

		QSharedPointer<Plugin> serial = ld->instance("QSerial", gtx->m_win);
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

static bool QSerial_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QSerialMenu* ctx, Logger* log) {
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
	QSerialMenu,
	{},
	true
)

QSerial::QSerial(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path)
	: IODevice(ld, plugins, parent, path, new SettingsDialog::SerialSettings(Settings::get(), path)),
	  m_serial(new QSerialPort(this)){
	emit message("QSerial::QSerial()", LoggerSeverity::LOG_DEBUG);

	Window* win = qobject_cast<Window*>(parent);

	m_serial->setPortName(settings<SettingsDialog::SerialSettings>()->name);
	m_serial->setBaudRate(settings<SettingsDialog::SerialSettings>()->baudRate);
	m_serial->setDataBits(settings<SettingsDialog::SerialSettings>()->dataBits);
	m_serial->setParity(settings<SettingsDialog::SerialSettings>()->parity);
	m_serial->setStopBits(settings<SettingsDialog::SerialSettings>()->stopBits);
	m_serial->setFlowControl(settings<SettingsDialog::SerialSettings>()->flowControl);

	QObject::connect(m_serial, &QSerialPort::readyRead, this, &QSerial::readData);

	if (settings<SettingsDialog::SerialSettings>()->autoConnect) {
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

	auto sld = dynamic_cast<PluginLoader<QSerial, QSerialMenu>*>(loader());

    if (m_serial->open(QIODevice::ReadWrite)) {
		emit message("QSerial::open: success");
		if (sld->context()) {
			sld->context()->m_actionConnect->setEnabled(false);
			sld->context()->m_actionDisconnect->setEnabled(true);
		}
        
    } else {
		emit message(QString("QSerial::open: failed: %1").arg(m_serial->errorString()));
		if (sld->context()) {
			sld->context()->m_actionConnect->setEnabled(true);
			sld->context()->m_actionDisconnect->setEnabled(false);
		}
		//TODO: display status: probably using log
        //showStatusMessage(tr("Open error"));
		return false;
    }

	//m_serial->op

	return true;
}

qint64 QSerial::write(const QString& data) {
	emit message("QSerial::write()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->write(data.toLocal8Bit()) : -1;
}

qint64 QSerial::write(const QByteArray& data) {
	emit message("QSerial::write()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->write(data) : -1;
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

	auto sld = dynamic_cast<PluginLoader<QSerial, QSerialMenu>*>(loader());
	if (sld->context()) {
		sld->context()->m_actionConnect->setEnabled(true);
		sld->context()->m_actionDisconnect->setEnabled(false);
	}
}

bool QSerial::isOpen() const {
	emit message("QSerial::isOpen()", LoggerSeverity::LOG_DEBUG);
	return (m_serial != nullptr) ? m_serial->isOpen() : false;
}

void QSerial::settingsChanged() {

	bool wasOpened = false;

	if ((wasOpened = m_serial->isOpen()) == true) {
		m_serial->close();
	}

	Window* win = qobject_cast<Window*>(parent());
	*(settings<SettingsDialog::SerialSettings>()) = SettingsDialog::SerialSettings(Settings::get(), settingsPath());
	
	/*
	m_serial->setPortName(m_settings.name);
	m_serial->setBaudRate(m_settings.baudRate);
	m_serial->setDataBits(m_settings.dataBits);
	m_serial->setParity(m_settings.parity);
	m_serial->setStopBits(m_settings.stopBits);
	m_serial->setFlowControl(m_settings.flowControl);
	*/

	if (wasOpened == true) {
		if (m_serial->open(QIODevice::ReadWrite) == true) {
			emit message("QSerial::open: success");

			auto sld = dynamic_cast<PluginLoader<QSerial, QSerialMenu>*>(loader());
			if (sld->context()) {
				sld->context()->m_actionConnect->setEnabled(false);
				sld->context()->m_actionDisconnect->setEnabled(true);
			}

		} else {
			emit message(QString("QSerial::open: failed: %1").arg(m_serial->errorString()));
			
			auto sld = dynamic_cast<PluginLoader<QSerial, QSerialMenu>*>(loader());
			if (sld->context()) {
				sld->context()->m_actionConnect->setEnabled(true);
				sld->context()->m_actionDisconnect->setEnabled(false);
			}
		}
	}
}

void QSerial::readData() {
	QByteArray data = m_serial->readAll();
	emit dataReady(data);
}

bool QSerial::flush() {
	return m_serial->flush();
}