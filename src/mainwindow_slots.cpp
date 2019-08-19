#include "mainwindow.h"

#include <QDateTime>
#include <QProcess>
#include <QByteArray>
#include <QPoint>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QIcon>

void MainWindow::connectToHost()
{
    appendLogEdit("服务器连接中...");

    QString host = m_host_edit->text().trimmed();
    quint16 port = static_cast<quint16>(m_port_spinbox->value());
    QString user = m_user_edit->text().trimmed();
    QString password = m_password_edit->text().trimmed();

    m_host_edit->setEnabled(false);
    m_port_spinbox->setEnabled(false);
    m_user_edit->setEnabled(false);
    m_password_edit->setEnabled(false);
    m_connect_btn->setText("断开");

    m_client->setClientId(m_clientid_edit->text().trimmed());
    m_client->setHostName(host);
    m_client->setPort(port);
    m_client->setUsername(user);
    m_client->setPassword(password.toLocal8Bit());
    m_client->setKeepAlive(60);
    m_client->connectToHost();
}

void MainWindow::disconnectToHost()
{
    m_host_edit->setEnabled(true);
    m_port_spinbox->setEnabled(true);
    m_user_edit->setEnabled(true);
    m_password_edit->setEnabled(true);
    m_connect_btn->setText("连接");

    m_client->disconnectFromHost();
}

void MainWindow::appendLogEdit(QString str)
{
    QString current_time = QDateTime::currentDateTime().toString("yyyy.mm.dd hh:mm:ss.zzz");

    m_log_edit->appendPlainText(current_time + " | " + str);
}

void MainWindow::slot_connectBtnClicked()
{
    if (!m_isconnect)
        connectToHost();
    else
        disconnectToHost();
}

void MainWindow::slot_pinBtnClicked()
{
    if(m_host_edit->text() != "")
    {
        this->setEnabled(false);

        QString  remoteIP = m_host_edit->text().trimmed();
        QProcess cmd(this);

        cmd.start("ping " + remoteIP);
        while (cmd.waitForFinished(20) == false)
        {
            QByteArray out = cmd.readAllStandardOutput();
            if (!out.isEmpty())
            {
                m_log_edit->appendPlainText(QString::fromLocal8Bit(out));
                repaint();
            }
        }
        QByteArray out = cmd.readAllStandardOutput();
        m_log_edit->appendPlainText(QString::fromLocal8Bit(out));

        this->setEnabled(true);
    }
    else
    {
        appendLogEdit("请检查参数后重新操作...");
    }
}

void MainWindow::slot_subBtnClicked()
{
    if (m_isconnect)
    {
        m_client->subscribe(m_subtopic_edit->text(), static_cast<quint8>(m_subqos_cbox->currentIndex()));
    }
    else
    {
        appendLogEdit("请连接后重新操作...");
    }
}

void MainWindow::slot_unsubBtnClicked()
{
    m_client->unsubscribe(m_subtopic_edit->text());
}

void MainWindow::slot_publishBtnClicked()
{
    if (m_isconnect)
    {
        const quint16 id         = 100;
        const QString topic      = m_publishtopic_edit->text();
        const QByteArray payload = m_publish_edit->text().toLocal8Bit();
        const quint8 qos         = static_cast<quint8>(m_publishqos_cbox->currentIndex());
        const bool retain        = m_retain_cbox->isChecked();
        const Message message(id, topic, payload, qos, retain);

        m_client->publish(message);

        QString sqos;
        sqos.sprintf("%d", qos);
        appendLogEdit("消息发送 [ " + m_publishtopic_edit->text() + "_" +  sqos + " ] " + payload);
    }
    else
    {
        appendLogEdit("请连接后重新操作...");
    }
}

//----------------------------------------
void MainWindow::slot_connected()
{
    appendLogEdit("服务器连接成功...");
    m_isconnect = true;
}

void MainWindow::slot_disconnected()
{
    appendLogEdit("服务器断开...");
    m_isconnect = false;
    disconnectToHost();
}

void MainWindow::slot_error(const QMQTT::ClientError error)
{
    Q_UNUSED(error);

    appendLogEdit("连接异常，请检查参数后重新操作...");
    disconnectToHost();
    m_isconnect = false;
}

void MainWindow::slot_subscribed(const QString& topic, const quint8 qos)
{
    QString sqos;
    sqos.sprintf("%d", qos);

    appendLogEdit("主题订阅成功 [ " + topic + "_" + sqos + " ] ...");
}

void MainWindow::slot_unsubscribed(const QString& topic)
{
    appendLogEdit("主题取消 [ " + topic + " ] ...");
}

void MainWindow::slot_received(const QMQTT::Message& message)
{
    appendLogEdit("消息接收 [ " + message.topic() + " ] " + message.payload());
}

void MainWindow::slot_customContextMenuRequested(const QPoint &pos)
{
    int x = this->x() + this->width() - m_log_edit->width() + pos.x() - 20;
    int y = this->y() + this->height() - m_log_edit->height() + pos.y() + 10;

    m_menu->exec(QPoint(x, y));
}
void MainWindow::slot_saveLog()
{
    QFileDialog fd(this);

    fd.setWindowTitle("保存");
    fd.setNameFilter("文本文档(*.txt)");
    fd.setWindowIcon(QIcon(":/res/logo.ico"));
    fd.setAcceptMode(QFileDialog::AcceptSave);

    if (fd.exec() == QFileDialog::Accepted)
    {
        QFile file(fd.selectedFiles()[0]);

        if (file.open(QFile::WriteOnly|QFile::Text))
        {
            QTextStream out(&file);

            out << m_log_edit->toPlainText();
            out.flush();

            file.close();
        }
    }
}
