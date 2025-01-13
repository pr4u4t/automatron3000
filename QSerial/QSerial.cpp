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
	}
	QMenu* m_serialMenu = nullptr;
	QMenu* m_settings = nullptr;
	QAction* m_newInstance = nullptr;
	QTranslator* m_translator = nullptr;

	QCoreApplication* m_app = nullptr;
};

static bool QSerial_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QSerialMenu* ctx, Logger* log) {
	log->message("QSerial_register()");

	GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
	if (gtx == nullptr) {
		log->message("QSerial_register(): application is non gui not registering");
		return false;
	}

	if (gtx->m_win == nullptr) {
		log->message("QSerial_register(): window pointer == nullptr");
		return false;
	}

	auto ioMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "Input/Output"));

	if (ioMenu == nullptr) {
		log->message("QSerial_register(): i/o menu not found");
		return false;
	}

	if ((ctx->m_serialMenu = ioMenu->addMenu(ctx->m_app->translate("MainWindow", "&Serial"))) == nullptr) {
		log->message("QSerial_register(): failed to create serial menu");
		return false;
	}

	windowAddInstanceSettings(ctx->m_serialMenu, &ctx->m_settings, &ctx->m_newInstance, "QSerial", ctx->m_app, log);

	QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
	QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
		windowAddPluginSettingsAction<QSerial, QSerialMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QSerial"), gtx, ctx, log);
	});

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
	true,
	500,
	SerialSettings
)

QSerial::QSerial(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path, SerialSettings* set, const QString& uuid)
	: IODevice(ld, plugins, parent, path, set, uuid) //Settings::get(), path)),
	,  m_serial(new QSerialPort(this)){
	emit message("QSerial::QSerial()", LoggerSeverity::LOG_DEBUG);

	
}

bool QSerial::initialize() {

	const auto set = settings<SerialSettings>();
	*set = SerialSettings(Settings::get(), settingsPath());

	m_serial->setPortName(set->name());
	m_serial->setBaudRate(set->baudRate());
	m_serial->setDataBits(set->dataBits());
	m_serial->setParity(set->parity());
	m_serial->setStopBits(set->stopBits());
	m_serial->setFlowControl(set->flowControl());

	QObject::connect(m_serial, &QSerialPort::readyRead, this, &QSerial::readData);

	if (settings<SerialSettings>()->autoConnect()) {
		emit message(QString("QSerial::QSerial serial port open: %1").arg(open(QString()) ? tr("Success") : tr("Failed")));
	}

	return true;
}

bool QSerial::deinitialize() {
	return true;
}

SettingsMdi* QSerial::settingsWindow() const {
	auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
	QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QSerial::settingsChanged);
	return ret;
}

bool QSerial::open(const QString& url) {
	emit message("QSerial::open()", LoggerSeverity::LOG_DEBUG);

	if (m_serial == nullptr) {
		return false;
	}

	if (m_serial->isOpen()) {
		return true;
	}

	return m_serial->open(QIODevice::ReadWrite);
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

	*(settings<SerialSettings>()) = SerialSettings(Settings::get(), settingsPath());
	
	/*
	m_serial->setPortName(m_settings.name);
	m_serial->setBaudRate(m_settings.baudRate);
	m_serial->setDataBits(m_settings.dataBits);
	m_serial->setParity(m_settings.parity);
	m_serial->setStopBits(m_settings.stopBits);
	m_serial->setFlowControl(m_settings.flowControl);
	*/

	emit settingsApplied();
}

void QSerial::readData() {
	QByteArray data = m_serial->readAll();
	emit dataReady(data);
}

bool QSerial::flush() {
	return m_serial->flush();
}