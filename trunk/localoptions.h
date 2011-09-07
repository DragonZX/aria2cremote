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

#ifndef LOCALOPTIONS_H
#define LOCALOPTIONS_H

#include <QDialog>
#include <QVariant>
#include "QTreeWidgetItemEx.h"
#include "xmlrpc/variant.h"
#include "xmlrpc.h"
#include "download.h"
#include "util.h"
#include "server.h"

using namespace xmlrpc;

namespace Ui {
    class LocalOptions;
}

const quint32 ARIA2C_VERSION_191 = 0x010901;
const quint32 ARIA2C_VERSION_192 = 0x010902;
const quint32 ARIA2C_VERSION_193 = 0x010903;
const quint32 ARIA2C_VERSION_194 = 0x010904;
const quint32 ARIA2C_VERSION_195 = 0x010905;
const quint32 ARIA2C_VERSION_1100 = 0x010A00;
const quint32 ARIA2C_VERSION_1101 = 0x010A01;
const quint32 ARIA2C_VERSION_1102 = 0x010A02;
const quint32 ARIA2C_VERSION_1103 = 0x010A03;
const quint32 ARIA2C_VERSION_1104 = 0x010A04;
const quint32 ARIA2C_VERSION_1105 = 0x010A05;
const quint32 ARIA2C_VERSION_1106 = 0x010A06;
const quint32 ARIA2C_VERSION_1107 = 0x010A07;
const quint32 ARIA2C_VERSION_1108 = 0x010A08;
const quint32 ARIA2C_VERSION_1109 = 0x010A09;
const quint32 ARIA2C_VERSION_1110 = 0x010B00;
const quint32 ARIA2C_VERSION_1111 = 0x010B01;
const quint32 ARIA2C_VERSION_1112 = 0x010B02;
const quint32 ARIA2C_VERSION_1120 = 0x010C00;
const quint32 ARIA2C_VERSION_1121 = 0x010C01;

class LocalOptions : public QDialog {
    Q_OBJECT
public:
    LocalOptions(QWidget *parent = 0);
    ~LocalOptions();
    void setOptions(QMap<QString, Variant> globaloptions, Download &d/*QMap<QString, Variant> localoptions, XML_TYPE type*/);
    QMap<QString, Variant> getLocalOption() {return m_localOptions; }

protected:
    void changeEvent(QEvent *e);

private:

    Ui::LocalOptions *ui;
    Server m_servers;

    int m_currentServer[5];

//    QList<SERVER_ITEM> m_HTTP_FTP_serverList;
//    int m_HTTP_FTP_currentItem;
//
//    QList<SERVER_ITEM> m_Proxy_serverList[4];
//    int m_Proxy_CurrentItem[4];

    template <class T> T SetProperties(T widget, quint32 uiMinAria2cVersion = ARIA2C_VERSION_191);
    //global option
    QMap<QString, Variant> m_globalOptions;
    QMap<QString, Variant> m_localOptions;
    Download m_download;

    void SetBasicOptions();
    void SetHTTPOptions();
    void SetFTPOptions();
    void SetBittorrentOptions();
    void SetMetalinkOptions();
    void SetAdvancedOptions();
    void SetServerOptions();

    //Local settings update
    void getUpdate(QString name, QString value);
    Variant Update(QString name, Variant default_value);

private slots:
    void on_spinBox_SeedTime_valueChanged(int );
    void on_treeWidget_Proxy_FTP_List_itemChanged(QTreeWidgetItem* item, int column);
    void on_treeWidget_Proxy_HTTPS_List_itemChanged(QTreeWidgetItem* item, int column);
    void on_treeWidget_Proxy_HTTP_List_itemChanged(QTreeWidgetItem* item, int column);
    void on_treeWidget_Proxy_All_List_itemChanged(QTreeWidgetItem* item, int column);
    void on_treeWidget_Server_List_itemChanged(QTreeWidgetItem* item, int column);
    void on_spinBox_PrioritizePieceHead_Size_valueChanged(int );
    void on_buttonBox_accepted();
    void on_treeWidget_Proxy_FTP_List_itemSelectionChanged();
    void on_treeWidget_Proxy_HTTPS_List_itemSelectionChanged();
    void on_treeWidget_Proxy_HTTP_List_itemSelectionChanged();
    void on_pushButton_Proxy_Modify_clicked();
    void on_pushButton_Proxy_Delete_clicked();
    void on_pushButton_Proxy_Add_clicked();
    void on_treeWidget_Server_List_itemSelectionChanged();
    void on_checkBox_PrioritizePieceTail_stateChanged(int );
    void on_checkBox_PrioritizePieceHead_stateChanged(int );
    void on_spinBox_ConnectTimeout_valueChanged(int );
    void on_spinBox_FTP_ConnectTimeout_valueChanged(int );
    void on_pushButton_Servers_Modify_clicked();
    void on_pushButton_Servers_Delete_clicked();
    void on_pushButton_Servers_Add_clicked();
    void on_tabWidget_Proxy_currentChanged(int index);
    void on_checkBox_Proxy_All_Enabled_stateChanged(int );
    void on_treeWidget_Proxy_All_List_itemSelectionChanged();
};

#endif // LOCALOPTIONS_H
