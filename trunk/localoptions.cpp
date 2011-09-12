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

#include "localoptions.h"
#include "ui_localoptions.h"
#include <QMessageBox>
#include <QFile>
#include <QUrl>
#include "util.h"
#include "addserver.h"

using namespace util;
extern quint32 g_uiAria2cVersion;

LocalOptions::LocalOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalOptions)
{
    ui->setupUi(this);

    m_currentServer[0] =
    m_currentServer[1] =
    m_currentServer[2] =
    m_currentServer[3] =
    m_currentServer[4] = -1;

    m_servers.LoadServerList();


    foreach (SERVER_ITEM si, m_servers.GetServers())
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setCheckState(0, Qt::Unchecked);
        item->setText(0, si.server);
        item->setText(1, si.user);
        if (si.type != SERVER_HTTP_FTP)
        {
            item->setText(2, QString::number(si.port));
        }
        if (si.type == SERVER_ALL_PROXY)
              ui->treeWidget_Proxy_All_List->addTopLevelItem(item);
        else if (si.type == SERVER_HTTP_PROXY)
             ui->treeWidget_Proxy_HTTP_List->addTopLevelItem(item);
        else if (si.type == SERVER_HTTPS_PROXY)
             ui->treeWidget_Proxy_HTTPS_List->addTopLevelItem(item);
        else if (si.type == SERVER_FTP_PROXY)
             ui->treeWidget_Proxy_FTP_List->addTopLevelItem(item);
        else if (si.type == SERVER_HTTP_FTP)
             ui->treeWidget_Server_List->addTopLevelItem(item);
        else delete item;
    }

    ui->pushButton_Servers_Delete->setEnabled(false);
    ui->pushButton_Servers_Modify->setEnabled(false);

    ui->checkBox_Proxy_All_Enabled->setCheckState(Qt::Unchecked);
    ui->pushButton_Proxy_Add->setEnabled(false);
    ui->pushButton_Proxy_Delete->setEnabled(false);
    ui->pushButton_Proxy_Modify->setEnabled(false);

    ui->treeWidget_Proxy_All_List->setEnabled(false);

    ui->treeWidget_Server_List->setColumnWidth(0, 280);
}

LocalOptions::~LocalOptions()
{
    delete ui;
}

void LocalOptions::changeEvent(QEvent *e)
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

template <class T> T LocalOptions::SetProperties(T widget, quint32 uiMinAria2cVersion)
{
    if (uiMinAria2cVersion > g_uiAria2cVersion)
    {
        QString tooltip(tr("Minimum Aria2c version: <b>%1.%2.%3</b>").arg(uiMinAria2cVersion >> 16).arg((uiMinAria2cVersion & 0x00FF00) >> 8).arg(uiMinAria2cVersion & 0xFF));

        widget->setToolTip(tooltip);
        widget->setEnabled(false);
    }
    return widget;
}

//proxy
void LocalOptions::on_checkBox_Proxy_All_Enabled_stateChanged(int state)
{
    bool isEnabled = (state == Qt::Checked);

    //set tab page state
    ui->tab_Proxy_HTTP->setEnabled(!isEnabled);
    ui->tab_Proxy_HTTPS->setEnabled(!isEnabled);
    ui->tab_Proxy_FTP->setEnabled(!isEnabled);

    ui->pushButton_Proxy_Add->setEnabled(isEnabled);
    ui->pushButton_Proxy_Delete->setEnabled(isEnabled);
    ui->pushButton_Proxy_Modify->setEnabled(isEnabled);

    ui->treeWidget_Proxy_All_List->setEnabled(isEnabled);
}

void LocalOptions::on_tabWidget_Proxy_currentChanged(int index)
{
    bool isEnabled = ui->checkBox_Proxy_All_Enabled->checkState() == Qt::Checked;
    bool isEnabledTabNum = false;
    switch (index)
    {
    case 0:
        isEnabledTabNum = (ui->treeWidget_Proxy_All_List->topLevelItemCount() > 0);
        break;
    case 1:
        isEnabledTabNum = (ui->treeWidget_Proxy_HTTP_List->topLevelItemCount() > 0);
        break;
    case 2:
        isEnabledTabNum = (ui->treeWidget_Proxy_HTTPS_List->topLevelItemCount() > 0);
        break;
    case 3:
        isEnabledTabNum = (ui->treeWidget_Proxy_FTP_List->topLevelItemCount() > 0);
        break;
    }

    ui->pushButton_Proxy_Add->setEnabled( (index == 0) ? (isEnabled) : (!isEnabled) );
    ui->pushButton_Proxy_Delete->setEnabled( (m_currentServer[index] != -1) && isEnabledTabNum && ( (index == 0) ? (isEnabled) : (!isEnabled) ) );
    ui->pushButton_Proxy_Modify->setEnabled( (m_currentServer[index] != -1) && isEnabledTabNum && ( (index == 0) ? (isEnabled) : (!isEnabled) ) );
}

void LocalOptions::on_pushButton_Proxy_Add_clicked()
{
    int iTabIndex = ui->tabWidget_Proxy->currentIndex();
    AddServer addserver(this);
    addserver.setVisiblePort(true);
    addserver.setWindowTitle(tr("Add %1 server").arg(Server_Type_String[iTabIndex]));
    if (addserver.exec() == QDialog::Accepted)
    {
        SERVER_ITEM s;

        s.server = addserver.getServer();
        s.user = addserver.getUser();
        s.password = addserver.getPassword();
        s.type = (SERVER_TYPE)iTabIndex;
        s.port = addserver.getPort();
        m_servers.AddServer(s);

        QTreeWidgetItem *item = new QTreeWidgetItem;

        item->setCheckState(0, Qt::Unchecked);
        item->setText(0, s.server);
        item->setText(1, s.user);
        item->setText(2, QString::number(s.port));

        switch (iTabIndex)
        {
        case 0:
            ui->treeWidget_Proxy_All_List->addTopLevelItem(item);
            break;
        case 1:
            ui->treeWidget_Proxy_HTTP_List->addTopLevelItem(item);
            break;
        case 2:
            ui->treeWidget_Proxy_HTTPS_List->addTopLevelItem(item);
            break;
        case 3:
            ui->treeWidget_Proxy_FTP_List->addTopLevelItem(item);
            break;
        default:
            delete item;
            break;
        }

        ui->pushButton_Proxy_Delete->setEnabled(m_currentServer[iTabIndex] != -1);
        ui->pushButton_Proxy_Modify->setEnabled(m_currentServer[iTabIndex] != -1);
    }
}

void LocalOptions::on_pushButton_Proxy_Delete_clicked()
{
    if (QMessageBox::question(this, "Delete", "Really want to delete the server?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        int iTabIndex = ui->tabWidget_Proxy->currentIndex();
        m_servers.DeleteServer((SERVER_TYPE)iTabIndex, m_currentServer[iTabIndex]);

        QTreeWidget *treeWidget = 0;
        switch (iTabIndex)
        {
        case 0:
            treeWidget = ui->treeWidget_Proxy_All_List;
            break;
        case 1:
            treeWidget = ui->treeWidget_Proxy_HTTP_List;
            break;
        case 2:
            treeWidget = ui->treeWidget_Proxy_HTTPS_List;
            break;
        case 3:
            treeWidget = ui->treeWidget_Proxy_FTP_List;
            break;
        }
        if (treeWidget == 0)
            return;

        QTreeWidgetItem *item = treeWidget->takeTopLevelItem(m_currentServer[iTabIndex]);

        if (item != 0)
        {
            delete item;
        }
        m_currentServer[iTabIndex] = 0;
        QTreeWidgetItem *item1 = treeWidget->currentItem();
        if (item1)
        {
            int iIndex = treeWidget->indexOfTopLevelItem(item1);
            if (iIndex != -1)
            {
                m_currentServer[iTabIndex] = iIndex;
            }
        }
        ui->pushButton_Proxy_Delete->setEnabled(m_servers.GetServers((SERVER_TYPE)iTabIndex).size() > 0);
        ui->pushButton_Proxy_Modify->setEnabled(m_servers.GetServers((SERVER_TYPE)iTabIndex).size() > 0);
    }
}

void LocalOptions::on_pushButton_Proxy_Modify_clicked()
{
    AddServer addserver(this);
    int iTabIndex = ui->tabWidget_Proxy->currentIndex();
    SERVER_ITEM s = m_servers.GetServers((SERVER_TYPE)iTabIndex)[m_currentServer[iTabIndex]];

    addserver.setServer(s.server);
    addserver.setUser(s.user);
    addserver.setPassword(s.password);
    addserver.setPort(s.port);
    addserver.setVisiblePort(true);
    addserver.setWindowTitle(tr("Modify %1 server").arg(Server_Type_String[iTabIndex]));
    if (addserver.exec() == QDialog::Accepted)
    {
        s.server = addserver.getServer();
        s.user = addserver.getUser();
        s.password = addserver.getPassword();
        s.port = addserver.getPort();
        m_servers.ModifyServer((SERVER_TYPE)iTabIndex, m_currentServer[iTabIndex], s);

        QTreeWidgetItem *item = 0;
        switch (iTabIndex)
        {
        case 0:
            item = ui->treeWidget_Proxy_All_List->currentItem();
            break;
        case 1:
            item = ui->treeWidget_Proxy_HTTP_List->currentItem();
            break;
        case 2:
            item = ui->treeWidget_Proxy_HTTPS_List->currentItem();
            break;
        case 3:
            item = ui->treeWidget_Proxy_FTP_List->currentItem();
            break;
        }

        if (item != 0)
        {
            item->setText(0, s.server);
            item->setText(1, s.user);
            item->setText(2, QString::number(s.port));
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_All_List_itemSelectionChanged()
{
    QTreeWidgetItem *item = ui->treeWidget_Proxy_All_List->currentItem();
    if (item)
    {
        int iIndex = ui->treeWidget_Proxy_All_List->indexOfTopLevelItem(item);
        if (iIndex != -1)
        {
            if (m_servers.GetServers(SERVER_ALL_PROXY).size() > iIndex)
            {
                m_currentServer[SERVER_ALL_PROXY] = iIndex;
                ui->pushButton_Proxy_Delete->setEnabled(true);
                ui->pushButton_Proxy_Modify->setEnabled(true);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_HTTP_List_itemSelectionChanged()
{
    QTreeWidgetItem *item = ui->treeWidget_Proxy_HTTP_List->currentItem();
    if (item)
    {
        int iIndex = ui->treeWidget_Proxy_HTTP_List->indexOfTopLevelItem(item);
        if (iIndex != -1)
        {
            if (m_servers.GetServers(SERVER_HTTP_PROXY).size() > iIndex)
            {
                m_currentServer[SERVER_HTTP_PROXY] = iIndex;
                ui->pushButton_Proxy_Delete->setEnabled(true);
                ui->pushButton_Proxy_Modify->setEnabled(true);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_HTTPS_List_itemSelectionChanged()
{
    QTreeWidgetItem *item = ui->treeWidget_Proxy_HTTPS_List->currentItem();
    if (item)
    {
        int iIndex = ui->treeWidget_Proxy_HTTPS_List->indexOfTopLevelItem(item);
        if (iIndex != -1)
        {
            if (m_servers.GetServers(SERVER_HTTPS_PROXY).size() > iIndex)
            {
                m_currentServer[SERVER_HTTPS_PROXY] = iIndex;
                ui->pushButton_Proxy_Delete->setEnabled(true);
                ui->pushButton_Proxy_Modify->setEnabled(true);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_FTP_List_itemSelectionChanged()
{
    QTreeWidgetItem *item = ui->treeWidget_Proxy_FTP_List->currentItem();
    if (item)
    {
        int iIndex = ui->treeWidget_Proxy_FTP_List->indexOfTopLevelItem(item);
        if (iIndex != -1)
        {
            if (m_servers.GetServers(SERVER_FTP_PROXY).size() > iIndex)
            {
                m_currentServer[SERVER_FTP_PROXY] = iIndex;
                ui->pushButton_Proxy_Delete->setEnabled(true);
                ui->pushButton_Proxy_Modify->setEnabled(true);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_All_List_itemChanged(QTreeWidgetItem* item, int column)
{
    if (item->checkState(0) == Qt::Checked)
    {
        int iSize = ui->treeWidget_Proxy_All_List->topLevelItemCount();
        for (int i = 0; i < iSize; ++i)
        {
            QTreeWidgetItem *currentItem = ui->treeWidget_Proxy_All_List->topLevelItem(i);
            if ( (item != currentItem) && (currentItem->checkState(0) == Qt::Checked))
            {
                currentItem->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_HTTP_List_itemChanged(QTreeWidgetItem* item, int column)
{
    if (item->checkState(0) == Qt::Checked)
    {
        int iSize = ui->treeWidget_Proxy_HTTP_List->topLevelItemCount();
        for (int i = 0; i < iSize; ++i)
        {
            QTreeWidgetItem *currentItem = ui->treeWidget_Proxy_HTTP_List->topLevelItem(i);
            if ( (item != currentItem) && (currentItem->checkState(0) == Qt::Checked))
            {
                currentItem->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_HTTPS_List_itemChanged(QTreeWidgetItem* item, int column)
{
    if (item->checkState(0) == Qt::Checked)
    {
        int iSize = ui->treeWidget_Proxy_HTTPS_List->topLevelItemCount();
        for (int i = 0; i < iSize; ++i)
        {
            QTreeWidgetItem *currentItem = ui->treeWidget_Proxy_HTTPS_List->topLevelItem(i);
            if ( (item != currentItem) && (currentItem->checkState(0) == Qt::Checked))
            {
                currentItem->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

void LocalOptions::on_treeWidget_Proxy_FTP_List_itemChanged(QTreeWidgetItem* item, int column)
{
    if (item->checkState(0) == Qt::Checked)
    {
        int iSize = ui->treeWidget_Proxy_FTP_List->topLevelItemCount();
        for (int i = 0; i < iSize; ++i)
        {
            QTreeWidgetItem *currentItem = ui->treeWidget_Proxy_FTP_List->topLevelItem(i);
            if ( (item != currentItem) && (currentItem->checkState(0) == Qt::Checked))
            {
                currentItem->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

//server functions
void LocalOptions::on_treeWidget_Server_List_itemSelectionChanged()
{
    QTreeWidgetItem *item = ui->treeWidget_Server_List->currentItem();
    if (item)
    {
        int iIndex = ui->treeWidget_Server_List->indexOfTopLevelItem(item);
        if (iIndex != -1)
        {
            if (m_servers.GetServers(SERVER_HTTP_FTP).size() > iIndex)
            {
                m_currentServer[SERVER_HTTP_FTP] = iIndex;
                ui->pushButton_Servers_Delete->setEnabled(true);
                ui->pushButton_Servers_Modify->setEnabled(true);
            }
        }
    }
}

void LocalOptions::on_pushButton_Servers_Add_clicked()
{
    AddServer addserver(this);
    addserver.setVisiblePort(false);

    if (addserver.exec() == QDialog::Accepted)
    {
        SERVER_ITEM s;

        s.server = addserver.getServer();
        s.user = addserver.getUser();
        s.password = addserver.getPassword();
        s.type = SERVER_HTTP_FTP;
        s.port = 0;
        m_servers.AddServer(s);

        QTreeWidgetItem *item = new QTreeWidgetItem;

        item->setCheckState(0, Qt::Unchecked);
        item->setText(0, s.server);
        item->setText(1, s.user);

        ui->treeWidget_Server_List->addTopLevelItem(item);
        ui->pushButton_Servers_Delete->setEnabled(m_currentServer[SERVER_HTTP_FTP] != -1);
        ui->pushButton_Servers_Modify->setEnabled(m_currentServer[SERVER_HTTP_FTP] != -1);
    }
}

void LocalOptions::on_pushButton_Servers_Delete_clicked()
{
    if (QMessageBox::question(this, "Delete", "Really want to delete the server?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        m_servers.DeleteServer(SERVER_HTTP_FTP, m_currentServer[SERVER_HTTP_FTP]);
        QTreeWidgetItem *item = ui->treeWidget_Server_List->takeTopLevelItem(m_currentServer[SERVER_HTTP_FTP]);
        if (item != 0)
        {
            delete item;
        }
        m_currentServer[SERVER_HTTP_FTP] = 0;
        QTreeWidgetItem * item1 = ui->treeWidget_Server_List->currentItem();
        if (item1)
        {
            int iIndex = ui->treeWidget_Server_List->indexOfTopLevelItem(item1);
            if (iIndex != -1)
            {
                m_currentServer[SERVER_HTTP_FTP] = iIndex;
            }
        }
        ui->pushButton_Servers_Delete->setEnabled(m_servers.GetServers(SERVER_HTTP_FTP).size() > 0);
        ui->pushButton_Servers_Modify->setEnabled(m_servers.GetServers(SERVER_HTTP_FTP).size() > 0);
    }
}

void LocalOptions::on_pushButton_Servers_Modify_clicked()
{
    AddServer addserver(this);
    SERVER_ITEM s = m_servers.GetServers(SERVER_HTTP_FTP)[m_currentServer[SERVER_HTTP_FTP]];

    addserver.setServer(s.server);
    addserver.setUser(s.user);
    addserver.setPassword(s.password);
    addserver.setPort(s.port);
    addserver.setVisiblePort(false);
    addserver.setServerList(m_servers.GetServers(SERVER_HTTP_FTP));
    addserver.setWindowTitle(tr("Modify %1 server").arg(Server_Type_String[4]));

    if (addserver.exec() == QDialog::Accepted)
    {
        s.server = addserver.getServer();
        s.user = addserver.getUser();
        s.password = addserver.getPassword();
        m_servers.ModifyServer(SERVER_HTTP_FTP, m_currentServer[SERVER_HTTP_FTP], s);

        QTreeWidgetItem *item = ui->treeWidget_Server_List->currentItem();

        item->setText(0, s.server);
        item->setText(1, s.user);
    }
}

void LocalOptions::on_treeWidget_Server_List_itemChanged(QTreeWidgetItem* item, int column)
{
    if (item->checkState(0) == Qt::Checked)
    {
        int iSize = ui->treeWidget_Server_List->topLevelItemCount();
        for (int i = 0; i < iSize; ++i)
        {
            QTreeWidgetItem *currentItem = ui->treeWidget_Server_List->topLevelItem(i);
            if ( (item != currentItem) && (currentItem->checkState(0) == Qt::Checked))
            {
                currentItem->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}

void LocalOptions::SetServerOptions()
{
    if (m_download.getType() == HTTP_FTP)
    {
        QUrl sURI(m_download.getURI());

        QList<SERVER_ITEM> list = m_servers.GetServers(SERVER_HTTP_FTP);
        int iSize = list.size();
        for (int i = 0; i < iSize; ++i)
        {
            QUrl serverUrl(list[i].server);

            if ( (sURI.scheme().size() != 0) && (serverUrl.host().size() != 0) && (sURI.scheme() == serverUrl.scheme()) && (sURI.host() == serverUrl.host()) )
            {
                QTreeWidgetItem *item = ui->treeWidget_Server_List->topLevelItem(i);
                if (item != 0)
                {
                    item->setCheckState(0, Qt::Checked);
                }
                break;
            }
        }
    }
}

//
void LocalOptions::on_spinBox_FTP_ConnectTimeout_valueChanged(int value)
{
    ui->spinBox_ConnectTimeout->setValue(value);
}

void LocalOptions::on_spinBox_ConnectTimeout_valueChanged(int value)
{
    ui->spinBox_FTP_ConnectTimeout->setValue(value);
}

//Bittorrent Prioritize piece functions
void LocalOptions::on_checkBox_PrioritizePieceHead_stateChanged(int value)
{
    ui->spinBox_PrioritizePieceHead_Size->setEnabled(Qt::Checked == value);
}

void LocalOptions::on_checkBox_PrioritizePieceTail_stateChanged(int value)
{
    ui->spinBox_PrioritizePieceTail_Size->setEnabled(Qt::Checked == value);
}

void LocalOptions::on_spinBox_PrioritizePieceHead_Size_valueChanged(int iValue)
{
    if (iValue == 0)
    {
        ui->spinBox_PrioritizePieceHead_Size->setSpecialValueText(tr("default"));
    }
}

void LocalOptions::on_spinBox_SeedTime_valueChanged(int value)
{
    if (value == -1)
    {
        ui->spinBox_SeedTime->setSpecialValueText(tr("undefinied"));
    }
}

//Local settings
void LocalOptions::getUpdate(QString name, QString value)
{
    m_localOptions.remove(name);
    if (m_globalOptions.value(name, "").toString() != value)
    {
        m_localOptions[name] = value;
    }
}

//Local settings
Variant LocalOptions::Update(QString name, Variant default_value)
{
    Variant Ret = m_localOptions.value(name, Variant());
    if (Ret == Variant())
    {
        Ret = m_globalOptions.value(name, default_value);
    }
    return Ret;
}

void LocalOptions::setOptions(QMap<QString, Variant> globaloptions, Download &d/*QMap<QString, Variant> localoptions, XML_TYPE type*/)
{
    m_download = d;
    XML_TYPE type = m_download.getType();
    m_globalOptions = globaloptions;
    m_localOptions = m_download.getCurrentParam();
    SetBasicOptions();
    SetHTTPOptions();
    SetFTPOptions();
    SetBittorrentOptions();
    SetMetalinkOptions();
    SetAdvancedOptions();
    SetServerOptions();

    ui->tab_Basic->setEnabled(true);
    ui->tab_FTP->setEnabled(type == HTTP_FTP);
    ui->tab_HTTP->setEnabled(type == HTTP_FTP);
    ui->tab_Bittorrent->setEnabled( (type == TORRENT) || (type == MAGNETLINK) );
    ui->tab_Metalink->setEnabled(type == METALINK);
    ui->tab_Advanced->setEnabled(true);
    ui->tab_Proxy->setEnabled(true);
    ui->tab_Server->setEnabled(type == HTTP_FTP);
}

void LocalOptions::SetBasicOptions()
{
    SetProperties(ui->lineEdit_Dir)->setText(Update("dir", QString("")).toString());
    SetProperties(ui->spinBox_Split)->setValue(Update("split", 5).toInt());
    SetProperties(ui->spinBox_MaxTries)->setValue(Update("max-tries", 5).toInt());
    SetProperties(ui->spinBox_Timeout)->setValue(Update("timeout", 60).toInt());

    SetProperties(ui->spinBox_LowestSpeedLimit)->setValue(Update(QString("lowest-speed-limit"), 0).toInt() / 1024);
    SetProperties(ui->spinBox_MaxDownloadLimit)->setValue(Update("max-download-limit", 0).toInt() / 1024);
    SetProperties(ui->spinBox_MaxFileNotFound)->setValue(Update("max-file-not-found", 0).toInt());
    SetProperties(ui->spinBox_MaxResumeFailureTries, util::ARIA2C_VERSION_191)->setValue(Update("max-resume-failure-tries", 0).toInt());
    SetProperties(ui->spinBox_MinSplitSize, util::ARIA2C_VERSION_1100)->setValue(Update("min-split-size", 20971520).toInt() / 1024);
    SetProperties(ui->spinBox_MaxConnectionPerServer, util::ARIA2C_VERSION_1100)->setValue(Update("max-connection-per-server", 1).toInt());
    SetProperties(ui->checkBox_ContinueDownload)->setCheckState(Update("continue", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
}

void LocalOptions::SetFTPOptions()
{
    SetProperties(ui->radioButton_Mode_Passive)->setChecked(Update("ftp-pasv", true).toBool());
    SetProperties(ui->radioButton_Mode_Active)->setChecked(!Update("ftp-pasv", true).toBool());
    SetProperties(ui->spinBox_FTP_ConnectTimeout)->setValue(Update("connect-timeout", 60).toInt());
    SetProperties(ui->checkBox_ReuseConnection)->setCheckState(Update("ftp-reuse-connection", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->radioButton_Type_Binary)->setChecked(Update("ftp-type", QString("binary")).toString() == QString("binary"));
    SetProperties(ui->radioButton_Type_ASCII)->setChecked(Update("ftp-type", QString("binary")).toString() == QString("ascii"));
}

void LocalOptions::SetHTTPOptions()
{
    SetProperties(ui->checkBox_HTTPKeepAlive)->setCheckState(Update("enable-http-keep-alive", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_HTTPPipelining)->setCheckState(Update("enable-http-pipelining", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AcceptGzip, util::ARIA2C_VERSION_191)->setCheckState(Update("http-accept-gzip", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AuthChallenge)->setCheckState(Update("http-auth-challenge", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_HTTPNoCache)->setCheckState(Update("http-no-cache", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_HTTPConditionalGet, util::ARIA2C_VERSION_1100)->setCheckState(Update("conditional-get", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->lineEdit_UserAgent)->setText(Update("user-agent", QString("aria2/")).toString());
    SetProperties(ui->spinBox_ConnectTimeout)->setValue(Update("connect-timeout", 60).toInt());
}

void LocalOptions::SetBittorrentOptions()
{
    SetProperties(ui->checkBox_EnableLpd, util::ARIA2C_VERSION_190)->setCheckState(Update("bt-enable-lpd", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_HashCheckSeed)->setCheckState(Update("bt-hash-check-seed", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_MetadataOnly)->setCheckState(Update("bt-metadata-only", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_RequireCrypto)->setCheckState(Update("bt-require-crypto", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_SaveMetadata)->setCheckState(Update("bt-save-metadata", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_SeedUnverified)->setCheckState(Update("bt-seed-unverified", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_FollowTorrent)->setCheckState(Update("follow-torrent", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_PeerExchange)->setCheckState(Update("enable-peer-exchange", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));

    bool ok;
    SetProperties(ui->doubleSpinBox_SeedRatio)->setValue(Update("seed-ratio", 1.0).toDouble(&ok));
    SetProperties(ui->spinBox_StopTimeout)->setValue(Update("bt-stop-timeout", 0).toInt());
    SetProperties(ui->spinBox_MaxUploadLimit)->setValue(Update("max-upload-limit", 0).toInt() / 1024);
    SetProperties(ui->spinBox_MaxOpenFiles)->setValue(Update("bt-max-open-files", 100).toInt());
    SetProperties(ui->spinBox_MaxPeers)->setValue(Update("bt-max-peers", 55).toInt());

    // ------- new elems ----------
    //seed time
    SetProperties(ui->spinBox_SeedTime)->setValue(Update("seed-time", -1).toInt());

    //External IP
    SetProperties(ui->lineEdit_ExternalIP)->setText(Update("bt-external-ip", QString("")).toString());

    //prioritize piece
    QString value = Update("bt-prioritize-piece", QString("")).toString();
    QStringList sSplited= value.split(",");
    bool bHead = false;
    bool bTail = false;
    int iHead = 1024;
    int iTail = 1024;
    foreach( QString s, sSplited)
    {
            QString name = s.left(4);
            QString value;
            int iValue = 1024;
            if (s.size() > 4)
            {
                    value = s;
                    value.remove(0, 5);
                    if (value.size() > 0)
                    {
                            value.remove(value.size() - 1, 1);
                            iValue = value.toInt();
                    }
            }
            if (name == "head")
            {
                    iHead = iValue;
                    bHead = true;
            }
            else if (name == "tail")
            {
                    iTail = iValue;
                    bTail = true;
            }
    }
    SetProperties(ui->checkBox_PrioritizePieceHead)->setCheckState(bHead ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_PrioritizePieceTail)->setCheckState(bTail ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->spinBox_PrioritizePieceHead_Size)->setValue(iHead);
    SetProperties(ui->spinBox_PrioritizePieceTail_Size)->setValue(iTail);
    SetProperties(ui->spinBox_PrioritizePieceHead_Size)->setEnabled(bHead);
    SetProperties(ui->spinBox_PrioritizePieceTail_Size)->setEnabled(bTail);
    // ------- new elems ----------

    SetProperties(ui->spinBox_TrackerInterval)->setValue(Update("bt-tracker-interval", 0).toInt());
    SetProperties(ui->spinBox_TrackerTimeout, util::ARIA2C_VERSION_191)->setValue(Update("bt-tracker-timeout", 60).toInt());
    SetProperties(ui->spinBox_TrackerConnectTimeout, util::ARIA2C_VERSION_191)->setValue(Update("bt-tracker-connect-timeout", 60).toInt());
    SetProperties(ui->lineEdit_PeerIDPrefix)->setText(Update("peer-id-prefix", QString("aria2/-")).toString());
    SetProperties(ui->spinBox_RequestPeerSpeedLimit)->setValue(Update("bt-request-peer-speed-limit", 51200).toInt() / 1024);

    QString s = Update("bt-min-crypto-level", QString("arc4")).toString();
    int iPos = -1;
    if (s == "arc4")
        iPos = 0;
    else if (s == "plain")
        iPos = 1;

    SetProperties(ui->comboBox_MinCryptoLevel)->setCurrentIndex(iPos);
    SetProperties(ui->lineEdit_BtTracker, util::ARIA2C_VERSION_1101)->setText(Update("bt-tracker", QString("")).toString());
    SetProperties(ui->lineEdit_BtExcludeTracker, util::ARIA2C_VERSION_1101)->setText(Update("bt-exclude-tracker", QString("")).toString());
}

void LocalOptions::SetMetalinkOptions()
{
    SetProperties(ui->radioButton_PreferredProtocol_HTTP)->setChecked(Update("metalink-preferred-protocol", QString("none")).toString() == QString("http"));
    SetProperties(ui->radioButton_PreferredProtocol_HTTPS)->setChecked(Update("metalink-preferred-protocol", QString("none")).toString() == QString("https"));
    SetProperties(ui->radioButton_PreferredProtocol_FTP)->setChecked(Update("metalink-preferred-protocol", QString("none")).toString() == QString("ftp"));
    SetProperties(ui->radioButton_PreferredProtocol_None)->setChecked(Update("metalink-preferred-protocol", QString("none")).toString() == QString("none"));

    SetProperties(ui->checkBox_FollowMetalink)->setCheckState(Update("follow-metalink", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_UniqueProtocol)->setCheckState(Update("metalink-enable-unique-protocol", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->spinBox_MetalinkServers)->setValue(Update("metalink-servers", 5).toInt());
    SetProperties(ui->lineEdit_BaseURI, util::ARIA2C_VERSION_1112)->setText(Update("metalink-base-uri", QString("")).toString());

    // ------- new elems ----------
    //Language
    SetProperties(ui->lineEdit_Language)->setText(Update("metalink-language", QString("")).toString());

    //Operation system
    SetProperties(ui->lineEdit_OS)->setText(Update("metalink-os", QString("")).toString());

    //Location
    SetProperties(ui->lineEdit_Location)->setText(Update("metalink-location", QString("")).toString());

    //Version
    SetProperties(ui->lineEdit_Version)->setText(Update("metalink-version", QString("")).toString());
    // ------- new elems ----------
}

void LocalOptions::SetAdvancedOptions()
{
    SetProperties(ui->checkBox_AllowOverwrite)->setCheckState(Update("allow-overwrite", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AllowPieceLengthChange)->setCheckState(Update("allow-piece-length-change", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AlwayResume, util::ARIA2C_VERSION_191)->setCheckState(Update("always-resume", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AsyncDns)->setCheckState(Update("async-dns", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AsyncDns6, util::ARIA2C_VERSION_1101)->setCheckState(Update("async-dns6", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_AutoFileRenaming)->setCheckState(Update("auto-file-renaming", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_CheckIntegrity)->setCheckState(Update("check-integrity", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_DryRun)->setCheckState(Update("dry-run", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));

    SetProperties(ui->checkBox_ReuseURI, util::ARIA2C_VERSION_190)->setCheckState(Update("reuse-uri", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_RemoveControlFile, util::ARIA2C_VERSION_191)->setCheckState(Update("remove-control-file", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_RemoteTime)->setCheckState(Update("remote-time", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_RealtimeChunkChecksum)->setCheckState(Update("realtime-chunk-checksum", true).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_NoNetrc)->setCheckState(Update("no-netrc", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_ParameterizedURI)->setCheckState(Update("parameterized-uri", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_UseHead)->setCheckState(Update("use-head", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));
    SetProperties(ui->checkBox_Pause, util::ARIA2C_VERSION_1120)->setCheckState(Update("pause", false).toBool() ? (Qt::Checked) : (Qt::Unchecked));

    SetProperties(ui->spinBox_NoFileAllocationLimit)->setValue(Update("no-file-allocation-limit", 5242880).toInt() / 1024);

    int iPos = -1;

    QString s = Update("file-allocation", QString("none")).toString();
    if (s == "none")
        iPos = 0;
    else if (s == "prealloc")
        iPos = 1;
    else if (s == "falloc")
        iPos = 2;

    SetProperties(ui->comboBox_FileAllocation)->setCurrentIndex(iPos);

    iPos = -1;
    s = Update("proxy-method", QString("get")).toString();
    if (s == "get")
        iPos = 0;
    else if (s == "tunnel")
        iPos = 1;

    SetProperties(ui->comboBox_ProxyMethod)->setCurrentIndex(iPos);
    SetProperties(ui->spinBox_RetryWait, util::ARIA2C_VERSION_1110)->setValue(Update("retry-wait", 0).toInt());

    iPos = -1;
    s = Update("stream-piece-selector", QString("default")).toString();
    if (s == "default")
        iPos = 0;
    else if (s == "inorder")
        iPos = 1;

    SetProperties(ui->comboBox_StreamPieceSelector, util::ARIA2C_VERSION_1120)->setCurrentIndex(iPos);

    SetProperties(ui->lineEdit_Referer)->setText(Update("referer", QString("")).toString());
    SetProperties(ui->lineEdit_Header)->setText(Update("header", QString("")).toString());
}

void LocalOptions::on_buttonBox_accepted()
{
    QString sText;

    //getBasic options
    getUpdate("dir", ui->lineEdit_Dir->text());
    getUpdate("split", QString::number(ui->spinBox_Split->value()));
    getUpdate("max-tries", QString::number(ui->spinBox_MaxTries->value()));
    getUpdate("timeout", QString::number(ui->spinBox_Timeout->value()));

    getUpdate("lowest-speed-limit", QString::number(ui->spinBox_LowestSpeedLimit->value() * 1024));
    getUpdate("max-download-limit", QString::number(ui->spinBox_MaxDownloadLimit->value() * 1024));
    getUpdate("max-file-not-found", QString::number(ui->spinBox_MaxFileNotFound->value()));
    getUpdate("max-resume-failure-tries", QString::number(ui->spinBox_MaxResumeFailureTries->value()));
    getUpdate("min-split-size", QString::number(ui->spinBox_MinSplitSize->value()));
    getUpdate("max-connection-per-server", QString::number(ui->spinBox_MaxConnectionPerServer->value()));

    getUpdate("continue", QVariant(ui->checkBox_ContinueDownload->checkState() == Qt::Checked).toString());

    //get FTP options
    getUpdate("ftp-pasv", QVariant(ui->radioButton_Mode_Passive->isChecked()).toString());
    getUpdate("connect-timeout", QString::number(ui->spinBox_FTP_ConnectTimeout->value()));
    getUpdate("ftp-reuse-connection", QVariant(ui->checkBox_ReuseConnection->checkState() == Qt::Checked).toString());
    getUpdate("ftp-type", ui->radioButton_Type_Binary->isChecked() ? (QString("binary")) : ("ascii"));

    //get HTTP options
    getUpdate("enable-http-keep-alive", QVariant(ui->checkBox_HTTPKeepAlive->checkState() == Qt::Checked).toString());
    getUpdate("enable-http-pipelining", QVariant(ui->checkBox_HTTPPipelining->checkState() == Qt::Checked).toString());
    getUpdate("http-accept-gzip", QVariant(ui->checkBox_AcceptGzip->checkState() == Qt::Checked).toString());
    getUpdate("http-auth-challenge", QVariant(ui->checkBox_AuthChallenge->checkState() == Qt::Checked).toString());
    getUpdate("http-no-cache", QVariant(ui->checkBox_HTTPNoCache->checkState() == Qt::Checked).toString());
    getUpdate("conditional-get", QVariant(ui->checkBox_HTTPConditionalGet->checkState() == Qt::Checked).toString());
    getUpdate("user-agent", ui->lineEdit_UserAgent->text());
    getUpdate("connect-timeout", QString::number(ui->spinBox_ConnectTimeout->value()));

    //get Bittorrent options
    getUpdate("bt-enable-lpd", QVariant(ui->checkBox_EnableLpd->checkState() == Qt::Checked).toString());
    getUpdate("bt-hash-check-seed", QVariant(ui->checkBox_HashCheckSeed->checkState() == Qt::Checked).toString());
    getUpdate("bt-metadata-only", QVariant(ui->checkBox_MetadataOnly->checkState() == Qt::Checked).toString());
    getUpdate("bt-require-crypto", QVariant(ui->checkBox_RequireCrypto->checkState() == Qt::Checked).toString());
    getUpdate("bt-save-metadata", QVariant(ui->checkBox_SaveMetadata->checkState() == Qt::Checked).toString());
    getUpdate("bt-seed-unverified", QVariant(ui->checkBox_SeedUnverified->checkState() == Qt::Checked).toString());
    getUpdate("follow-torrent", QVariant(ui->checkBox_FollowTorrent->checkState() == Qt::Checked).toString());
    getUpdate("enable-peer-exchange", QVariant(ui->checkBox_PeerExchange->checkState() == Qt::Checked).toString());

    bool ok;
    double dValue = ui->doubleSpinBox_SeedRatio->value();
    double dGlobalValue = m_globalOptions.value("seed-ratio", 1.0).toDouble(&ok);
    if (dValue == dGlobalValue)
        m_localOptions.remove("seed-ratio");
    else
        m_localOptions["seed-ratio"] = QString::number(dValue, 'f', 1);

    getUpdate("bt-stop-timeout", QString::number(ui->spinBox_StopTimeout->value()));
    getUpdate("max-upload-limit", QString::number(ui->spinBox_MaxUploadLimit->value() * 1024));
    getUpdate("bt-max-open-files", QString::number(ui->spinBox_MaxOpenFiles->value()));
    getUpdate("bt-max-peers", QString::number(ui->spinBox_MaxPeers->value()));

    int iValue = ui->spinBox_SeedTime->value();
    if (iValue == -1)
    {
        m_localOptions.remove("seed-time");
    }
    else
    {
        m_localOptions["seed-time"] = QString::number(iValue);
    }

    sText = ui->lineEdit_ExternalIP->text();
    QRegExp regex("([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})");
    int pos = 0;
    if ( (regex.indexIn(sText, pos) != -1) &&
         (regex.cap(1).toInt() < 256) &&
         (regex.cap(2).toInt() < 256) &&
         (regex.cap(3).toInt() < 256) &&
         (regex.cap(4).toInt() < 256) )
    {
        m_localOptions["bt-external-ip"] = sText;
    }
    else
    {
        m_localOptions.remove("bt-external-ip");
    }

    QString btPrioritizePiece;
    bool bHead = (ui->checkBox_PrioritizePieceHead->checkState() == Qt::Checked);
    bool bTail = (ui->checkBox_PrioritizePieceTail->checkState() == Qt::Checked);
    int iHead = ui->spinBox_PrioritizePieceHead_Size->value();
    int iTail = ui->spinBox_PrioritizePieceTail_Size->value();
    if (bHead)
    {
        btPrioritizePiece = QString("head");
        if (iHead != 1024)
        {
            btPrioritizePiece += (QString("=") + QString::number(iHead) + QString("K"));
        }
        if (bTail)
        {
             btPrioritizePiece += QString(",tail");
            if (iTail != 1024)
            {
                btPrioritizePiece += (QString("=") + QString::number(iTail) + QString("K"));
            }
        }
    }
    else
    {
        if (bTail)
        {
             btPrioritizePiece = QString("tail");
            if (iTail != 1024)
            {
                btPrioritizePiece += (QString("=") + QString::number(iTail) + QString("K"));
            }
        }
    }
    if (bHead || bTail)
        getUpdate("bt-prioritize-piece", btPrioritizePiece);

    getUpdate("bt-tracker-interval", QString::number(ui->spinBox_TrackerInterval->value()));
    getUpdate("bt-tracker-timeout", QString::number(ui->spinBox_TrackerTimeout->value()));
    getUpdate("bt-tracker-connect-timeout", QString::number(ui->spinBox_TrackerConnectTimeout->value()));
    getUpdate("peer-id-prefix", ui->lineEdit_PeerIDPrefix->text());
    getUpdate("bt-request-peer-speed-limit", QString::number(ui->spinBox_RequestPeerSpeedLimit->value() * 1024));
    getUpdate("bt-min-crypto-level", ui->comboBox_MinCryptoLevel->currentText());
    getUpdate("bt-tracker", ui->lineEdit_BtTracker->text());
    getUpdate("bt-exclude-tracker", ui->lineEdit_BtExcludeTracker->text());

    //get metalink options
    if (ui->radioButton_PreferredProtocol_HTTP->isChecked())
        getUpdate("metalink-preferred-protocol", "http");
    else if (ui->radioButton_PreferredProtocol_HTTPS->isChecked())
        getUpdate("metalink-preferred-protocol", "https");
    else if (ui->radioButton_PreferredProtocol_FTP->isChecked())
        getUpdate("metalink-preferred-protocol", "ftp");
    else if (ui->radioButton_PreferredProtocol_None->isChecked())
        getUpdate("metalink-preferred-protocol", "none");


    getUpdate("follow-metalink", QVariant(ui->checkBox_FollowMetalink->checkState() == Qt::Checked).toString());
    getUpdate("metalink-enable-unique-protocol", QVariant(ui->checkBox_UniqueProtocol->checkState() == Qt::Checked).toString());
    getUpdate("metalink-servers", QString::number(ui->spinBox_MetalinkServers->value()));

    sText = ui->lineEdit_BaseURI->text();
    if (m_localOptions.value("metalink-base-uri", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("metalink-base-uri");
        else
           m_localOptions["metalink-base-uri"] = sText;
    }

    sText = ui->lineEdit_Language->text();
    if (m_localOptions.value("metalink-language", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("metalink-language");
        else
           m_localOptions["metalink-language"] = sText;
    }

    sText = ui->lineEdit_OS->text();
    if (m_localOptions.value("metalink-os", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("metalink-os");
        else
           m_localOptions["metalink-os"] = sText;
    }

    sText = ui->lineEdit_Location->text();
    if (m_localOptions.value("metalink-location", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("metalink-location");
        else
           m_localOptions["metalink-location"] = sText;
    }

    sText = ui->lineEdit_Version->text();
    if (m_localOptions.value("metalink-version", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("metalink-version");
        else
           m_localOptions["metalink-version"] = sText;
    }

    //get Advanced options
    getUpdate("allow-overwrite", QVariant(ui->checkBox_AllowOverwrite->checkState() == Qt::Checked).toString());
    getUpdate("allow-piece-length-change", QVariant(ui->checkBox_AllowPieceLengthChange->checkState() == Qt::Checked).toString());
    getUpdate("always-resume", QVariant(ui->checkBox_AlwayResume->checkState() == Qt::Checked).toString());
    getUpdate("async-dns", QVariant(ui->checkBox_AsyncDns->checkState() == Qt::Checked).toString());
    getUpdate("async-dns6", QVariant(ui->checkBox_AsyncDns6->checkState() == Qt::Checked).toString());
    getUpdate("auto-file-renaming", QVariant(ui->checkBox_AutoFileRenaming->checkState() == Qt::Checked).toString());
    getUpdate("check-integrity", QVariant(ui->checkBox_CheckIntegrity->checkState() == Qt::Checked).toString());
    getUpdate("dry-run", QVariant(ui->checkBox_DryRun->checkState() == Qt::Checked).toString());

    getUpdate("reuse-uri", QVariant(ui->checkBox_ReuseURI->checkState() == Qt::Checked).toString());
    getUpdate("remove-control-file", QVariant(ui->checkBox_RemoveControlFile->checkState() == Qt::Checked).toString());
    getUpdate("remote-time", QVariant(ui->checkBox_RemoteTime->checkState() == Qt::Checked).toString());
    getUpdate("realtime-chunk-checksum", QVariant(ui->checkBox_RealtimeChunkChecksum->checkState() == Qt::Checked).toString());
    getUpdate("no-netrc", QVariant(ui->checkBox_NoNetrc->checkState() == Qt::Checked).toString());
    getUpdate("parameterized-uri", QVariant(ui->checkBox_ParameterizedURI->checkState() == Qt::Checked).toString());
    getUpdate("use-head", QVariant(ui->checkBox_UseHead->checkState() == Qt::Checked).toString());
    getUpdate("pause", QVariant(ui->checkBox_Pause->checkState() == Qt::Checked).toString());

    getUpdate("no-file-allocation-limit", QString::number(ui->spinBox_NoFileAllocationLimit->value() * 1024));
    getUpdate("file-allocation", ui->comboBox_FileAllocation->currentText());
    getUpdate("proxy-method", ui->comboBox_ProxyMethod->currentText());
    getUpdate("retry-wait", QString::number(ui->spinBox_NoFileAllocationLimit->value()));
    getUpdate("stream-piece-selector", ui->comboBox_StreamPieceSelector->currentText());

    sText = ui->lineEdit_Referer->text();
    if (m_localOptions.value("referer", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("referer");
        else
           m_localOptions["referer"] = sText;
    }

    sText = ui->lineEdit_Header->text();
    if (m_localOptions.value("header", QString("")).toString() != sText)
    {
        if (sText.size() == 0)
           m_localOptions.remove("header");
        else
           m_localOptions["header"] = sText;
    }

    //get Proxy list
    if (ui->checkBox_Proxy_All_Enabled->checkState() == Qt::Checked)
    {
        //all proxy
        int iSize = ui->treeWidget_Proxy_All_List->topLevelItemCount();
        bool bFound = false;
        for (int i = 0; i < iSize; ++i)
        {
            QTreeWidgetItem *item = ui->treeWidget_Proxy_All_List->topLevelItem(i);

            if ( (item != 0) && (item->checkState(0) == Qt::Checked) )
            {
                //[http://][USER:PASSWORD@]HOST[:PORT]
                SERVER_ITEM si = m_servers.GetServers(SERVER_ALL_PROXY)[i];
                m_localOptions["all-proxy"] = QString("http://%1:%2@%3:%4")
                                              .arg(si.user)
                                              .arg(si.password)
                                              .arg(si.server)
                                              .arg(si.port);
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            m_localOptions.remove("all-proxy");
        }
    }
    else
    {
        //http, https, ftp proxy
        for (int y = 1; y < 4; ++y)
        {
            QTreeWidget *treeWidget = 0;
            QString sProxyType;
            switch (y)
            {
            case 1:
                treeWidget = ui->treeWidget_Proxy_HTTP_List;
                sProxyType = "http-proxy";
                break;
            case 2:
                treeWidget = ui->treeWidget_Proxy_HTTPS_List;
                sProxyType = "https-proxy";
                break;
            case 3:
                treeWidget = ui->treeWidget_Proxy_FTP_List;
                sProxyType = "ftp-proxy";
                break;
            }

            int iSize = treeWidget->topLevelItemCount();
            bool bFound = false;
            for (int i = 0; i < iSize; ++i)
            {
                QTreeWidgetItem *currentItem = treeWidget->topLevelItem(i);

                if ( (currentItem != 0) && (currentItem->checkState(0) == Qt::Checked) )
                {
                    //[http://][USER:PASSWORD@]HOST[:PORT]
                    SERVER_ITEM si = m_servers.GetServers((SERVER_TYPE)y)[i];
                    m_localOptions[sProxyType] = QString("http://%1:%2@%3:%4")
                                                  .arg(si.user)
                                                  .arg(si.password)
                                                  .arg(si.server)
                                                  .arg(si.port);
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                m_localOptions.remove(sProxyType);
            }
        }
    }

    //get Server list
    int iSize = ui->treeWidget_Server_List->topLevelItemCount();
    bool bFound = false;
    for (int i = 0; i < iSize; ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget_Server_List->topLevelItem(i);
        if ( (item != 0) && (item->checkState(0) == Qt::Checked) )
        {
            SERVER_ITEM si = m_servers.GetServers(SERVER_HTTP_FTP)[i];
            QUrl url(si.server);
            QString scheme = url.scheme();
            if ((scheme == "http") || (scheme == "") )
            {
                m_localOptions["http-user"] = si.user;
                m_localOptions["http-passwd"] = si.password;
            }
            else if (scheme == "ftp")
            {
                m_localOptions["ftp-user"] = si.user;
                m_localOptions["ftp-passwd"] = si.password;
            }
            break;
        }
    }
    if (!bFound)
    {
        m_localOptions.remove("http-user");
        m_localOptions.remove("http-passwd");
    }
}
