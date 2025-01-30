#include "QDataTable.h"
#include <QTranslator>
#include <QJsonObject>
#include "../core/core.h"
#include "ui_qdatatable.h"

struct QDataTableMenu {
    QDataTableMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QDataTable", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_dataTableMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QDataTable_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QDataTableMenu* ctx, Logger* log) {
    log->message("QDataTable_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QDataTable_register(): application is non gui not registering");
        return false;
    }

    auto visualMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));

    if (visualMenu == nullptr) {
        log->message("QDataTable_register(): visualizations menu not found");
        return false;
    }

    ctx->m_dataTableMenu = visualMenu->addMenu(ctx->m_app->translate("MainWindow", "DataTable"));

    windowAddInstanceSettings(ctx->m_dataTableMenu, &ctx->m_settings, &ctx->m_newInstance, "QDataTable", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QDataTable, QDataTableMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QDataTable"), gtx, ctx, log);
    });

    return true;
}

static bool QDataTable_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QDataTableMenu* ctx, Logger* log) {
    log->message("QDataTable_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QDataTable,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QDataTable_register,
    QDataTable_unregister,
    QDataTableMenu,
    { "QSerial" },
    true,
    1100,
    DataTableSettings
)

union ColumnVariable {
    int64_t integer;
    float floating;
    double doublep;
};

QDataTable::QDataTable(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, DataTableSettings* set, const QString& uuid) 
    : Widget(ld, plugins, parent, settingsPath, set, uuid) 
    , m_data(new Ui::QDataTableUI){
    m_data.m_ui->setupUi(this);
    
    m_data.m_ui->tableView->setShowGrid(true);
    m_data.m_ui->tableView->setAlternatingRowColors(true);
    m_data.m_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_data.m_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_data.m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_data.m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
}

SettingsMdi* QDataTable::settingsWindow() const {
    emit message("QDataTable::settingsWindow()");
    auto ret = new SettingsDialog(nullptr, this);
    connect(ret, &SettingsDialog::settingsUpdated, this, &QDataTable::settingsChanged);
    return ret;
}

Q_INVOKABLE bool QDataTable::reset(Reset type) {
    emit message("QData::reset(Reset type)");
    return true;
}

bool QDataTable::initialize() {
    emit message("QDataTable::initialize");

    auto setts = settings<DataTableSettings>();
    auto columns = setts->columns();
    QRegularExpression rx("((signed|unsigned)[ ]+)?(int|float|double)(8|16|32|64)?");
    
    m_data.m_columns.clear();
    m_data.m_model = new QStandardItemModel(0, columns.size()+1);
    int col = 0;
    m_data.m_model->setHeaderData(col++, Qt::Horizontal, tr("time"));
    

    for (auto begin = columns.begin(), end = columns.end(); begin != end; ++begin) {
        if (begin->isObject() == false) {
            emit message(QString("QDataTable::initialize: invalid column data: %1").arg(begin->toString()));
            continue;
        }
        const auto column = begin->toObject();
        auto match = rx.match(column["type"].toString());

        if (match.hasMatch() == false) {
            emit message(QString("QDataTable::initialize: invalid type %1").arg(column["type"].toString()));
            continue;
        }

        auto sign = match.captured(1);
        auto type = match.captured(3);
        auto len = match.captured(4);

        emit message(QString("QDataTable::initialize: found sign: %1, type: %2, len: %3").arg(sign).arg(type).arg(len));

        m_data.m_columns << ColumnData(column["name"].toString(), sign, type, len);

        emit message(QString("QDataTable::initialize: added column: %1").arg(m_data.m_columns.last()));

        m_data.m_model->setHeaderData(col++, Qt::Horizontal, column["name"].toString());
    }

    m_data.m_ui->tableView->setModel(m_data.m_model);

    auto io = plugins()->instance(setts->input(), nullptr);
    if (io.isNull()) {
        emit message(QString("QDataTable::initialize: port %1 not found"));
        return false;
    }
    QObject::connect(dynamic_cast<IODevice*>(io.data()), &IODevice::dataReady, this, &QDataTable::dataReady);
    auto port = io.dynamicCast<IODevice>();
    if (port->isOpen() == false) {
        if (port->open() == false) {
            emit message(QString("QDataTable::initialize: failed to open port: %1").arg(setts->input()));
            return false;
        }

        emit message(QString("QDataTable::initialize: opened port: %1").arg(setts->input()));
    }

    return true;
}

bool QDataTable::deinitialize() {
    emit message("QDataTable::deinitialize()");
    m_data.m_ui->tableView->setModel(nullptr);
    m_data.m_buffer.clear();

    if (m_data.m_model) {
        delete m_data.m_model;
    }
    return true;
}

void QDataTable::settingsChanged() {
    emit message("QDataTable::settingsChanged()");
    const auto set = settings<DataTableSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<DataTableSettings>();
    *set = *nset;

    deinitialize();
    initialize();

    emit settingsApplied();
}

void QDataTable::dataReady(const QByteArray& data) {
    emit message("QDataTable::dataReady(const QByteArray& data)");

    emit message(QString("QDataTable::dataReady: recv data size: %1").arg(data.size()));
    const auto set = settings<DataTableSettings>();
    
    switch (set->mode()) {
    case DataTableSettings::DataTableMode::BINARY:
        handleBinaryData(data, set);
        break;
    case DataTableSettings::DataTableMode::TEXT:
        handleAsciiData(data, set);
        break;
    
    default:
        emit message(QString("QDataTable::dataReady: invalid mode selected: %1").arg(static_cast<int>(set->mode())));
    }
}

void QDataTable::handleBinaryData(const QByteArray& data, const DataTableSettings* set) {
    auto len = columnsLength(m_data.m_columns);

    m_data.m_buffer += data;

    if (m_data.m_buffer.size() < len) {
        emit message(QString("QDataTable::dataReady: data in buffer has size: %1 but columns length is: %2 waiting for more data").arg(m_data.m_buffer.size()).arg(len));
        return;
    }

    while (m_data.m_buffer.size() >= len) {
        auto idx = m_data.m_buffer.indexOf(set->magic());
        if (idx == -1) {
            emit message("QDataTable::dataReady: magic not found in buffer");
            return;
        }
        else {
            m_data.m_buffer.remove(0, idx + set->magic().size());
            emit message(QString("QDataTable::dataReady: removing first %1 characters from buffer").arg(idx));
        }
        auto current = m_data.m_buffer.first(len);

        if (current.size() != len) {
            emit message("QDataTable::dataReady: current.size() != len");
            return;
        }

        m_data.m_buffer.remove(0, len);
        QList<QStandardItem*> columns;
        QString text;
        ColumnVariable var;
        const char* data = current.data();

        columns << new QStandardItem(QString::number(QDateTime::currentSecsSinceEpoch()));

        for (auto begin = m_data.m_columns.begin(), end = m_data.m_columns.end(); begin != end; ++begin) {
            memset(&var, 0, sizeof(var));
            switch (begin->m_type) {
            case VariableType::INTEGER:
                memcpy(&var.integer, data, begin->m_length);
                text = QString::number(var.integer);
                break;
            case VariableType::FLOAT:
                memcpy(&var.floating, data, begin->m_length);
                text = QString::number(var.floating);
                break;
            case VariableType::DOUBLE:
                memcpy(&var.doublep, data, begin->m_length);
                text = QString::number(var.doublep);
                break;
            }
            data += begin->m_length;
            columns << new QStandardItem(text);
        }
        m_data.m_model->appendRow(columns);
        if (m_data.m_model->rowCount() > set->maxRows()) {
            m_data.m_model->removeRow(0);
        }
    }
}

void QDataTable::handleAsciiData(const QByteArray& data, const DataTableSettings* set) {
    m_data.m_buffer += data;
    const auto rx = columnsToRegExp(m_data.m_columns, set);
    QRegularExpressionMatch match;
    const auto newLine = QString(QByteArray::fromPercentEncoding(set->newLine().toLocal8Bit(), '\\'));

    while((match = rx.match(m_data.m_buffer)).hasMatch()) {
        const auto txts = match.capturedTexts();
        for (auto begin = txts.begin(), end = txts.end(); begin != end; ++begin) {
            const auto str = begin->mid(0, begin->size()-newLine.size());
            const auto fields = begin->split(set->separator());

            QList<QStandardItem*> cols;
            cols << new QStandardItem(QString::number(QDateTime::currentSecsSinceEpoch()));
            for (auto it = fields.begin(), stop = fields.end(); it != stop; ++it) {
                cols << new QStandardItem(*it);
            }
            m_data.m_model->appendRow(cols);
            if (m_data.m_model->rowCount() > set->maxRows()) {
                m_data.m_model->removeRow(0);
            }
        }
        m_data.m_buffer.remove(0, match.captured().size());
    }
}

QRegularExpression QDataTable::columnsToRegExp(const QListColumns& columns, const DataTableSettings* set) const {
    emit message("QDataTable::columnsToRegExp(const QListColumns& columns) const");
    QString ret;
    int i = 0;
    const auto newLine = QString(QByteArray::fromPercentEncoding(set->newLine().toLocal8Bit(), '\\'));

    for (auto begin = columns.begin(), end = columns.end(); begin != end; ++begin) {
        switch (begin->m_type) {
        case VariableType::DOUBLE:
            emit message("QDataTable::columnsToRegExp: found double");
            [[fallthrough]];
        case VariableType::FLOAT:
            emit message("QDataTable::columnsToRegExp: found float");
            ret += "[+-]?([0-9]*[.])?[0-9]+";
            break;
        case VariableType::INTEGER:
            ret += "[+-]?[1-9][0-9]*";
            break;
        default:
            emit message("QDataTable::columnsToRegExp: invalid type");
            continue;
        }

        if (++i < columns.size()) {
            ret += set->separator();
        }
    }

    ret += newLine;

    return QRegularExpression(ret);
}

int QDataTable::columnsLength(const QListColumns& columns) const {
    emit message("QDataTable::columnsLength(const QListColumns& columns) const");
    int ret = 0;
    for (auto begin = columns.begin(), end = columns.end(); begin != end; ++begin) {
        ret += begin->m_length;
    }
    return ret;
}