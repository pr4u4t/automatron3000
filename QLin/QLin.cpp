#include "QLin.h"
#include "settingsdialog.h"
#include "../core/core.h"

#include <QApplication>
#include <QCoreApplication>
#include <QTranslator>


struct QLinMenu {
	QLinMenu(QCoreApplication* app)
		: m_app(app) {
		if (m_app != nullptr) {
			m_translator = new QTranslator();
			if (m_translator->load(QLocale::system(), "QLin", "_", "translations")) { //set directory of ts
				m_app->installTranslator(m_translator);
			}
		}

		//m_serialMenu = new QMenu(m_app->translate("MainWindow", "&Serial"));
		m_actionConfigure = new QAction(m_app->translate("MainWindow", "Settings"));
		m_actionConfigure->setData(QVariant("QLin/Settings"));
		//m_serialMenu->addAction(m_actionConfigure);

		m_actionConnect = new QAction(app->translate("MainWindow", "Connect"));
		//m_serialMenu->addAction(m_actionConnect);

		m_actionDisconnect = new QAction(app->translate("MainWindow", "Disconnect"));
		//m_serialMenu->addAction(m_actionDisconnect);
	}

	QMenu* m_linbusMenu = nullptr;
	QAction* m_actionConfigure = nullptr;
	QAction* m_actionConnect = nullptr;
	QAction* m_actionDisconnect = nullptr;
	QTranslator* m_translator = nullptr;
	QCoreApplication* m_app = nullptr;
};

static bool QLin_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinMenu* ctx, Logger* log) {
	log->message("QLin_register()", LoggerSeverity::LOG_DEBUG);
	
	//qRegisterMetaType<ChannelConfig>("ChannelConfig");

	GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
	if (gtx == nullptr) {
		log->message("PluginList_register(): application is non gui not registering");
		return false;
	}

	if (gtx->m_win == nullptr) {
		log->message("QLin_register(): window pointer == nullptr");
		return false;
	}

	ctx->m_linbusMenu = gtx->m_win->menuBar()->addMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
	ctx->m_linbusMenu->addAction(ctx->m_actionConnect);
	ctx->m_linbusMenu->addAction(ctx->m_actionDisconnect);
	ctx->m_linbusMenu->addAction(ctx->m_actionConfigure);

	QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
	if (windowMenu != nullptr) {
		gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_linbusMenu);
	}

	QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, gtx, ctx, log] {
		QSharedPointer<Plugin> serial = ld->instance("QLin", gtx->m_win);
		SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, serial->settingsPath());
		QObject::connect(dialog, &MdiChild::message, log, &Logger::message);
		QObject::connect(dialog, &SettingsDialog::settingsUpdated, serial.dynamicCast<QLin>().data(), &QLin::settingsChanged);
		gtx->m_win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Lin/Settings"));
	});

	QObject::connect(ctx->m_actionConnect, &QAction::triggered, [ld, gtx, ctx, log] {
		QSharedPointer<Plugin> serial = ld->instance("QLin", gtx->m_win);
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
		if (ld->hasInstance("QLin") == false) {
			return;
		}

		QSharedPointer<Plugin> serial = ld->instance("QLin", gtx->m_win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();
		if (io->isOpen()) {
			io->close();
			ctx->m_actionDisconnect->setEnabled(false);
			ctx->m_actionConnect->setEnabled(true);
			ctx->m_actionConfigure->setEnabled(true);
		}
	});

	if (ld->hasInstance("QLin") == true) {

	} else {
		ctx->m_actionConfigure->setEnabled(true);
		ctx->m_actionConnect->setEnabled(true);
		ctx->m_actionDisconnect->setEnabled(false);
	}

	return true;
}

static bool QLin_unregister(ModuleLoaderContext* win, PluginsLoader* ld, QLinMenu* ctx, Logger* log) {
	log->message("QLin_unregister()", LoggerSeverity::LOG_DEBUG);
	return true;
}

REGISTER_PLUGIN(
	QLin,
	Plugin::Type::IODEVICE,
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"example plugin",
	QLin_register,
	QLin_unregister,
	QLinMenu,
	{},
	false
)

QLin::QLin(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path)
	: IODevice(ld, plugins, parent, path)
	, m_open(false)
	, m_settings(SettingsDialog::LinSettings(Settings::get(), settingsPath()))
	, m_lin(new CLin(&m_settings)) {
	connect(m_lin, &CLin::message, this, &QLin::message);
	connect(m_lin, &CLin::dataReady, this, &QLin::dataReady);
}

bool QLin::open(const QString& url) {
	emit message("QLin::open()", LoggerSeverity::LOG_DEBUG);
	if (m_lin == nullptr) {
		return false;
	}

	if (m_lin->LINOpen() != XL_SUCCESS) {
		emit message("QLin::open(): LINOpen failed", LoggerSeverity::LOG_ERROR);
		close();
		return false;
	}
	/*
	if (m_lin->LINInit(
		m_settings.mode == SettingsDialog::LinSettings::Mode::MASTER ? m_settings.masterID : m_settings.slaveID,
		m_settings.linVersion,
		m_settings.baudrate,
		m_settings.mode == SettingsDialog::LinSettings::Mode::MASTER ? 1 : 2,
		m_settings.slaveID,
		(unsigned char*) m_settings.initialData.data(),
		m_settings.initialData.size()
		) != XL_SUCCESS) {
		emit message("QLin::open(): LINInit failed");
		close();
		return false;
	}
	*/

	auto sld = dynamic_cast<PluginLoader<QLin, QLinMenu>*>(loader());
	if (sld->context()) {
		sld->context()->m_actionConnect->setEnabled(false);
		sld->context()->m_actionDisconnect->setEnabled(true);
	}

	return (m_open = true);
}

qint64 QLin::write(const QString& data) {
	emit message("QLin::write()", LoggerSeverity::LOG_DEBUG);
	XLstatus rc;

	if (m_lin == nullptr) {
		emit message("QLin::write(): not initialized", LoggerSeverity::LOG_ERROR);
		return -1;
	}
	
	if (m_open != true) {
		emit message("QLin::write(): not open", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (m_settings.mode == SettingsDialog::LinSettings::Mode::SLAVE) {
		emit message("QLin::write(): SLAVE mode cannot send", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (data.isEmpty()) {
		emit message("QLin::write(): data empty", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	QStringList list = data.split(" ");
	switch (list.size()) {
		case 1:
			if ((rc = m_lin->LINSendMasterReq(list[0].toUInt())) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return sizeof(list[0].toInt());

		case 2:
			if ((rc = m_lin->LINSendMasterReq(list[0].toUInt(), (unsigned char*) list[1].toLocal8Bit().data(), list[1].size())) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return sizeof(list[0].toInt())+sizeof(BYTE);


		default:
			emit message("QLin::write: failed invalid data: 'linID(int) [data(BYTE)]'", LoggerSeverity::LOG_ERROR);
			return -1;
	}

	return -1;
}

qint64 QLin::write(const QByteArray& data) {
	emit message("QLin::write()");
	XLstatus rc;

	if (m_lin == nullptr) {
		emit message("QLin::write(): m_lin == nullptr", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (m_open != true) {
		emit message("QLin::write(): lin not open", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (m_settings.mode == SettingsDialog::LinSettings::Mode::SLAVE) {
		emit message("QLin::write(): not in master mode write unavailable", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (data.isEmpty()) {
		emit message("QLin::write(): data is empty", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	QByteArrayList list = data.split(' ');

	switch (list.size()) {
		case 1:
			if ((rc = m_lin->LINSendMasterReq(list[0].data()[0])) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return sizeof(list[0].toInt());

		case 2:
			if ((rc = m_lin->LINSendMasterReq(list[0].data()[0], (unsigned char*) list[1].data(), list[1].size())) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return sizeof(list[0].toInt()) + sizeof(BYTE);


		default:
			emit message("QLin::write: failed invalid data: 'linID(int) [data(BYTE)]'", LoggerSeverity::LOG_ERROR);
			return -1;
	}

	return -1;
}

QString QLin::read(qint64 maxLen) {
	return QString();
}

qint64 QLin::bytesAvailable() {
	return -1;
}

void QLin::close() {
	emit message("QLin::close()", LoggerSeverity::LOG_DEBUG);
	if (m_lin != nullptr) {
		m_lin->LINClose();
		auto sld = dynamic_cast<PluginLoader<QLin, QLinMenu>*>(loader());
		if (sld->context()) {
			sld->context()->m_actionConnect->setEnabled(true);
			sld->context()->m_actionDisconnect->setEnabled(false);
		}
		m_open = false;
	}
}

bool QLin::isOpen() const {
	emit message("QLin::isOpen()", LoggerSeverity::LOG_DEBUG);
	if (m_lin == nullptr) {
		return true;
	}
	return m_open;
}

bool QLin::flush() {
	emit message("QLin::flush()", LoggerSeverity::LOG_DEBUG);
	return true;
}

void QLin::settingsChanged() {
	emit message("QLin::settingsChanged()", LoggerSeverity::LOG_DEBUG);
	m_settings = SettingsDialog::LinSettings(Settings::get(), settingsPath());
	close();
	open(QString());
}
