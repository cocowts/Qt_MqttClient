#include "mainwindow.h"

#include <QIcon>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    m_qos_list << "0 级" << "1 级" << "2 级";
    m_client = new Client();
    m_isconnect = false;

    initUi();
    initSlots();
}

void MainWindow::initUi()
{
    QVBoxLayout* vlayout = new QVBoxLayout;

    vlayout->addWidget(createConnectionBox());
    vlayout->addWidget(createSubscribeBox());
    vlayout->addWidget(createPublishBox());
    vlayout->addWidget(createLogBox());

    createMenu();

    this->setLayout(vlayout);
    this->setWindowIcon(QIcon(":/res/logo.ico"));
    this->setWindowTitle("MQTT客户端测试工具 - By TianSong");
    this->setMinimumWidth(500);
}

void MainWindow::initSlots()
{
    connect(m_connect_btn, SIGNAL(clicked()), this, SLOT(slot_connectBtnClicked()));
    connect(m_pin_btn, SIGNAL(clicked()), this, SLOT(slot_pinBtnClicked()));
    connect(m_sub_btn, SIGNAL(clicked()), this, SLOT(slot_subBtnClicked()));
    connect(m_unsub_btn, SIGNAL(clicked()), this, SLOT(slot_unsubBtnClicked()));
    connect(m_publish_btn, SIGNAL(clicked()), this, SLOT(slot_publishBtnClicked()));

    connect(m_client, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
    connect(m_client, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(slot_error(const QMQTT::ClientError)));
    connect(m_client, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(slot_subscribed(const QString&, const quint8)));
    connect(m_client, SIGNAL(unsubscribed(const QString&)), this, SLOT(slot_unsubscribed(const QString&)));
    connect(m_client, SIGNAL(received(const QMQTT::Message&)), this, SLOT(slot_received(const QMQTT::Message&)));

    connect(m_log_edit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slot_customContextMenuRequested(const QPoint&)));

    connect(m_select_action, SIGNAL(triggered()), m_log_edit, SLOT(selectAll()));
    connect(m_copy_action, SIGNAL(triggered()), m_log_edit, SLOT(copy()));
    connect(m_clear_action, SIGNAL(triggered()), m_log_edit, SLOT(clear()));
    connect(m_save_action, SIGNAL(triggered()), this, SLOT(slot_saveLog()));
}

QGroupBox* MainWindow::createConnectionBox()
{
    QGroupBox* gbox      = new QGroupBox(this);
    QGridLayout* glayout = new QGridLayout;

    m_host_edit     = new QLineEdit();
    m_user_edit     = new QLineEdit();
    m_password_edit = new QLineEdit();
    m_clientid_edit = new QLineEdit();
    m_port_spinbox  = new QSpinBox();
    m_connect_btn   = new QPushButton("连接");
    m_pin_btn       = new QPushButton("PIN");

    m_host_edit->setMinimumWidth(80);
    m_host_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_user_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_port_spinbox->setSingleStep(1);
    m_port_spinbox->setRange(0, 65535);
    m_port_spinbox->setFixedWidth(80);
    m_port_spinbox->setContextMenuPolicy(Qt::NoContextMenu);
    m_connect_btn->setCursor(Qt::PointingHandCursor);
    m_pin_btn->setCursor(Qt::PointingHandCursor);
    m_password_edit->setFixedWidth(80);
    m_password_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_clientid_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_clientid_edit->setMaxLength(30);

    glayout->addWidget(new QLabel("主机IP："), 0, 0);
    glayout->addWidget(m_host_edit, 0, 1);
    glayout->addWidget(new QLabel("端口号："), 0, 2);
    glayout->addWidget(m_port_spinbox, 0, 3);
    glayout->addWidget(new QLabel, 0, 4);
    glayout->addWidget(m_connect_btn, 0, 5);

    glayout->addWidget(new QLabel("用户名："), 1, 0);
    glayout->addWidget(m_user_edit, 1, 1);
    glayout->addWidget(new QLabel("密 码："), 1, 2);
    glayout->addWidget(m_password_edit, 1, 3);
    glayout->addWidget(new QLabel, 1, 4);
    glayout->addWidget(m_pin_btn, 1, 5);

    glayout->addWidget(new QLabel("客户端ID:"), 2, 0);
    glayout->addWidget(m_clientid_edit, 2, 1, 1, 3);

    glayout->setMargin(10);
    glayout->setSpacing(10);

    gbox->setLayout(glayout);
    gbox->setTitle("连接参数");
    gbox->setFixedHeight(115);

    return gbox;
}

QGroupBox* MainWindow::createSubscribeBox()
{
    QGroupBox* gbox = new QGroupBox(this);
    QGridLayout* glayout = new QGridLayout;

    m_subtopic_edit = new QLineEdit();
    m_subqos_cbox   = new QComboBox();
    m_sub_btn       = new QPushButton("订阅");
    m_unsub_btn     = new QPushButton("取消订阅");

    m_subtopic_edit->setMinimumWidth(80);
    m_subtopic_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_subqos_cbox->addItems(m_qos_list);
    m_sub_btn->setCursor(Qt::PointingHandCursor);
    m_unsub_btn->setCursor(Qt::PointingHandCursor);
    m_subqos_cbox->setCursor(Qt::PointingHandCursor);

    glayout->addWidget(new QLabel("主  题："), 0, 0);
    glayout->addWidget(m_subtopic_edit, 0, 1);
    glayout->addWidget(new QLabel(), 0, 2);
    glayout->addWidget(m_sub_btn, 0, 3);
    glayout->addWidget(new QLabel("质  量："), 1, 0);
    glayout->addWidget(m_subqos_cbox, 1, 1);
    glayout->addWidget(new QLabel(), 1, 2);
    glayout->addWidget(m_unsub_btn, 1, 3);

    glayout->setMargin(10);
    glayout->setSpacing(10);

    gbox->setLayout(glayout);
    gbox->setTitle("主题订阅");
    gbox->setFixedHeight(85);

    return gbox;
}

QGroupBox* MainWindow::createPublishBox()
{
    QGroupBox* gbox = new QGroupBox(this);
    QGridLayout* glayout = new QGridLayout;

    m_publish_edit      = new QLineEdit();
    m_retain_cbox       = new QCheckBox("遗言");
    m_publishtopic_edit = new QLineEdit();
    m_publishqos_cbox   = new QComboBox();
    m_publish_btn       = new QPushButton("发布");

    m_publish_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_publishtopic_edit->setContextMenuPolicy(Qt::NoContextMenu);
    m_publish_btn->setCursor(Qt::PointingHandCursor);
    m_retain_cbox->setCursor(Qt::PointingHandCursor);
    m_publishqos_cbox->setCursor(Qt::PointingHandCursor);
    m_publishqos_cbox->addItems(m_qos_list);

    glayout->addWidget(new QLabel("消  息："), 0, 0);
    glayout->addWidget(m_publish_edit, 0, 1, 1, 3);
    glayout->addWidget(new QLabel(), 1, 4);
    glayout->addWidget(m_retain_cbox, 0, 5);

    glayout->addWidget(new QLabel("主  题："), 1, 0);
    glayout->addWidget(m_publishtopic_edit, 1, 1);
    glayout->addWidget(new QLabel("质量："), 1, 2);
    glayout->addWidget(m_publishqos_cbox, 1, 3);
    glayout->addWidget(new QLabel(), 1, 4);
    glayout->addWidget(m_publish_btn, 1, 5);

    glayout->setMargin(10);
    glayout->setSpacing(10);

    gbox->setLayout(glayout);
    gbox->setTitle("消息发布");
    gbox->setFixedHeight(85);

    return gbox;
}

QGroupBox* MainWindow::createLogBox()
{
    QGroupBox* gbox = new QGroupBox(this);
    QVBoxLayout* vlayout = new QVBoxLayout;
    m_log_edit = new QPlainTextEdit();

    m_log_edit->setContextMenuPolicy(Qt::CustomContextMenu);
    m_log_edit->setReadOnly(true);

    m_log_edit->appendPlainText("[ 个人小站： https://segmentfault.com/u/tiansong/about ]\n");

    vlayout->addWidget(m_log_edit);

    gbox->setLayout(vlayout);
    gbox->setTitle("消息发布");

    return gbox;
}

void MainWindow::createMenu()
{
    m_menu = new QMenu(this);
    m_select_action = new QAction("全选", m_menu);
    m_copy_action   = new QAction("复制", m_menu);
    m_clear_action  = new QAction("清空", m_menu);
    m_save_action   = new QAction("保存", m_menu);

    m_menu->addAction(m_select_action);
    m_menu->addAction(m_copy_action);
    m_menu->addSeparator();
    m_menu->addAction(m_clear_action);
    m_menu->addSeparator();
    m_menu->addAction(m_save_action);
}

MainWindow::~MainWindow()
{

}
