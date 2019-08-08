#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mqtt/qmqtt.h"

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QGridLayout>

using namespace QMQTT;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    

protected:
    Client*     m_client;
    QStringList m_qos_list;
    bool        m_isconnect;

    // 连接 groupBox
    QLineEdit*   m_host_edit;
    QLineEdit*   m_user_edit;
    QLineEdit*   m_password_edit;
    QLineEdit*   m_clientid_edit;
    QSpinBox*    m_port_spinbox;
    QPushButton* m_connect_btn;
    QPushButton* m_pin_btn;

    // 订阅 groupBox
    QLineEdit*   m_subtopic_edit;
    QComboBox*   m_subqos_cbox;
    QPushButton* m_sub_btn;
    QPushButton* m_unsub_btn;

    // 发布 groupBox
    QLineEdit*   m_publish_edit;
    QCheckBox*   m_retain_cbox;
    QLineEdit*   m_publishtopic_edit;
    QComboBox*   m_publishqos_cbox;
    QPushButton* m_publish_btn;

    // 日志 groupBox
    QPlainTextEdit* m_log_edit;

    // menu
    QMenu*          m_menu;
    QAction*        m_select_action;
    QAction*        m_copy_action;
    QAction*        m_clear_action;
    QAction*        m_save_action;

protected:
    void initUi();
    void initSlots();

    QGroupBox* createConnectionBox();
    QGroupBox* createSubscribeBox();
    QGroupBox* createPublishBox();
    QGroupBox* createLogBox();
    void       createMenu();

    void connectToHost();
    void disconnectToHost();
    void appendLogEdit(QString str);

protected slots:
    void slot_connectBtnClicked();
    void slot_pinBtnClicked();
    void slot_subBtnClicked();
    void slot_unsubBtnClicked();
    void slot_publishBtnClicked();

    void slot_connected();
    void slot_disconnected();
    void slot_error(const QMQTT::ClientError error);
    void slot_subscribed(const QString& topic, const quint8 qos = 0);
    void slot_unsubscribed(const QString& topic);
    void slot_received(const QMQTT::Message& message);

    void slot_customContextMenuRequested(const QPoint &pos);

    void slot_saveLog();
};

#endif // MAINWINDOW_H
