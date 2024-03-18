#include "QLin.h"

#include "settingsdialog.h"
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

static bool QLin_register(Window* win, PluginsLoader* ld, QLinMenu* ctx, Logger* log) {
	log->message("QLin_register()");

	if (win == nullptr) {
		log->message("QLin_register(): window pointer == nullptr");
		return false;
	}

	ctx->m_linbusMenu = win->menuBar()->addMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
	ctx->m_linbusMenu->addAction(ctx->m_actionConnect);
	ctx->m_linbusMenu->addAction(ctx->m_actionDisconnect);
	ctx->m_linbusMenu->addAction(ctx->m_actionConfigure);

	QMenu* windowMenu = win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
	if (windowMenu != nullptr) {
		win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_linbusMenu);
	}

	QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, win, ctx] {
		QSharedPointer<Plugin> serial = ld->instance("QLin", win);
		SettingsDialog* dialog = new SettingsDialog(win, nullptr, serial->settingsPath());
		QObject::connect(dialog, &SettingsDialog::settingsUpdated, serial.dynamicCast<QLin>().data(), &QLin::settingsChanged);
		win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Lin/Settings"));
		});

	QObject::connect(ctx->m_actionConnect, &QAction::triggered, [ld, win, ctx, log] {
		QSharedPointer<Plugin> serial = ld->instance("QLin", win);
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
		if (ld->hasInstance("QLin") == false) {
			return;
		}

		QSharedPointer<Plugin> serial = ld->instance("QLin", win);
		QSharedPointer<IODevice> io = serial.dynamicCast<IODevice>();
		if (io->isOpen()) {
			io->close();
			ctx->m_actionDisconnect->setEnabled(false);
			ctx->m_actionConnect->setEnabled(true);
			ctx->m_actionConfigure->setEnabled(true);
		}
		});

	if (ld->hasInstance("QLin") == true) {

	}
	else {
		ctx->m_actionConfigure->setEnabled(true);
		ctx->m_actionConnect->setEnabled(true);
		ctx->m_actionDisconnect->setEnabled(false);
	}

	return true;
}

static bool QLin_unregister(Window* win, PluginsLoader* ld, QLinMenu* ctx, Logger* log) {
	log->message("QLin_unregister()");
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
	{}
)

QLin::QLin(Loader* ld, PluginsLoader* plugins, QObject* parent, const QString& path)
	: IODevice(ld, plugins, parent, path)
	, m_lin(new CLin("QLin"))
	, m_open(false){
	connect(m_lin, &CLin::message, this, &QLin::message);
	connect(m_lin, &CLin::dataReady, this, &QLin::dataReady);
}

bool QLin::open(const QString& url) {
	if (m_lin == nullptr) {
		return false;
	}

	if (m_lin->LINGetDevice() != XL_SUCCESS) {
		return false;
	}
	
	if (m_lin->LINInit(m_settings.master ? m_settings.masterID : m_settings.slaveID, m_settings.linVersion) != XL_SUCCESS) {
		close();
		return false;
	}

	return (m_open = true);
}

qint64 QLin::write(const QString& data) {
	XLstatus rc;

	if (m_lin == nullptr) {
		return -1;
	}
	
	if (m_open != true) {
		return -1;
	}

	if (m_settings.master != true) {
		return -1;
	}

	if (data.isEmpty()) {
		return -1;
	}

	QStringList list = data.split(" ");
	switch (list.size()) {
		case 1:
			if ((rc = m_lin->LINSendMasterReq(list[0].toInt())) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc));
				return -1;
			}
			return sizeof(list[0].toInt());

		case 2:
			if ((rc = m_lin->LINSendMasterReq(list[0].toInt(), list[1].toLocal8Bit().data()[0])) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc));
				return -1;
			}
			return sizeof(list[0].toInt())+sizeof(BYTE);


		default:
			emit message("QLin::write: failed invalid data: 'linID(int) [data(BYTE)]'");
			return -1;
	}

	return -1;
}

qint64 QLin::write(const QByteArray& data) {
	emit message("QLin::write()");
	XLstatus rc;

	if (m_lin == nullptr) {
		emit message("QLin::write(): m_lin == nullptr");
		return -1;
	}

	if (m_open != true) {
		emit message("QLin::write(): lin not open");
		return -1;
	}

	if (m_settings.master != true) {
		emit message("QLin::write(): not in master mode write unavailable");
		return -1;
	}

	if (data.isEmpty()) {
		emit message("QLin::write(): data is empty");
		return -1;
	}

	QByteArrayList list = data.split(' ');

	switch (list.size()) {
		case 1:
			if ((rc = m_lin->LINSendMasterReq(list[0].data()[0])) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc));
				return -1;
			}
			return sizeof(list[0].toInt());

		case 2:
			if ((rc = m_lin->LINSendMasterReq(list[0].toInt(), list[1].data()[0])) != XL_SUCCESS) {
				emit message(QString("QLin::write: send maser request failed %1").arg(rc));
				return -1;
			}
			return sizeof(list[0].toInt()) + sizeof(BYTE);


		default:
			emit message("QLin::write: failed invalid data: 'linID(int) [data(BYTE)]'");
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
	emit message("QLin::close()");
	if (m_lin != nullptr) {
		m_lin->LINClose();
	}
}

bool QLin::isOpen() const {
	emit message("QLin::isOpen()");
	if (m_lin == nullptr) {
		return true;
	}
	return m_open;
}

bool QLin::flush() {
	emit message("QLin::flush()");
	return true;
}

void QLin::settingsChanged() {
	emit message("QLin::settingsChanged()");
	close();
	open(QString());
}
