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
 
#include "aria2cremote.h"
#include "ui_aria2cremote.h"
#include "metainfo.h"
#include "util.h"
#include "server.h"
#include "aria2options.h"
#include <QFileIconProvider>
#include <QRegExp>

Aria2cRemote::Aria2cRemote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Aria2cRemote),
    m_transfer(NULL),
    bValidGlobalOption(false),
    m_bConnected(false),
    m_currentGID(-1),
    m_host("localhost"),
    m_user(""),
    m_password(""),
    m_port(6800),
    m_proxyServer(""),
    m_proxyUser(""),
    m_proxyPassword(""),
    m_proxyPort(8080),
    m_seeding(0),
    m_downloading(0),
    m_complete(0),
    m_pause(0),
    m_error(0),
    m_waiting(0),
    m_remove(0)
 {
    ui->setupUi(this);

    //Connection options load
    util::LoadConnectionList(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);

    //set main view;
    m_DetailsTab = new DetailsTabView;
    m_listView = new ListView;
    m_mainListView = new MainListView;

    QIcon icon(":/icon/logo_trayicon.png");

    m_mainSplitter = new QSplitter(Qt::Horizontal);
    m_mainSplitter1 = new QSplitter(Qt::Vertical);

    m_mainSplitter->addWidget(m_mainListView);
    m_mainSplitter->addWidget(m_listView);
    m_mainSplitter->setStretchFactor(1, 1);

    //Set attributes
    m_mainSplitter->setCollapsible(0, false);
    m_mainSplitter->setCollapsible(1, false);


    m_mainSplitter1->addWidget(m_mainSplitter);
    m_mainSplitter1->addWidget(m_DetailsTab);

    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter1->setCollapsible(0, false);
    m_mainSplitter1->setCollapsible(1, false);

    //init status bar
    statusBar()->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
    statusBarEx = new StatusBarEx();
    statusBar()->addPermanentWidget(statusBarEx);

    //statusbar version
    m_connectStateText.setText(tr("Disconnected"));
    m_connectStateText.setToolTip("");
    QIcon iconConnect(":/icon/block.png");
    QSize size = iconConnect.actualSize(QSize(16, 16));
    m_connectStateIcon.setPixmap(iconConnect.pixmap(size));
    statusBar()->addWidget(&m_connectStateIcon);
    statusBar()->addWidget(&m_connectStateText);

    //SystemTrayIcon
    QMenu *m_SystemTrayMenu = new QMenu;

    m_SystemTrayMenu->addAction(ui->action_AddURI);
    m_SystemTrayMenu->addSeparator();
    m_SystemTrayMenu->addAction(ui->action_Start);
    m_SystemTrayMenu->addAction(ui->actionStart_all);
    m_SystemTrayMenu->addAction(ui->action_Pause);
    m_SystemTrayMenu->addAction(ui->actionPause_all);
    m_SystemTrayMenu->addSeparator();
    m_SystemTrayMenu->addAction(ui->actionForce_pause);
    m_SystemTrayMenu->addAction(ui->actionForce_pause_all);
    m_SystemTrayMenu->addSeparator();
    m_SystemTrayMenu->addAction(ui->action_Exit);

    m_SystemTrayIcon = new QSystemTrayIcon(icon);

    m_SystemTrayIcon->setContextMenu(m_SystemTrayMenu);
    m_SystemTrayIcon->show();

    setCentralWidget(m_mainSplitter1);
    setWindowTitle(tr("Aria2c Remote"));

    //Set list view
    m_mainListView->setCurrentRow(0);

    //Working thread init
    m_workThread.setConnection(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);
    m_workThread.SetSleep(5000);
    m_workThread.setRequestResponseSynchronize(&m_Aria2RequestResponseSynchronize);
    qRegisterMetaType <XML_RPC_RESPONSE_MAP>("XML_RPC_RESPONSE_MAP");
    connect(&m_workThread, SIGNAL(Response(XML_RPC_RESPONSE_MAP, XML_RPC_RESPONSE_MAP, XML_RPC_RESPONSE_MAP)), this, SLOT(ResponseXML(XML_RPC_RESPONSE_MAP, XML_RPC_RESPONSE_MAP, XML_RPC_RESPONSE_MAP)));
    connect(&m_workThread, SIGNAL(processFaultToUI(int, int, QString)), this, SLOT(processFaultToUI(int, int, QString)));
    m_workThread.start();

    m_requestThread.setRequestResponseSynchronize(&m_Aria2RequestResponseSynchronize);
    m_requestThread.setConnection(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);
    connect(&m_requestThread, SIGNAL(ShowTransferDialog(QString)), this, SLOT(ShowTransferDialog(QString)));
    connect(&m_requestThread, SIGNAL(HideTransferDialog()), this, SLOT(HideTransferDialog()));
    connect(&m_requestThread, SIGNAL(processFaultToUI(int, int, QString)), this, SLOT(processFaultToUI(int, int, QString)));
    connect(&m_requestThread, SIGNAL(ResponseVersionInfo(QVariant)), this, SLOT(ResponseVersionInfo(QVariant)));
    m_requestThread.start();

    QFileIconProvider fileIconProvider;
    ui->action_AddURI->setIcon(fileIconProvider.icon(QFileIconProvider::Folder));

    //set connect
    connect(m_listView, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(ListViewItemClicked(QTreeWidgetItem*, int)));
    connect(m_mainListView, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(mainListViewItemClicked(QListWidgetItem*)));

    //set Add state
    ui->action_AddURI->setEnabled(false);
    ui->actionAdd_HTTP_FTP->setEnabled(false);
    ui->actionAdd_Multi_HTTP_FTP->setEnabled(false);
    ui->actionAdd_Torrent->setEnabled(false);
    ui->actionAdd_MagnetLink->setEnabled(false);
    ui->actionAdd_Metalink->setEnabled(false);
    ui->actionGlobal_Options->setEnabled(false);
    //set Action state
    ui->action_Start->setEnabled(false);
    ui->actionStart_all->setEnabled(false);
    ui->action_Pause->setEnabled(false);
    ui->actionPause_all->setEnabled(false);
    ui->action_Remove->setEnabled(false);
    ui->actionPurge->setEnabled(false);
    ui->actionPower_off_Aria2c->setEnabled(false);
    ui->actionForce_pause->setEnabled(false);
    ui->actionForce_pause_all->setEnabled(false);
    ui->actionForce_remove->setEnabled(false);
    ui->actionForce_power_off_Aria2c->setEnabled(false);

    //set mainlistview state

    m_mainListView->setEnabled(false);
    m_DetailsTab->setEnabled(false);
    m_listView->setEnabled(false);
}

Aria2cRemote::~Aria2cRemote()
{
    delete ui;
    m_SystemTrayIcon->hide();
    m_workThread.stop();
    m_requestThread.stop();
    m_requestThread.wakeUp();
    m_workThread.wait();
    m_requestThread.wait();
}

void Aria2cRemote::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Aria2cRemote::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void Aria2cRemote::on_actionAbout_triggered()
{
    About a(this);

    a.exec();
}

void Aria2cRemote::on_action_AddURI()
{
    AddURI a(this);

    ui->action_AddURI->setEnabled(false);
    a.setRequestThread(&m_requestThread);
    connect(&m_requestThread, SIGNAL(ResponseGetGlobalOptions(QVariant)), &a, SLOT(GetGlobalOptions(QVariant)));
    if (a.exec() == QDialog::Accepted)
    {
        QList<Download> dlist = a.getDownloadList();
        if (dlist.size() != 0)
        {
            m_requestThread.addRequest(dlist);
            m_requestThread.setDescriptionText(tr("Add new multi downloads"));
            m_requestThread.wakeUp();
        }
    }
    disconnect(&m_requestThread, SIGNAL(ResponseGetGlobalOptions(QVariant)), &a, SLOT(GetGlobalOptions(QVariant)));
    ui->action_AddURI->setEnabled(true);
}

void Aria2cRemote::on_actionAdd_Torrent_triggered()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QList<Download> dlist;
    QMap<QString, Variant> vCurrentParam;
    int iAddTorrent = 0;

    QStringList sFileNameList = QFileDialog::getOpenFileNames(this,
                                                              tr("Open torrent file"),
                                                              tr(""),
                                                              tr("Torrent Files (*.torrent)"),
                                                              &selectedFilter,
                                                              options);

    foreach(QString sFileName, sFileNameList)
    {
        if (sFileName.size() != 0)
        {
            //select files in torrent file
            SelectTorrentFiles torrent(this);
            MetaInfo metainfo;

            if (util::ReadTorrentFile(sFileName, metainfo))
            {
                torrent.SetData(metainfo);
                if (torrent.exec() == QDialog::Accepted)
                {
                    Download d;

                    vCurrentParam["select-file"] = torrent.getSelectedFileList();
                    d.addTorrent(sFileName, vCurrentParam);
                    dlist << d;
                    iAddTorrent++;
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Error"), tr("Torrent file read error"));
            }
        }
    }
    if (iAddTorrent > 0)
    {
        m_requestThread.addRequest(dlist);
        m_requestThread.setDescriptionText(tr("Add new torrent download"));
        m_requestThread.wakeUp();
    }
}

void Aria2cRemote::on_actionAdd_HTTP_FTP_triggered()
{
    const QClipboard *clipboard = QApplication::clipboard();
    QString sStr = clipboard->text(QClipboard::Clipboard);
    add_http_ftp_magnetlink add(this);
    QString sUrl;
    QMap<QString, Variant> vCurrentParam;
    QIcon icon(":/icon/uri/ftp.png");

    add.SetURI(sStr);
    add.setWindowIcon(icon);
    add.SetTypeText("URL:");
    add.setWindowTitle(tr("Add URL"));
    if (add.exec() == QDialog::Accepted)
    {
        sUrl = add.GetURI();
        util::deletePrePostSpace(sUrl);
        if (sUrl.size() > 0)
        {
            SERVER_ITEM si;
            Server s;
            Download d;

            s.LoadServerList();
            if (s.MatchedServer(sUrl, si))
            {
                QUrl url(si.server);
                QString scheme = url.scheme();
                if ((scheme == "http") || (scheme == "") )
                {
                    vCurrentParam["http-user"] = si.user;
                    vCurrentParam["http-passwd"] = si.password;
                }
                else if (scheme == "ftp")
                {
                    vCurrentParam["ftp-user"] = si.user;
                    vCurrentParam["ftp-passwd"] = si.password;
                }
            }
            d.addHttpFtp(sUrl, vCurrentParam);

            m_requestThread.addRequest(d);
            m_requestThread.setDescriptionText(tr("Add new HTTP/FTP download"));
            m_requestThread.wakeUp();
        }
    }
}

void Aria2cRemote::on_actionAdd_Multi_HTTP_FTP_triggered()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QList<QString> sUrls;
    QMap<QString, Variant> vCurrentParam;
    QList<Download> dlist;
    int iFound = 0;
    Server s;

    QStringList sFileNameList = QFileDialog::getOpenFileNames(this,
                                                              tr("Open URI link list file"),
                                                              tr(""),
                                                              tr("URI link list Files (*.txt);;All files (*.*)"),
                                                              &selectedFilter,
                                                              options);

    if (sFileNameList.size() != 0)
    {
        s.LoadServerList();
    }
    foreach(QString sFileName, sFileNameList)
    {
        if (sFileName.size() != 0)
        {

            QFile f(sFileName);

            if (f.open(QFile::ReadOnly))
            {
                QFileInfo fInfo(sFileName);
                bool bRegExDecode = (fInfo.suffix() != "txt");

                if (bRegExDecode)
                {
                    QRegExp regex("(www\\.(?!\\.)|([a-zA-z]+)://)[\\d\\w\\./,:_~\\?=&;#@\\-\\+\\%\\$]+[\\d\\w/]");

                    QString str(f.readAll());
                    f.close();

                    int pos = 0;
                    while ((pos = regex.indexIn(str, pos)) != -1)
                    {
                        QString str1(regex.cap());
                        bool bFound = false;
                        foreach (QString s, sUrls)
                        {
                            if (s == str1)
                            {
                                bFound = true;
                                break;
                            }
                        }

                        if (!bFound)
                        {
                            Download d;
                            SERVER_ITEM si;

                            if (s.MatchedServer(str1, si))
                            {
                                QUrl url(si.server);
                                QString scheme = url.scheme();
                                if ((scheme == "http") || (scheme == "") )
                                {
                                    vCurrentParam["http-user"] = si.user;
                                    vCurrentParam["http-passwd"] = si.password;
                                }
                                else if (scheme == "ftp")
                                {
                                    vCurrentParam["ftp-user"] = si.user;
                                    vCurrentParam["ftp-passwd"] = si.password;
                                }
                            }
                            d.addHttpFtp(str1, vCurrentParam);
                            dlist << d;

                            sUrls << str1;
                            iFound++;
                        }

                        pos += regex.matchedLength();
                    }
                }
                else
                {
                    QTextStream stream( &f );
                    QString line;
                    while ( !stream.atEnd() )
                    {
                        line = stream.readLine();
                        bool bFound = false;
                        foreach (QString s, sUrls)
                        {
                            if (s == line)
                            {
                                bFound = true;
                                break;
                            }
                        }
                        if (!bFound)
                        {
                            Download d;
                            SERVER_ITEM si;

                            util::deletePrePostSpace(line);

                            if (line.size() > 0)
                            {
                                if (s.MatchedServer(line, si))
                                {
                                    QUrl url(si.server);
                                    QString scheme = url.scheme();
                                    if ((scheme == "http") || (scheme == "") )
                                    {
                                        vCurrentParam["http-user"] = si.user;
                                        vCurrentParam["http-passwd"] = si.password;
                                    }
                                    else if (scheme == "ftp")
                                    {
                                        vCurrentParam["ftp-user"] = si.user;
                                        vCurrentParam["ftp-passwd"] = si.password;
                                    }
                                }
                                d.addHttpFtp(line, vCurrentParam);
                                dlist << d;

                                sUrls << line;
                                iFound++;
                            }
                        }
                    }
                }
                f.close();
            }
            else
            {
                QMessageBox::warning(this, tr("Multiple file open"), tr("Unable open to %1").arg(sFileName));
            }
        }
    }
    if (iFound != 0)
    {
       m_requestThread.addRequest(dlist);
       m_requestThread.setDescriptionText(tr("Add new multi HTTP/FTP download"));
       m_requestThread.wakeUp();
    }
}

void Aria2cRemote::on_actionAdd_Metalink_triggered()
{
   QFileDialog::Options options;
   QString selectedFilter;
   QList<QString> sUrls;
   QMap<QString, Variant> vCurrentParam;
   QList<Download> dlist;
   int iAddMetalink = 0;

   QStringList sFileNameList = QFileDialog::getOpenFileNames(this,
                               tr("Open metalink files"),
                               tr(""),
                               tr("Metalink Files (*.metalink)"),
                               &selectedFilter,
                               options);

   foreach(QString sFileName, sFileNameList)
   {
       if (sFileName.size() != 0)
       {
           Download d;

           d.addMetalink(sFileName, vCurrentParam);
           dlist << d;
           iAddMetalink++;
       }
   }
   if (iAddMetalink > 0)
   {
       m_requestThread.addRequest(dlist);
       m_requestThread.setDescriptionText(tr("Add new metalink download"));
       m_requestThread.wakeUp();
   }
}

void Aria2cRemote::on_actionAdd_MagnetLink_triggered()
{
    const QClipboard *clipboard = QApplication::clipboard();
    QString sStr = clipboard->text(QClipboard::Clipboard);
    add_http_ftp_magnetlink add(this);
    QMap<QString, Variant> vCurrentParam;
    QString sMagnetLink;
    QIcon icon(":/icon/uri/magnet.png");

    add.SetURI(sStr);
    add.setWindowIcon(icon);
    add.SetTypeText(tr("Magnet Link:"));
    add.setWindowTitle(tr("Add Magnet Link"));
    if (add.exec() == QDialog::Accepted)
    {
        sMagnetLink = add.GetURI();

        Download d;
        d.addMagnetLink(sMagnetLink, vCurrentParam);

        m_requestThread.addRequest(d);
        m_requestThread.setDescriptionText(tr("Add new magnetlink download"));
        m_requestThread.wakeUp();
    }
}

void Aria2cRemote::processFaultToUI( int requestId, int errorCode, QString errorString )
{
    m_globalOptions.clear();
    m_tellActive.clear();
    m_tellStopped.clear();
    m_tellWaiting.clear();

    bValidGlobalOption = false;
    if (m_bConnected)
    {
        m_currentGID = -1;
        m_bConnected = false;

        ui->action_AddURI->setEnabled(m_bConnected);
        ui->actionAdd_HTTP_FTP->setEnabled(m_bConnected);
        ui->actionAdd_Multi_HTTP_FTP->setEnabled(m_bConnected);
        ui->actionAdd_Torrent->setEnabled(m_bConnected);
        ui->actionAdd_MagnetLink->setEnabled(m_bConnected);
        ui->actionAdd_Metalink->setEnabled(m_bConnected);
        ui->actionGlobal_Options->setEnabled(m_bConnected);

        ui->action_Start->setEnabled(m_bConnected);
        ui->actionStart_all->setEnabled(m_bConnected);
        ui->action_Pause->setEnabled(m_bConnected);
        ui->actionPause_all->setEnabled(m_bConnected);
        ui->action_Remove->setEnabled(m_bConnected);
        ui->actionPurge->setEnabled(m_bConnected);
        ui->actionPower_off_Aria2c->setEnabled(m_bConnected);
        ui->actionForce_pause->setEnabled(m_bConnected);
        ui->actionForce_pause_all->setEnabled(m_bConnected);
        ui->actionForce_remove->setEnabled(m_bConnected);
        ui->actionForce_power_off_Aria2c->setEnabled(m_bConnected);

        //erase all items
        foreach(qint64 gid, m_downloadView.keys() )
        {
            DOWNLOAD_LIST list;
            list.item = 0;
            QTreeWidgetItem *item = m_downloadView.value(gid, list).item;
            if (item != 0)
            {
                m_listView->takeTopLevelItem(m_listView->indexOfTopLevelItem( item ));
                delete item;
            }
            m_downloadView.erase(m_downloadView.find(gid));
        }
        m_downloadView.clear();

        m_mainListView->item(0)->setText(tr("All"));
        m_mainListView->item(1)->setText(tr("Seeding"));
        m_mainListView->item(2)->setText(tr("Downloading"));
        m_mainListView->item(3)->setText(tr("Complete"));
        m_mainListView->item(4)->setText(tr("Pause"));
        m_mainListView->item(5)->setText(tr("Error"));
        m_mainListView->item(6)->setText(tr("Waiting"));
        m_mainListView->item(7)->setText(tr("Remove"));

        m_mainListView->setEnabled(m_bConnected);
        m_DetailsTab->setEnabled(m_bConnected);
        m_listView->setEnabled(m_bConnected);

        //set status bar connect state
        m_connectStateText.setText(tr("Disconnected"));
        m_connectStateText.setToolTip("");
        QIcon iconConnect(":/icon/block.png");
        QSize size = iconConnect.actualSize(QSize(16, 16));
        m_connectStateIcon.setPixmap(iconConnect.pixmap(size));

        QMessageBox::warning(this, tr("Request failed"), QString(tr("XML-RPC request  failed.\n\nFault code: %1\n%2\n")).arg(errorCode).arg(errorString), QMessageBox::Ok );
    }
}

void Aria2cRemote::ResponseVersionInfo(QVariant params)
{
    xmlrpc::XmlRPC versioninfo;
    XMLRPC_VERSION_INFO vInfo;

    versioninfo.versionInfo(params);
    vInfo = versioninfo.getVersionInfo();
    m_connectStateText.setText("Aria2c " + vInfo.version);

    QString tooltip(QString("<b>%1:</b>").arg(tr("Enabled features")));
    foreach (QString t, vInfo.enabledFeatures)
    {
        if (t.compare("gzip", Qt::CaseInsensitive) == 0)
        {
            m_requestThread.SetGZipEnabled();
            m_workThread.SetGZipEnabled();
        }

        tooltip += ("<br/>" + t);
    }
    m_connectStateText.setToolTip(tooltip);
}

void Aria2cRemote::ResponseXML(XML_RPC_RESPONSE_MAP tellActive, XML_RPC_RESPONSE_MAP tellStopped, XML_RPC_RESPONSE_MAP tellWaiting)
{
    QMap<qint64, DOWNLOAD_LIST> d;
    int iCurrentMode = m_mainListView->currentRow();

    //Set states
    if (!m_bConnected)
    {
        m_bConnected = true;
        ui->action_AddURI->setEnabled(m_bConnected);
        ui->actionAdd_HTTP_FTP->setEnabled(m_bConnected);
        ui->actionAdd_Multi_HTTP_FTP->setEnabled(m_bConnected);
        ui->actionAdd_Torrent->setEnabled(m_bConnected);
        ui->actionAdd_MagnetLink->setEnabled(m_bConnected);
        ui->actionAdd_Metalink->setEnabled(m_bConnected);
        ui->actionGlobal_Options->setEnabled(m_bConnected);

        ui->actionPower_off_Aria2c->setEnabled(m_bConnected);
        ui->actionForce_power_off_Aria2c->setEnabled(m_bConnected);

        m_mainListView->setEnabled(m_bConnected);
        m_DetailsTab->setEnabled(m_bConnected);
        m_listView->setEnabled(m_bConnected);

        //set status bar connect state
        m_connectStateText.setText(tr("Connected"));
        m_connectStateText.setToolTip("");
        QIcon iconConnect(":/icon/tick.png");
        QSize size = iconConnect.actualSize(QSize(16, 16));
        m_connectStateIcon.setPixmap(iconConnect.pixmap(size));

        Download d;
        QMap<QString, Variant> vCurrentParam;
        d.addVersionInfo(vCurrentParam);
        m_requestThread.addRequest(d);
        m_requestThread.wakeUp();
    }

    //clear
    m_tellActive.clear();
    m_tellStopped.clear();
    m_tellWaiting.clear();

    m_seeding =
    m_downloading =
    m_complete =
    m_pause =
    m_error =
    m_waiting =
    m_remove = 0;

    m_tellActive = tellActive;
    m_tellStopped = tellStopped;
    m_tellWaiting = tellWaiting;

    //tellActive
    QList<QTreeWidgetItemEx*> newItemsTellActive;
    foreach(xmlrpc::XmlRPC dw, tellActive)
    {
        DOWNLOAD_LIST list;
        qint64 gid = dw.getGID();
        bool bSeeding = (dw.getCompletedLength() != 0) && ( dw.getCompletedLength() == dw.getTotalLength() );
        //All or Seeding or Downloading
        if ( (iCurrentMode == 0) ||
             ( (iCurrentMode == 1) &&  bSeeding) ||
             ( (iCurrentMode == 2) && !bSeeding) )
        {
            if (!m_downloadView.contains(gid))
            {
                list.item = new QTreeWidgetItemEx;
                setInitTreeWidgetItem(list.item);
                newItemsTellActive << list.item;
            }
            else
            {
                list = m_downloadView.find(gid).value();
            }
            list.list = dw;
            setTreeWidgetItem(list.item, dw);
            d.insert(gid, list);
        }
        if ( ((dw.getType() == TORRENT) || (dw.getType() == MAGNETLINK)) && bSeeding )
            m_seeding++; //Seeding -> iCurrentMode = 1
        else
            m_downloading++; // Complete -> iCurrentMode = 2
    }

    foreach(QTreeWidgetItemEx * itemEx, newItemsTellActive)
    {
        m_listView->addTopLevelItem( itemEx );
    }

    //tellStopped
    QList<QTreeWidgetItemEx*> newItemsTellStopped;
    foreach(xmlrpc::XmlRPC dw, tellStopped)
    {
        DOWNLOAD_LIST list;
        qint64 gid = dw.getGID();
        URI_STATUS status = dw.getiStatus();
        //All or Seeding or Downloading or Paused or Error or Remove
        if ( (iCurrentMode == 0) ||
             ( (iCurrentMode == 3) && (status == STATUS_COMPLETE) ) ||
             ( (iCurrentMode == 4) && (status == STATUS_PAUSED) ) ||
             ( (iCurrentMode == 5) && (status == STATUS_ERROR) ) ||
             ( (iCurrentMode == 7) && (status == STATUS_REMOVED) ) )
        {
            if (!m_downloadView.contains(gid))
            {
                list.item = new QTreeWidgetItemEx;
                setInitTreeWidgetItem(list.item);
                newItemsTellStopped << list.item;
            }
            else
            {
                list = m_downloadView.find(gid).value();
            }
            list.list = dw;
            setTreeWidgetItem(list.item, dw);
            d.insert(gid, list);
        }
        switch (status)
        {
        case STATUS_COMPLETE:
            m_complete++;
            break;
        case STATUS_PAUSED:
             m_pause++;
            break;
        case STATUS_ERROR:
            m_error++;
            break;
        case STATUS_REMOVED:
            m_remove++;
            break;
        default:
            break;
        }
    }
    foreach(QTreeWidgetItemEx * itemEx, newItemsTellStopped)
    {
        m_listView->addTopLevelItem( itemEx );
    }

    //tellWaiting
    QList<QTreeWidgetItemEx*> newItemsTellWaiting;
    foreach(xmlrpc::XmlRPC dw, tellWaiting)
    {
        DOWNLOAD_LIST list;
        qint64 gid = dw.getGID();
        if ( (iCurrentMode == 0) || (iCurrentMode == 6) )
        {
            if (!m_downloadView.contains(gid))
            {
                list.item = new QTreeWidgetItemEx;
                setInitTreeWidgetItem(list.item);
                newItemsTellWaiting << list.item;
            }
            else
            {
                list = m_downloadView.find(gid).value();
            }
            list.list = dw;
            setTreeWidgetItem(list.item, dw);
            d.insert(gid, list);
        }
        m_waiting++; // Waiting -> iCurrentMode = 6
    }
    foreach(QTreeWidgetItemEx * itemEx, newItemsTellWaiting)
    {
        m_listView->addTopLevelItem( itemEx );
    }

    //erase not visibled download
    foreach(qint64 gid, m_downloadView.keys() )
    {
        if (!d.contains(gid))
        {
            DOWNLOAD_LIST dw_list;
            dw_list.item = 0;
            QTreeWidgetItemEx *item = m_downloadView.value(gid, dw_list).item;
            if (item != 0)
            {
                m_listView->takeTopLevelItem(m_listView->indexOfTopLevelItem( item ));
                delete item;
            }
            m_downloadView.erase(m_downloadView.find(gid));
        }
    }

    m_downloadView.clear();
    m_downloadView = d;

    QTreeWidgetItemEx *currentItem = (QTreeWidgetItemEx *)m_listView->currentItem();
    xmlrpc::XmlRPC currentDownload;
    bool bUnPause = false;
    bool bUnPauseAll = false;
    bool bPause = false;
    bool bPauseAll = false;
    bool bRemove = false;
    bool bPurge = false;
    quint64 uOverAllDownloadSpeed = 0;
    quint64 uOverAllUploadSpeed = 0;

    m_currentGID = -1;
    foreach(DOWNLOAD_LIST dl, m_downloadView)
    {
        URI_STATUS status = dl.list.getiStatus();
        if ( (currentItem != 0) && (dl.item == currentItem) )
        {
            currentDownload = dl.list;
            m_currentGID = currentDownload.getGID();

            bUnPause = (status == STATUS_PAUSED);
            bPause = (status == STATUS_ACTIVE);
            bRemove = true;
        }

        bUnPauseAll |= (status == STATUS_PAUSED); //start All
        bPauseAll |= (status == STATUS_ACTIVE); //pause all
        bPurge |= ( (status == STATUS_COMPLETE) || (status == STATUS_ERROR) || (status == STATUS_REMOVED) );

        uOverAllDownloadSpeed += dl.list.getDownloadSpeed();
        uOverAllUploadSpeed += dl.list.getUploadSpeed();
    }

    //Main view update
#ifdef QT_DEBUG
    qDebug() << m_seeding << m_downloading << m_complete << m_pause << m_error << m_waiting << m_remove;
#endif
    quint64 all = m_seeding + m_downloading + m_complete + m_pause + m_error + m_waiting + m_remove;
    m_mainListView->item(0)->setText( (all > 0) ? tr("All (%1)").arg(all) : tr("All"));
    m_mainListView->item(1)->setText( (m_seeding > 0) ? tr("Seeding (%1)").arg(m_seeding) : tr("Seeding"));
    m_mainListView->item(2)->setText( (m_downloading > 0) ? tr("Downloading (%1)").arg(m_downloading) : tr("Downloading"));
    m_mainListView->item(3)->setText( (m_complete > 0) ? tr("Complete (%1)").arg(m_complete) : tr("Complete"));
    m_mainListView->item(4)->setText( (m_pause > 0) ? tr("Pause (%1)").arg(m_pause) : tr("Pause"));
    m_mainListView->item(5)->setText( (m_error > 0) ? tr("Error (%1)").arg(m_error) : tr("Error"));
    m_mainListView->item(6)->setText( (m_waiting > 0) ? tr("Waiting (%1)").arg(m_waiting) : tr("Waiting"));
    m_mainListView->item(7)->setText( (m_remove > 0) ? tr("Remove (%1)").arg(m_remove) : tr("Remove"));

    //Action update
    ui->action_Start->setEnabled(bUnPause);
    ui->actionStart_all->setEnabled(bUnPauseAll);
    ui->action_Pause->setEnabled(bPause);
    ui->actionPause_all->setEnabled(bPauseAll);
    ui->action_Remove->setEnabled(bRemove);
    ui->actionPurge->setEnabled(bPurge);

    ui->actionForce_pause->setEnabled(bPause);
    ui->actionForce_pause_all->setEnabled(bPauseAll);
    ui->actionForce_remove->setEnabled(bRemove);

    m_DetailsTab->setTabPages(currentDownload);

    //update status bar
    statusBarEx->setStatusBar(uOverAllDownloadSpeed, uOverAllUploadSpeed);

    //Set current download's GID
    m_workThread.setCurrentGID(m_currentGID);
}

void Aria2cRemote:: setInitTreeWidgetItem(QTreeWidgetItemEx *item)
{
    item->setTextAlignment(0, Qt::AlignRight);
    item->setTextAlignment(1, Qt::AlignLeft);
    item->setTextAlignment(2, Qt::AlignRight);
    item->setTextAlignment(3, Qt::AlignHCenter);
    item->setTextAlignment(4, Qt::AlignHCenter);
    item->setTextAlignment(5, Qt::AlignRight);
    item->setTextAlignment(6, Qt::AlignRight);
    item->setTextAlignment(7, Qt::AlignRight);
    item->setTextAlignment(8, Qt::AlignRight);
    item->setTextAlignment(9, Qt::AlignRight);
    item->setTextAlignment(10, Qt::AlignRight);
}

void Aria2cRemote::setTreeWidgetItem(QTreeWidgetItemEx *item, XmlRPC& x)
{
    URI_STATUS status = x.getiStatus();
    XML_TYPE type = x.getType();
    QIcon icon;
    if (type == HTTP_FTP)
    {
        icon.addFile(":/icon/uri/ftp.png");
    } else if ( (type == TORRENT) || (type == MAGNETLINK) )
    {
        icon.addFile(":/icon/uri/bittorrent.png");
    } else
    {
        icon.addFile(":/icon/uri/metalink.png");
    }
    item->setIcon(0, icon);
    item->setText(0, QString::number(x.getGID())); //GID
    item->setData(0, Qt::UserRole, x.getGID());
    item->setText(1, x.getName()); //Name
    item->setText(2, util::ConvertNumberToSuffixString(x.getTotalLength())); //Size
    item->setData(2, Qt::UserRole, x.getTotalLength());

    if (status == STATUS_COMPLETE)
    {
        item->setText(3, "100%"); //Done
        item->setData(3, Qt::UserRole, (quint64)100);
    }
    else
    {
        item->setText(3, x.getsProcessPercent()); //Done
        item->setData(3, Qt::UserRole, x.getProcessPercent());
    }

    item->setText(4, x.getStatus()); //Status
    item->setText(5, x.getsSeeders()); //Seeds
    item->setData(5, Qt::UserRole, x.getsSeeders());

    item->setText(6, ""); //Peers
    item->setText(7, util::ConvertNumberToSuffixString(x.getDownloadSpeed()) + "/s"); //DownSpeed
    item->setData(7, Qt::UserRole, x.getDownloadSpeed());
    item->setText(8, util::ConvertNumberToSuffixString(x.getUploadSpeed()) + "/s"); //UpSpeed
    item->setData(8, Qt::UserRole, x.getUploadSpeed());
    item->setText(9, x.getsRatio()); //Ratio
    item->setData(9, Qt::UserRole, x.getRatio());
    item->setText(10, x.getsETA());
    item->setData(10, Qt::UserRole, x.getETA());
}

void Aria2cRemote::ListViewItemClicked(QTreeWidgetItem *item, int value)
{
    bool bUnPause = false;
    bool bUnPauseAll = false;
    bool bPause = false;
    bool bPauseAll = false;
    bool bRemove = false;
    bool bPurge = false;

    foreach(DOWNLOAD_LIST dl, m_downloadView)
    {
        URI_STATUS status = dl.list.getiStatus();
        if (item == dl.item)
        {
            bUnPause = (status == STATUS_PAUSED);
            bPause = (status == STATUS_ACTIVE);
            bRemove = true;
        }

        bUnPauseAll |= (status == STATUS_PAUSED); //start All
        bPauseAll |= (status == STATUS_ACTIVE); //pause all
        bPurge |= ( (status == STATUS_COMPLETE) || (status == STATUS_ERROR) || (status == STATUS_REMOVED) );
    }
    ui->action_Start->setEnabled(bUnPause);
    ui->actionStart_all->setEnabled(bUnPauseAll);
    ui->action_Pause->setEnabled(bPause);
    ui->actionPause_all->setEnabled(bPauseAll);
    ui->action_Remove->setEnabled(bRemove);
    ui->actionPurge->setEnabled(bPurge);

    ui->actionForce_pause->setEnabled(bPause);
    ui->actionForce_pause_all->setEnabled(bPauseAll);
    ui->actionForce_remove->setEnabled(bRemove);
}

void Aria2cRemote::mainListViewItemClicked(QListWidgetItem *item)
{
    ResponseXML(m_tellActive, m_tellStopped, m_tellWaiting);
}

void Aria2cRemote::ShowTransferDialog(QString sDescription)
{
    if (m_transfer)
    {
        m_transfer->done(0);
        delete m_transfer;
        m_transfer = NULL;
    }
    m_transfer = new Transfer(this);
    m_transfer->setTransferText(sDescription);
    m_transfer->exec();
}

void Aria2cRemote::HideTransferDialog()
{
    if (m_transfer)
    {
        m_transfer->done(0);
        delete m_transfer;
        m_transfer = NULL;
    }
}

void Aria2cRemote::on_actionOption_triggered()
{
    Aria2Options options(this);

    options.setConnection(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);
    if (options.exec() == QDialog::Accepted)
    {
         options.getConnection(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);
         util::SaveConnectionList(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);

         //set threads
         m_workThread.setConnection(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);
         m_requestThread.setConnection(m_host, m_user, m_password, m_port, m_proxyServer, m_proxyUser, m_proxyPassword, m_proxyPort);
    }
}

qint64 Aria2cRemote::getCurrentGIDFromListView()
{
    qint64 gid = -1;
    QTreeWidgetItem *currentItem = (QTreeWidgetItem *)m_listView->currentItem();
    foreach(DOWNLOAD_LIST dl, m_downloadView)
    {
        if ( (currentItem != 0) && (dl.item == currentItem) )
        {
            gid = dl.list.getGID();
            break;
        }
    }
    return gid;
}

void Aria2cRemote::on_action_Start_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    qint64 gid = getCurrentGIDFromListView();

    if (gid != -1)
    {
        vCurrentParam["gid"] = Variant(QString::number(gid));
    }

    d.addUnPause(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Start URI"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionStart_all_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    d.addUnPauseAll(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Start all URIs"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_action_Pause_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    qint64 gid = getCurrentGIDFromListView();

    if (gid != -1)
    {
        vCurrentParam["gid"] = Variant(QString::number(gid));
    }

    d.addPause(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Pause URI"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionPause_all_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    d.addPauseAll(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Pause all URIs"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_action_Remove_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    qint64 gid = getCurrentGIDFromListView();

    if (gid != -1)
    {
        vCurrentParam["gid"] = Variant(QString::number(gid));
    }

    d.addRemove(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Remove URI"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionPurge_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    d.addPurge(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Purge completed/error/removed URIs"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionPower_off_Aria2c_triggered()
{
    if (QMessageBox::question(this, tr("Shutdown"), tr("Do you really shutdown to Aria2c?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        Download d;
        QMap<QString, Variant> vCurrentParam;

        d.addPowerOff(vCurrentParam);
        m_requestThread.addRequest(d);
        m_requestThread.setDescriptionText(tr("Shutdown Aria2c"));
        m_requestThread.wakeUp();
    }
}

void Aria2cRemote::on_actionForce_pause_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    qint64 gid = getCurrentGIDFromListView();

    if (gid != -1)
    {
        vCurrentParam["gid"] = Variant(QString::number(gid));
    }

    d.addForcePause(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Force pause URI"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionForce_pause_all_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    d.addForcePauseAll(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Force pause all URIs"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionForce_remove_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    qint64 gid = getCurrentGIDFromListView();

    if (gid != -1)
    {
        vCurrentParam["gid"] = Variant(QString::number(gid));
    }

    d.addForceRemove(vCurrentParam);
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Force remove URI"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::on_actionForce_power_off_Aria2c_triggered()
{
    if (QMessageBox::question(this, tr("Force Shutdown"), tr("Do you really force shutdown to Aria2c?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        Download d;
        QMap<QString, Variant> vCurrentParam;

        d.addForcePowerOff(vCurrentParam);
        m_requestThread.addRequest(d);
        m_requestThread.setDescriptionText(tr("Force shutdown Aria2c"));
        m_requestThread.wakeUp();
    }
}

void Aria2cRemote::on_actionGlobal_Options_triggered()
{
    Download d;
    QMap<QString, Variant> vCurrentParam;

    d.addGetGlobalOptions(vCurrentParam);
    connect(&m_requestThread, SIGNAL(ResponseGetGlobalOptions(QVariant)), this, SLOT(GetGlobalOptions(QVariant)));
    m_requestThread.addRequest(d);
    m_requestThread.setDescriptionText(tr("Get global variables"));
    m_requestThread.wakeUp();
}

void Aria2cRemote::GetGlobalOptions(QVariant value)
{
    XmlRPC x;
    m_globalOptions = x.OptionToMap(value);

    disconnect(&m_requestThread, SIGNAL(ResponseGetGlobalOptions(QVariant)), this, SLOT(GetGlobalOptions(QVariant)));

    GlobalOptions g(this);
    g.SetGlobalOptions(m_globalOptions);
    if (g.exec() == QDialog::Accepted)
    {
        QMap<QString, Variant> m_newGlobalOptions = g.GetNewGlobalOptions();
        QMap<QString, Variant> vCurrentParam;

        bool bFoundChangeParams = false;
        for (size_t i = 0; i < sizeof(GlobalChangeableOption) / sizeof(GlobalChangeableOption[0]); ++i)
        {
            bool ok;
            if (m_globalOptions[GlobalChangeableOption[i]].toInt(&ok) != m_newGlobalOptions[GlobalChangeableOption[i]].toInt(&ok))
            {
                bFoundChangeParams = true;
                vCurrentParam[GlobalChangeableOption[i]] = m_newGlobalOptions[GlobalChangeableOption[i]].toString();
            }
        }

        if (bFoundChangeParams)
        {
            Download d;
            d.addChangeGlobalOptions(vCurrentParam);
            m_requestThread.addRequest(d);
            m_requestThread.setDescriptionText(tr("Change global variables"));
            m_requestThread.wakeUp();
        }
    }
}
