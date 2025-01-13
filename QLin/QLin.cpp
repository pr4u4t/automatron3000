#include "QLin.h"
#include "settingsdialog.h"
#include "../core/core.h"

#include <QApplication>
#include <QCoreApplication>
#include <QTranslator>
#include <QStyle>

struct QLinMenu {
	QLinMenu(QCoreApplication* app)
		: m_app(app) {

		if (app != nullptr && Settings::localeNeeded()) {
			m_translator = new QTranslator();
			if (m_translator->load(QLocale::system(), "QLin", "_", "translations")) { //set directory of ts
				m_app->installTranslator(m_translator);
			}
		}
	}

	QMenu* m_linbusMenu = nullptr;
	QAction* m_newInstance = nullptr;
	QMenu* m_settings = nullptr;

	QCoreApplication* m_app = nullptr;
	QTranslator* m_translator = nullptr;
};

static bool QLin_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinMenu* ctx, Logger* log) {
	log->message("QLin_register()");

	GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
	if (gtx == nullptr) {
		log->message("QLin_register(): application is non gui not registering");
		return false;
	}

	if (gtx->m_win == nullptr) {
		log->message("QLin_register(): window pointer == nullptr");
		return false;
	}

	auto ioMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "Input/Output"));
	ctx->m_linbusMenu = ioMenu->addMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
	
	windowAddInstanceSettings(ctx->m_linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLin", ctx->m_app, log);

	QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
	QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
		windowAddPluginSettingsAction<QLin, QLinMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLin"), gtx, ctx, log);
	});
	
	return true;
}

static bool QLin_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinMenu* ctx, Logger* log) {
	log->message("QLin_unregister()");
	return true;
}

REGISTER_PLUGIN(
	QLin,
	Plugin::Type::IODEVICE,
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"Vector Gmbh VN16XX Lin Driver",
	QLin_register,
	QLin_unregister,
	QLinMenu,
	{},
	false,
	600,
	LinSettings
)

QLin::QLin(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path, LinSettings* set, const QString& uuid)
	: IODevice(ld, plugins, parent, path, set, uuid)
	, m_data(new CLin(settings<LinSettings>()), false) {
	connect(m_data.m_lin, &CLin::message, this, &QLin::message);
	connect(m_data.m_lin, &CLin::dataReady, this, &QLin::dataReady);
	connect(m_data.m_lin, &CLin::error, this, &QLin::error);
}

bool QLin::initialize() {
	const auto set = settings<LinSettings>();
	*set = LinSettings(Settings::get(), settingsPath());
	return true;
}

bool QLin::deinitialize() {
	return false;
}

void QLin::settingsChanged() {
	emit message("QLin::settingsChanged()", LoggerSeverity::LOG_DEBUG);
	const auto set = settings<LinSettings>();
	//*set = LinSettings(Settings::get(), settingsPath());
	*set = *(Settings::fetch<LinSettings>(settingsPath()));
	close();
	open(QString());
	emit settingsApplied();
}

int QLin::slaveID() const {
	return settings<LinSettings>()->slaveID();
}

bool QLin::open(const QString& url) {
	emit message("QLin::open()", LoggerSeverity::LOG_DEBUG);
	if (m_data.m_lin == nullptr) {
		return false;
	}

	if (m_data.m_lin->LINOpen() != XL_SUCCESS) {
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

	//auto sld = dynamic_cast<PluginLoader<QLin, QLinMenu>*>(loader());
	//TODO:
	//if (sld->context()) {
	//	sld->context()->m_actionConnect->setEnabled(false);
	//	sld->context()->m_actionDisconnect->setEnabled(true);
	//}

	return (m_data.m_open = true);
}

SettingsMdi* QLin::settingsWindow() const {
	auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
	QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLin::settingsChanged);
	return ret;
}

qint64 QLin::write(const QString& data) {
	emit message("QLin::write()", LoggerSeverity::LOG_DEBUG);
	XLstatus rc;

	if (m_data.m_lin == nullptr) {
		emit message("QLin::write(): not initialized", LoggerSeverity::LOG_ERROR);
		return -1;
	}
	
	if (m_data.m_open != true) {
		emit message("QLin::write(): not open", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (settings<LinSettings>()->mode() == LinSettings::Mode::SLAVE) {
		emit message("QLin::write(): SLAVE mode cannot send", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (data.isEmpty()) {
		emit message("QLin::write(): data empty", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (data.size() > 9) {
		emit message("QLin::write(): data too long", LoggerSeverity::LOG_ERROR);
		emit message("QLin::write: failed invalid data: 'linID(int)[data(BYTE)]'", LoggerSeverity::LOG_ERROR);
		return -1;
	}
	
	switch (data.size()) {
		case 1:
			if ((rc = m_data.m_lin->LINSendMasterReq(static_cast<unsigned char>(data.toLocal8Bit()[0]))) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return data.size();

		default:
			if ((rc = m_data.m_lin->LINSendMasterReq(
					static_cast<unsigned char>(data.toLocal8Bit()[0]), 
					reinterpret_cast<unsigned char*>(data.toLocal8Bit().data()+1), 
					data.size()-1)
			) != XL_SUCCESS) {
				emit message(QString("QLin::write: send master request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return data.size();
	}

	return -1;
}

qint64 QLin::write(const QByteArray& data) {
	emit message("QLin::write()");
	XLstatus rc;

	if (m_data.m_lin == nullptr) {
		emit message("QLin::write(): m_lin == nullptr", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (m_data.m_open != true) {
		emit message("QLin::write(): lin not open", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (settings<LinSettings>()->mode() == LinSettings::Mode::SLAVE) {
		emit message("QLin::write(): not in master mode write unavailable", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (data.isEmpty()) {
		emit message("QLin::write(): data is empty", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	if (data.size() > 9) {
		emit message("QLin::write(): data too long", LoggerSeverity::LOG_ERROR);
		emit message("QLin::write: failed invalid data: 'linID(int)[data(BYTE)]'", LoggerSeverity::LOG_ERROR);
		return -1;
	}

	switch (data.size()) {
		case 1:
			if ((rc = m_data.m_lin->LINSendMasterReq(static_cast<unsigned char>(data[0]))) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return data.size();

		default:
			if ((rc = m_data.m_lin->LINSendMasterReq(
					static_cast<unsigned char>(data[0]), 
					reinterpret_cast<const unsigned char*>(data.constData()+1),
					data.size()-1)
			) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc), LoggerSeverity::LOG_ERROR);
				return -1;
			}
			return data.size();
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
	if (m_data.m_lin != nullptr) {
		m_data.m_lin->LINClose();
		//auto sld = dynamic_cast<PluginLoader<QLin, QLinMenu>*>(loader());
		//TODO:
		//if (sld->context()) {
		//	sld->context()->m_actionConnect->setEnabled(true);
		//	sld->context()->m_actionDisconnect->setEnabled(false);
		//}
		m_data.m_open = false;
	}

	emit closed();
}

bool QLin::isOpen() const {
	emit message("QLin::isOpen()", LoggerSeverity::LOG_DEBUG);
	if (m_data.m_lin == nullptr) {
		return true;
	}
	return m_data.m_open;
}

bool QLin::flush() {
	emit message("QLin::flush()", LoggerSeverity::LOG_DEBUG);
	return true;
}


