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

#ifndef ARIA2CREMOTE_H
#define ARIA2CREMOTE_H

#include <QMainWindow>
#include <QTabWidget>
#include <QIcon>
#include <QSplitter>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QSettings>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QClipboard>
#include <QMutex>
#include <QTreeWidgetItem>
#include <QStatusBar>
#include "tabwidget.h"
#include "listview.h"
#include "mainlistview.h"
#include "adduri.h"
#include "add_http_ftp_magnetlink.h"
#include "globaloptions.h"
#include "selecttorrentfiles.h"
#include "download.h"
#include "workthread.h"
#include "reguestthread.h"
#include "transfer.h"
#include "QTreeWidgetItemEx.h"
#include "statusbarex.h"

namespace Ui {
    class Aria2cRemote;
}

typedef struct downloadList
{
    QTreeWidgetItemEx* item;
    xmlrpc::XmlRPC list;
}DOWNLOAD_LIST, *PDOWNLOAD_LIST;

class Aria2cRemote : public QMainWindow {
    Q_OBJECT
public:
    Aria2cRemote(QWidget *parent = 0);
    ~Aria2cRemote();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_actionGlobal_Options_triggered();
    void on_actionForce_power_off_Aria2c_triggered();
    void on_actionForce_remove_triggered();
    void on_actionForce_pause_all_triggered();
    void on_actionForce_pause_triggered();
    void on_actionPower_off_Aria2c_triggered();
    void on_actionPurge_triggered();
    void on_action_Remove_triggered();
    void on_actionPause_all_triggered();
    void on_action_Pause_triggered();
    void on_actionStart_all_triggered();
    void on_action_Start_triggered();
    void on_actionOption_triggered();
    void ResponseXML(XML_RPC_RESPONSE_MAP tellActive, XML_RPC_RESPONSE_MAP tellStopped, XML_RPC_RESPONSE_MAP tellWaiting);
    void processFaultToUI( int requestId, int errorCode, QString errorString );
    void ResponseVersionInfo(QVariant params);
    void ShowTransferDialog(QString sDescription);
    void HideTransferDialog();
    void GetGlobalOptions(QVariant value);

private:
    Ui::Aria2cRemote *ui;
    DetailsTabView *m_DetailsTab;
    ListView *m_listView;
    MainListView *m_mainListView;
    QSplitter *m_mainSplitter;
    QSplitter *m_mainSplitter1;
    QSystemTrayIcon *m_SystemTrayIcon;

    workThread m_workThread;
    reguestThread m_requestThread;
    QMutex m_Aria2RequestResponseSynchronize;

    Transfer *m_transfer;

    //global options
    QMap<QString, Variant> m_globalOptions;
    bool bValidGlobalOption;

    //connected to aria2c
    bool m_bConnected;

    //current item
    qint64 m_currentGID;

    //Connection options + proxy
    QString m_host;
    QString m_user;
    QString m_password;
    int m_port;
    QString m_proxyServer;
    QString m_proxyUser;
    QString m_proxyPassword;
    int m_proxyPort;

    XML_RPC_RESPONSE_MAP m_tellActive;
    XML_RPC_RESPONSE_MAP m_tellStopped;
    XML_RPC_RESPONSE_MAP m_tellWaiting;

    QMap<qint64, DOWNLOAD_LIST> m_downloadView;

    //Downloads status
    quint64 m_seeding;
    quint64 m_downloading;
    quint64 m_complete;
    quint64 m_pause;
    quint64 m_error;
    quint64 m_waiting;
    quint64 m_remove;

    //status bar
    StatusBarEx *statusBarEx;

    //status bar connect state + Aria2c version
    QLabel m_connectStateIcon;
    QLabel m_connectStateText;

private:
    void setTreeWidgetItem(QTreeWidgetItemEx *item, XmlRPC& x);
    void setInitTreeWidgetItem(QTreeWidgetItemEx *item);
    qint64 getCurrentGIDFromListView();

private slots:
    void on_actionAdd_MagnetLink_triggered();
    void on_actionAdd_Metalink_triggered();
    void on_actionAdd_Multi_HTTP_FTP_triggered();
    void on_actionAdd_HTTP_FTP_triggered();
    void on_actionAdd_Torrent_triggered();
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_action_AddURI();
    void ListViewItemClicked(QTreeWidgetItem*, int);
    void mainListViewItemClicked(QListWidgetItem*);
};

#endif // ARIA2CREMOTE_H
