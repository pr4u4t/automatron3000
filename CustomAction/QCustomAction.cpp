#include "QCustomAction.h"

#include "QCustomAction.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qcustomaction.h"

#include <QtGlobal>
#include <QtGui>
#include <QScrollBar>
#include <QFileDialog>

struct QCustomActionMenu {
    QCustomActionMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QCustomAction", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_actionsMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QCustomAction_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCustomActionMenu* ctx, Logger* log) {
    log->message("QCustomAction_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QCustomAction_register(): application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("QCustomAction_register(): window pointer == nullptr");
        return false;
    }

    auto actionsMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "Actions"));
    ctx->m_actionsMenu = actionsMenu->addMenu(ctx->m_app->translate("MainWindow", "CustomAction"));

    windowAddInstanceSettings(ctx->m_actionsMenu, &ctx->m_settings, &ctx->m_newInstance, "QCustomAction", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QCustomAction, QCustomActionMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QCustomAction"), gtx, ctx, log);
    });

    return true;
}

static bool QCustomAction_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCustomActionMenu* ctx, Logger* log) {
    log->message("QCustomAction_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QCustomAction,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QCustomAction_register,
    QCustomAction_unregister,
    QCustomActionMenu,
    {},
    true,
    100,
    CustomActionSettings
)

QCustomAction::QCustomAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, CustomActionSettings* set, const QString& uuid)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        set,
        uuid
    )
    , m_data(new Ui::QCustomActionUI) {
    m_data.m_ui->setupUi(this);
    QObject::connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QCustomAction::execClicked);
    QObject::connect(m_data.m_ui->clearButton, &QPushButton::clicked, this, &QCustomAction::clearLog);
    QObject::connect(m_data.m_ui->exportButton, &QPushButton::clicked, this, &QCustomAction::exportLog);
    QObject::connect(m_data.m_ui->stopButton, &QPushButton::clicked, this, &QCustomAction::stopClicked);
    QObject::connect(m_data.m_ui->pauseButton, &QPushButton::clicked, this, &QCustomAction::pauseClicked);
}

bool QCustomAction::initialize() {
    const auto set = settings<CustomActionSettings>();
    m_data.m_ui->pushButton->setText(set->buttonText());
    m_data.m_ui->title->setText(set->title());
    m_data.m_ui->progressBar->setHidden(!set->progress());
    m_data.m_ui->textEdit->setHidden(!set->verbose());
    return true;
}

bool QCustomAction::deinitialize() {
    return true;
}

bool QCustomAction::saveSettings() {
    return true;
}

/*!
  \brief Destructor
  \param none
*/
QCustomAction::~QCustomAction() {
}

void QCustomAction::exportLog() const {
    emit message("QCustomAction::exportLog()");
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save File"),
        ".",
        tr("Text (*.txt)"));
    if (fileName.isEmpty()) {
        emit message("QCustomAction::exportLog: no file selected");
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit message(QString("QCustomAction::exportLog: failed to open file %1").arg(fileName));
        return;
    }
    QString log = m_data.m_ui->textEdit->toPlainText();
    file.write(log.toLocal8Bit());
    file.close();
}

void QCustomAction::clearLog() const {
    m_data.m_ui->textEdit->clear();
    m_data.m_ui->progressBar->setValue(0);
}

void QCustomAction::settingsChanged() {
    emit message("QCustomAction::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<CustomActionSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<CustomActionSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

SettingsMdi* QCustomAction::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QCustomAction::settingsChanged);
    return ret;
}

void QCustomAction::execClicked(bool checked) {
    const auto set = settings<CustomActionSettings>();
    QList<Job> jobs = set->jobList();
    m_data.m_state = QCustomActionState::INPROGRESS;
    m_data.m_ui->progressBar->setMaximum(jobs.size());
    int i = 0;
    for (const auto job : jobs) {
        auto plugin = plugins()->findByObjectName(job.m_module);
        auto object = plugin.dynamicCast<QObject>();
        //mere this into union
        bool retVal = false;
        QVariant ret;
        int tries;

        connect(object.data(), SIGNAL(message(const QString&, LoggerSeverity)), this, SLOT(jobMessage(const QString&, LoggerSeverity)));

        jobMessage(QString("Executing.............%1").arg(object->objectName()));

        for (tries = 0; tries < set->numberOfRetries(); ++tries) {
            switch (job.m_type) {
            case JobReturnType::VOID:
                QMetaObject::invokeMethod(object.data(), job.m_function.toLocal8Bit().data(),
                    Qt::DirectConnection);
                emit message(QString("QCustomAction::execClicked: %1::%2").arg(job.m_module).arg(job.m_function));
                goto JOB_SUCCESS;

            case JobReturnType::BOOLEAN:
                QMetaObject::invokeMethod(object.data(), job.m_function.toLocal8Bit().data(),
                    Qt::DirectConnection, Q_RETURN_ARG(bool, retVal));
                emit message(QString("QCustomAction::execClicked: %1::%2: status: %3").arg(job.m_module).arg(job.m_function).arg(retVal));
                if (retVal == true) {
                    goto JOB_SUCCESS;
                } else break;

            case JobReturnType::QVARIANT:
                QMetaObject::invokeMethod(object.data(), job.m_function.toLocal8Bit().data(),
                    Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret));
                emit message(QString("QCustomAction::execClicked: %1::%2: status: %3").arg(job.m_module).arg(job.m_function).arg(ret.isNull()));
                if (ret.isNull() == false) {
                    goto JOB_SUCCESS;
                } else break;
            }

            for (auto start = QDateTime::currentMSecsSinceEpoch(); QDateTime::currentMSecsSinceEpoch() - start < set->retryInterval() && m_data.m_state != QCustomActionState::INPROGRESS;) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, set->retryInterval());
            }
        }

        emit message("QCustomAction::execClicked: job failed");
        return;

    JOB_SUCCESS:

        m_data.m_ui->progressBar->setValue(++i);
        jobMessage(QString("Progress.............%1").arg(m_data.m_ui->progressBar->text()));
        for (auto start = QDateTime::currentMSecsSinceEpoch(); QDateTime::currentMSecsSinceEpoch() - start < set->interval() && m_data.m_state == QCustomActionState::INPROGRESS;) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, set->interval());
        }

        const auto value = disconnect(object.data(), SIGNAL(message(const QString&, LoggerSeverity)), this, SLOT(jobMessage(const QString&, LoggerSeverity)));
        emit message(QString("QCustomAction::execClicked: disconnect result: %1").arg(value));

        if (m_data.m_state != QCustomActionState::INPROGRESS) {
            break;
        }
    }

    //m_ui->progressBar->setValue(m_ui->progressBar->maximum());
}

void QCustomAction::jobMessage(const QString& msg, LoggerSeverity severity) {
    QColor color = m_data.m_ui->textEdit->textColor();

    switch (severity) {
    case LoggerSeverity::LOG_INFO:
        m_data.m_ui->textEdit->setTextColor(QColor(56, 61, 65));
        break;

    case LoggerSeverity::LOG_NOTICE:
        m_data.m_ui->textEdit->setTextColor(QColor(12, 84, 96));
        break;

    case LoggerSeverity::LOG_WARNING:
        m_data.m_ui->textEdit->setTextColor(QColor(133, 100, 4));
        break;

    case LoggerSeverity::LOG_ERROR:
        m_data.m_ui->textEdit->setTextColor(QColor(114, 28, 36));
        break;

    case LoggerSeverity::LOG_DEBUG:
        break;
    }

    m_data.m_ui->textEdit->append(msg);
    m_data.m_ui->textEdit->setTextColor(color);
    m_data.m_ui->textEdit->verticalScrollBar()->setValue(m_data.m_ui->textEdit->verticalScrollBar()->maximum());
}

bool QCustomAction::reset(Reset type) {
    emit message("QCustomAction::reset(Reset type)");
    m_data.m_ui->progressBar->setValue(0);
    m_data.m_ui->textEdit->clear();
    
    return true;
}
