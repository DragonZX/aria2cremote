/*************************************************************************
 *                                                                       *
 * Aria2c Remote Control - UI for Aria2c                                 *
 * Copyright (C) 2010 Zoltan Molnar                                      *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 * Contact : <molnarzoltan@users.sourceforge.net>                        *
 *                                                                       *
 *************************************************************************/
 
#include "aria2options.h"
#include "ui_aria2options.h"
#include <QFile>

Aria2Options::Aria2Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Aria2Options),
    m_host(""),
    m_user(""),
    m_password(""),
    m_port(6800),
    m_proxyServer(""),
    m_proxyUser(""),
    m_proxyPassword(""),
    m_proxyPort(8080)
{
    ui->setupUi(this);

    ui->lineEdit_RemoteHost->setText(m_host);
    ui->lineEdit_UserName->setText(m_user);
    ui->lineEdit_Password->setText(m_password);
    ui->spinBox_Port->setValue(m_port);

    ui->lineEdit_Proxy_Server->setText(m_proxyServer);
    ui->lineEdit_Proxy_UserName->setText(m_proxyUser);
    ui->lineEdit_Proxy_Password->setText(m_proxyPassword);
    ui->spinBox_Proxy_Port->setValue(m_proxyPort);
}

Aria2Options::~Aria2Options()
{
    delete ui;
}

void Aria2Options::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Aria2Options::setConnection(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort)
{
    m_host = host;
    m_user = user;
    m_password = password;
    m_port = port;

    m_proxyServer = proxyServer;
    m_proxyUser = proxyUser;
    m_proxyPassword = proxyPassword;
    m_proxyPort = proxyPort;

    ui->lineEdit_RemoteHost->setText(m_host);
    ui->lineEdit_UserName->setText(m_user);
    ui->lineEdit_Password->setText(m_password);
    ui->spinBox_Port->setValue(m_port);

    ui->lineEdit_Proxy_Server->setText(m_proxyServer);
    ui->lineEdit_Proxy_UserName->setText(m_proxyUser);
    ui->lineEdit_Proxy_Password->setText(m_proxyPassword);
    ui->spinBox_Proxy_Port->setValue(m_proxyPort);

    bool isEnabled = (m_proxyServer.size() > 0) && (m_proxyUser.size() > 0) && (m_proxyPassword.size() > 0);
    ui->checkBox_Enable_Proxy->setCheckState(isEnabled ? (Qt::Checked) :(Qt::Unchecked));

    ui->lineEdit_Proxy_Server->setEnabled(isEnabled);
    ui->lineEdit_Proxy_UserName->setEnabled(isEnabled);
    ui->lineEdit_Proxy_Password->setEnabled(isEnabled);
    ui->spinBox_Proxy_Port->setEnabled(isEnabled);
}

void Aria2Options::on_buttonBox_accepted()
{
    m_host = ui->lineEdit_RemoteHost->text();
    m_user = ui->lineEdit_UserName->text();
    m_password = ui->lineEdit_Password->text();
    m_port = ui->spinBox_Port->value();
    m_proxyServer =ui->lineEdit_Proxy_Server->text();
    m_proxyUser = ui->lineEdit_Proxy_UserName->text();
    m_proxyPassword = ui->lineEdit_Proxy_Password->text();
    m_proxyPort = ui->spinBox_Proxy_Port->value();
}

void Aria2Options::on_checkBox_Enable_Proxy_stateChanged(int value)
{
    bool isEnabled = (value == Qt::Checked);

    ui->lineEdit_Proxy_Server->setEnabled(isEnabled);
    ui->lineEdit_Proxy_UserName->setEnabled(isEnabled);
    ui->lineEdit_Proxy_Password->setEnabled(isEnabled);
    ui->spinBox_Proxy_Port->setEnabled(isEnabled);
}
