#include "logviewer.h"
#include "../api/api.h"
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QAction>
#include <QScrollBar>

#include "MainWindow.h"

LogViewer::LogViewer(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath)
    , m_text(new QTextEdit()){
    m_text->document()->setMaximumBlockCount(100);
    m_text->setReadOnly(true);
    
    MLoader* p = reinterpret_cast<MLoader*>(plugins);

    connect(p->logger(),&Logger::echo,this,&LogViewer::message);
    
    QBoxLayout *l = new QVBoxLayout();
    l->addWidget(m_text);
    setLayout(l);

    QContiguousCache<QString> cache = p->logger()->cache();
    //for (int i = 0; i < cache.size(); ++i) {
    while(cache.size() > 0){
        message(cache.takeFirst());
    }
}

SettingsMdi* LogViewer::settingsWindow() const {
    return nullptr;
}

void LogViewer::settingsChanged(){

}
    
void LogViewer::message(const QString& msg, LoggerSeverity severity){
    QColor color = m_text->textColor();
    
    switch (severity) {
    case LoggerSeverity::LOG_INFO:
        m_text->setTextColor(QColor(56, 61, 65));
        break;

    case LoggerSeverity::LOG_NOTICE:
        m_text->setTextColor(QColor(12, 84, 96));
        break;

    case LoggerSeverity::LOG_WARNING:
        m_text->setTextColor(QColor(133, 100, 4));
        break;

    case LoggerSeverity::LOG_ERROR:
        m_text->setTextColor(QColor(114, 28, 36));
        break;

    case LoggerSeverity::LOG_DEBUG:
        break;
    }

    m_text->append(msg);
    m_text->setTextColor(color);
    m_text->verticalScrollBar()->setValue(m_text->verticalScrollBar()->maximum());
}

struct LogViewerMenu {
    LogViewerMenu(QCoreApplication* app) 
        : m_app(app){
        if (app != nullptr) {
            m_logviewer = new QAction("Log Viewer");
            m_logviewer->setData(QVariant("LogViewer"));
        }
    }

    QAction* m_logviewer = nullptr;
    QCoreApplication* m_app = nullptr;
};

static bool LogViewer_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, LogViewerMenu* ctx, Logger* log) {
    log->message("LogViewer_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("LogViewer_register(): application is non gui not registering");
        return false;
    }

    ctx->m_app = QCoreApplication::instance();
    ctx->m_logviewer = new QAction("Log Viewer");
    ctx->m_logviewer->setData(QVariant("LogViewer"));
    ctx->m_logviewer->setText(ctx->m_app->translate("MainWindow", "Log Viewer"));

    QMenu* fileMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    ctx->m_logviewer->setParent(fileMenu);
    QObject::connect(ctx->m_logviewer, &QAction::triggered, gtx->m_win, &Window::createOrActivate);

    QList<QAction*> actions = fileMenu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
    fileMenu->insertAction(actions.size() > 0 ? actions[1] : nullptr, ctx->m_logviewer);
    
    return true;
}

static bool LogViewer_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, LogViewerMenu* ctx, Logger* log) {

    return true;
}

REGISTER_STATIC_PLUGIN(
    LogViewer, 
    Plugin::Type::WIDGET, 
    "0.0.1", 
    "pawel.ciejka@gmail.com", 
    "Log Viewer", 
    LogViewer_register, 
    LogViewer_unregister, 
    LogViewerMenu,
    {},
    false,
    1400
)