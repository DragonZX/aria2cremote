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

#include "tabwidget.h"
#include "ui_tabwidget.h"
#include "util.h"

DetailsTabView::DetailsTabView(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget_Details),
    m_progress(NULL),
    m_pieces(NULL),
    m_availability(NULL),
    m_currentType(UNKNOWN)
{
    ui->setupUi(this);
    m_progress = new bargraph(this);
    m_pieces = new bargraph(this);
    m_availability = new bargraph(this);
    m_availability->setEnabled(false);
    m_availability->setBackColor(Qt::red);

    ui->horizontalLayout_Progress->insertWidget(1, m_progress);
    m_progress->setVisible(true);

    ui->horizontalLayout_Pieces->insertWidget(1, m_pieces);
    m_pieces->setVisible(true);

    ui->horizontalLayout_Availability->insertWidget(1, m_availability);
    m_availability->setVisible(true);
}

DetailsTabView::~DetailsTabView()
{
    delete ui;
}

void DetailsTabView::changeEvent(QEvent *e)
{
    QTabWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DetailsTabView::setTabPages(xmlrpc::XmlRPC &dw)
{
    setTabPageGeneral(dw);
    setTabPagePeers(dw);
    setTabPageFiles(dw);
}

void DetailsTabView::setTabPageGeneral(xmlrpc::XmlRPC &dw)
{
    //Bars
    std::vector<quint64> vAvailability;

    m_progress->setDoneBar(dw.getTotalLength(), dw.getCompletedLength());

    if (dw.getiStatus() == STATUS_COMPLETE)
    {
        ui->label_ProgressPercent_text->setText("100%"); //Done
    }
    else
    {
        ui->label_ProgressPercent_text->setText(dw.getsProcessPercent());//Done
    }

    std::vector<quint64> vPieces = dw.getBitfield();
    double piecesPercent = m_pieces->setPiecesBar(vPieces);
    ui->label_DownloadPercent_Text->setText(QString::number(piecesPercent * 100.0, 'f', 2) + "%");

    bool isTorrent = (dw.getType() == TORRENT) || (dw.getType() == MAGNETLINK);
    bool bSeeding = (dw.getCompletedLength() != 0) && ( dw.getCompletedLength() == dw.getTotalLength() );
    size_t iPeerCount = dw.getBitfield().size();
    for (size_t i = 0; i < iPeerCount; ++i)
    {
        vAvailability.push_back( (isTorrent && bSeeding) ? 1 : 0);
    }

    foreach (XMLRPC_PEER_LIST peer, dw.getPeerList())
    {
        for (size_t i = 0; (i < iPeerCount) && (i < peer.bitfield.size()); ++i)
        {
            vAvailability[i] += peer.bitfield[i];
        }
    }
    double availabilityPercent = m_availability->setAvailabilityBar(vAvailability);
    m_availability->setEnabled(isTorrent);
    ui->label_AvailabilityPercent->setText(QString::number(availabilityPercent, 'f', 2));


    //Tranfer section
    ui->label_Status->setText(dw.getStatus());
    ui->label_Downloaded->setText(util::ConvertNumberToSuffixString(dw.getCompletedLength()));
    ui->label_DownloadSpeed->setText(util::ConvertNumberToSuffixString(dw.getDownloadSpeed()) + "/s");
    ui->label_Seeds->setText(dw.getsSeeders());
    ui->label_Tracker->setText(dw.getsTracker());

    ui->label_Error->setText(dw.getErrorCode());
    ui->label_Uploaded->setText(util::ConvertNumberToSuffixString(dw.getUploadLength()));
    ui->label_UploadSpeed->setText(util::ConvertNumberToSuffixString(dw.getUploadSpeed()) + "/s");
    ui->label_Peers->setText("");
    ui->label_ETA->setText(dw.getsETA());

    ui->label_SharedRatio->setText(dw.getsRatio());

    //Information section
    ui->label_Name->setText(dw.getName());
    ui->label_TotalSize->setText(util::ConvertNumberToSuffixString(dw.getTotalLength()));
    ui->label_Hash->setText(dw.getInfoHash());
    ui->label_Pieces->setText(QString::number(dw.getNumPieces()));
    ui->label_Comment->setText(dw.getComment());
    ui->label_Path->setText(dw.getDir());
}

void DetailsTabView::setTabPagePeers(xmlrpc::XmlRPC &dw)
{
    XML_TYPE type = dw.getType();
    bool isTorrent = (type == TORRENT) || (type == MAGNETLINK);

    ui->label_Availability_Text->setEnabled(isTorrent);
    ui->label_AvailabilityPercent->setEnabled(isTorrent);
    m_availability->setEnabled(isTorrent);

    if (type != m_currentType)
    {
        setTabText(1, isTorrent ? tr("Peers") : tr("Servers"));

        //erase all items
        foreach (QString iIndex, m_tabViewPeersServers.keys() )
        {
            SERVER_PEER_LIST list;
            list.item = 0;
            QTreeWidgetItemEx *item = m_tabViewPeersServers.value(iIndex, list).item;
            if (item != 0)
            {
                if (item->childCount() != 0)
                {
                    foreach(QTreeWidgetItem *itemChild, item->takeChildren())
                    {
                        delete itemChild;
                    }
                }
                ui->treeWidget_Peers_Servers->takeTopLevelItem(ui->treeWidget_Peers_Servers->indexOfTopLevelItem( item ));
                delete item;
            }
            m_tabViewPeersServers.erase(m_tabViewPeersServers.find(iIndex));
        }
        m_tabViewPeersServers.clear();
        m_currentType = type;

        QStringList headers;
        if (isTorrent)
        {
            //create peers columns
            headers << tr("IP Address")  << tr("Down speed") << tr("Up speed");
            ui->treeWidget_Peers_Servers->setColumnCount(headers.size());
            ui->treeWidget_Peers_Servers->setHeaderLabels(headers);
            ui->treeWidget_Peers_Servers->setRootIsDecorated(false);
        }
        else if (type != UNKNOWN)
        {
            //create servers column
            headers << tr("Uri") << tr("Current URI") << tr("Size");
            ui->treeWidget_Peers_Servers->setColumnCount(headers.size());
            ui->treeWidget_Peers_Servers->setHeaderLabels(headers);
            ui->treeWidget_Peers_Servers->setRootIsDecorated(true);
        }
    }

    QMap<QString, SERVER_PEER_LIST> d;
    if (isTorrent)
    {
        //peer
        QList<QTreeWidgetItemEx*> newItemsPeer;
        foreach (XMLRPC_PEER_LIST peer, dw.getPeerList())
        {
            SERVER_PEER_LIST list;
            QString ipAddress = peer.ip;
            if (!m_tabViewPeersServers.contains(ipAddress))
            {
                list.item = new QTreeWidgetItemEx;
                newItemsPeer << list.item;
            }
            else
            {
                list = m_tabViewPeersServers.find(ipAddress).value();
            }

            list.item->setText(0, ipAddress);
            list.item->setText(1, util::ConvertNumberToSuffixString(peer.download) + "/s");
            list.item->setText(2, util::ConvertNumberToSuffixString(peer.uploadSpeed) + "/s");
            d.insert(ipAddress, list);
        }
        foreach(QTreeWidgetItemEx * itemEx, newItemsPeer)
        {
            ui->treeWidget_Peers_Servers->addTopLevelItem( itemEx );
        }

        //erase not visibled peer
        foreach (QString ipAddress, m_tabViewPeersServers.keys() )
        {
            if (!d.contains(ipAddress))
            {
                SERVER_PEER_LIST list;
                list.item = 0;
                QTreeWidgetItemEx *item = m_tabViewPeersServers.value(ipAddress, list).item;
                if (item != 0)
                {
                    ui->treeWidget_Peers_Servers->takeTopLevelItem(ui->treeWidget_Peers_Servers->indexOfTopLevelItem( item ));
                    delete item;
                }
                m_tabViewPeersServers.erase(m_tabViewPeersServers.find(ipAddress));
            }
        }
        m_tabViewPeersServers.clear();
        m_tabViewPeersServers = d;
    }
    else if (type != UNKNOWN)
    {
        //Server
        QList<QTreeWidgetItemEx*> newItemsServer;
        foreach (XMLRPC_SERVER_LIST Server, dw.getServerList())
        {
            SERVER_PEER_LIST list;
            QString ServerIndex = QString::number(Server.index);
            if (!m_tabViewPeersServers.contains(ServerIndex))
            {
                list.item = new QTreeWidgetItemEx;
                newItemsServer << list.item;
            }
            else
            {
                list = m_tabViewPeersServers.find(ServerIndex).value();
            }

            list.item->setText(0, ServerIndex);
            if(list.item->childCount() == 0)
            {
                foreach (XMLRPC_SERVER s, Server.servers)
                {
                    QTreeWidgetItemEx *child = new QTreeWidgetItemEx;

                    child->setText(0, s.uri);
                    child->setText(1, s.currentUri);
                    child->setText(2, util::ConvertNumberToSuffixString(s.downloadSpeed) + "/s");
                    list.item->addChild(child);
                }
            }

            d.insert(ServerIndex, list);
        }
        foreach(QTreeWidgetItemEx * itemEx, newItemsServer)
        {
            ui->treeWidget_Peers_Servers->addTopLevelItem( itemEx );
        }

        //erase not visibled peer
        foreach (QString ServerURI, m_tabViewPeersServers.keys() )
        {
            if (!d.contains(ServerURI))
            {
                SERVER_PEER_LIST list;
                list.item = 0;
                QTreeWidgetItemEx *item = m_tabViewPeersServers.value(ServerURI, list).item;
                if (item != 0)
                {
                    ui->treeWidget_Peers_Servers->takeTopLevelItem(ui->treeWidget_Peers_Servers->indexOfTopLevelItem( item ));
                    delete item;
                }
                m_tabViewPeersServers.erase(m_tabViewPeersServers.find(ServerURI));
            }
        }
        m_tabViewPeersServers.clear();
        m_tabViewPeersServers = d;
    }
}

void DetailsTabView::setTabPageFiles(xmlrpc::XmlRPC &dw)
{
    QList<XMLRPC_FILES> files = dw.getFiles();
    QMap<QString, FILES_LIST> d;
    QList<QTreeWidgetItemEx*> newItemsFile;

    foreach (XMLRPC_FILES f, files)
    {
        FILES_LIST list;
        QString FileIndex = f.path;
        if (!m_tabViewFiles.contains(FileIndex))
        {
            list.item = new QTreeWidgetItemEx;
            newItemsFile << list.item;
        }
        else
        {
            list = m_tabViewFiles.find(FileIndex).value();
        }

        QFileInfo fi(QFileInfo(f.path).fileName());
        QString sDesc;
        QIcon icon = util::getWinIcon(fi, sDesc);

        list.item->setText(0, f.path);
        list.item->setText(1, util::ConvertNumberToSuffixString(f.length));
        list.item->setText(2, sDesc);
        list.item->setIcon(2, icon);

        d.insert(FileIndex, list);
    }
    foreach(QTreeWidgetItemEx * itemEx, newItemsFile)
    {
        ui->treeWidget_files->addTopLevelItem( itemEx );
    }

    //erase not visibled peer
    foreach(QString FileIndex, m_tabViewFiles.keys() )
    {
        if (!d.contains(FileIndex))
        {
            FILES_LIST list;
            list.item = 0;
            QTreeWidgetItemEx *item = m_tabViewFiles.value(FileIndex, list).item;
            if (item != 0)
            {
                ui->treeWidget_files->takeTopLevelItem(ui->treeWidget_files->indexOfTopLevelItem( item ));
                delete item;
            }
            m_tabViewFiles.erase(m_tabViewFiles.find(FileIndex));
        }
    }
    m_tabViewFiles.clear();
    m_tabViewFiles = d;
}
