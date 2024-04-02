#ifndef QLINBUS_H
#define QLINBUS_H

#include <QTimer>
#include <QStandardItemModel>
#include <QScrollBar>

#include "qlinbus_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_qlinbus.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinBusUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QLINBUS_EXPORT QLinBus : public Widget {
    Q_OBJECT

public:
    QLinBus(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

public slots:
    void settingsChanged();

    void init() {
        auto plugin = plugins()->instance("QLin", 0);
        auto lin = plugin.dynamicCast<IODevice>();
        m_lin = lin;
        connect(lin.data(), &IODevice::dataReady, this, &QLinBus::dataReady);
        if (lin->isOpen() == false) {
            lin->open();
        }
    }

    void scanStep() {
        if (m_scan == m_settings.scanStartID) {
            m_model->removeRows(0, m_model->rowCount());
        }

        if (m_scan <= m_settings.scanStopID) {
            QByteArray data(1, static_cast<char>(m_scan));
            m_lin->write(data);

            QList<QStandardItem*> rows;
            rows << new QStandardItem("0x"+QString::number(m_scan, 16)) << new QStandardItem("REQUEST") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem("-");
            m_model->appendRow(rows);

            m_ui->scanProgress->setValue(m_scan);
            ++m_scan;
            QTimer::singleShot(0, this, &QLinBus::scanStep);
        } else {
            m_scan = m_settings.scanStartID;
        }
    }

    void dataReady(const QByteArray& data) {
        emit message("QLinBus::dataReady(): "+data);

        if (data.startsWith("LIN NOANS")) {
            QByteArrayList list = data.split(',');
            for (auto& item : list) {
                item = item.mid(item.indexOf(':') + 1);
            }
            QList<QStandardItem*> rows;
            rows << new QStandardItem(list[0]) << new QStandardItem("NOANS") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem(list[2]);
            m_model->appendRow(rows);
        }else if(data.startsWith("ID")) {
            QByteArrayList list = data.split(',');
            for (auto& item : list) {
                item = item.mid(item.indexOf(':')+1);
            }
            QList<QStandardItem*> rows;
            rows << new QStandardItem(list[0]) << new QStandardItem("ANS") << new QStandardItem(list[1].removeIf([](QChar ch) { return ch == '\'';  })) << new QStandardItem(list[2]) << new QStandardItem(list[3]);
            m_model->appendRow(rows);
        }

        m_ui->scanTable->verticalScrollBar()->setSliderPosition(m_ui->scanTable->verticalScrollBar()->maximum());
    }

private:
    SettingsDialog::LinBusSettings m_settings;
    Ui::QLinBusUI* m_ui = nullptr;
    uint16_t m_scan = 0;
    QSharedPointer<IODevice> m_lin;
    QStandardItemModel* m_model = nullptr;
};

#endif