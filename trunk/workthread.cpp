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

#include "workthread.h"
#include <QtGui/QApplication>
#include <QMessageBox>
#include <QByteArray>

workThread::workThread() :
        m_exit(false),
        m_sleep(10000),
        m_host("localhost"),
        m_user(""),
        m_password(""),
        m_port(6800),
        m_proxyServer(""),
        m_proxyUser(""),
        m_proxyPassword(""),
        m_proxyPort(8080),
        m_currentGID(-1)

{
    client.setHost(m_host, m_port, "/rpc");
    client.setUser(m_user, m_password);

    connect( &client, SIGNAL(done( int, qint64, int, QVariant )), this, SLOT(processReturnValue( int, qint64, int, QVariant )) );
    connect( &client, SIGNAL(failed( int, int, QString )), this, SLOT(processFault( int, int, QString )) );
}

void workThread::setConnection(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort)
{
    m_host = host;
    m_user = user;
    m_password = password;
    m_port = port;

    m_proxyServer = proxyServer;
    m_proxyUser = proxyUser;
    m_proxyPassword = proxyPassword;
    m_proxyPort = proxyPort;

    client.setHost(m_host, m_port, "/rpc");
    client.setUser(m_user, m_password);
    if ( (m_proxyServer.size() > 0) && (m_proxyUser.size() > 0) && (m_proxyPassword.size() > 0) )
    {
       client.setProxy(m_proxyServer, m_proxyPort, m_proxyUser, m_proxyPassword);
    }
}

void workThread::run()
{
    while(!m_exit)
    {
        //set request
        QVariantList v;

        QMap<QString, Variant> methodTellActive;
        QVariantList paramsTellActive;
        QMap<QString, Variant> methodTellStopped;
        QMap<QString, Variant> methodTellWaiting;
        QVariantList paramsStruct;

        paramsStruct << Variant(0) << Variant(LONG_LONG_MAX);

        methodTellActive["methodName"] = QString("aria2.tellActive");
        methodTellActive["params"] = paramsTellActive;

        methodTellStopped["methodName"] = QString("aria2.tellStopped");
        methodTellStopped["params"] = paramsStruct;

        methodTellWaiting["methodName"] = QString("aria2.tellWaiting");
        methodTellWaiting["params"] = paramsStruct;

        v += Variant(methodTellActive);
        v += Variant(methodTellStopped);
        v += Variant(methodTellWaiting);
        if (m_currentGID != -1)
        {
            QMap<QString, Variant> methodGetPeers;
            QVariantList paramsGetPeers;
            QMap<QString, Variant> methodGetServers;
            QVariantList paramsGetServers;

            methodGetPeers["methodName"] = QString("aria2.getPeers");
            paramsGetPeers << Variant(QString::number(m_currentGID));
            methodGetPeers["params"] = paramsGetPeers;

            methodGetServers["methodName"] = QString("aria2.getServers");
            paramsGetServers << Variant(QString::number(m_currentGID));
            methodGetServers["params"] = paramsGetServers;

            v += Variant(methodGetPeers);
            v += Variant(methodGetServers);
        }

        //response from Aria2c
        QList<Variant> multiCall;
        multiCall << Variant(v);

        m_syncronize->lock();
        client.request(multiCall, "system.multicall", 0, m_currentGID);
        m_syncronize->unlock();

        msleep(m_sleep);
    }
}

void workThread::stop()
{
    m_exit = true;
}

void workThread::processReturnValue( int iTypes, qint64 iGID, int requestId, QVariant value )
{
    QVariantList vl;
    QString errorMessage;

    vl << value;
    QByteArray array(xmlrpc::Response(vl).composeResponse());
    xmlrpc::Response re;

#ifdef XMLRPC_DEBUG
    QFile kiiras(util::getHomePath() + "aria_response.xml");
    if (kiiras.open(QIODevice::ReadWrite))
    {
        kiiras.seek(kiiras.size());
        kiiras.write(array);
        kiiras.close();
    }
#endif

    if (re.setContent(array, &errorMessage))
    {
        XML_RPC_RESPONSE_MAP responseTellActive;
        XML_RPC_RESPONSE_MAP responseTellStopped;
        XML_RPC_RESPONSE_MAP responseTellWaiting;
        QVariantList values = value.toList();

        QVariantList tmpList = values[0].toList();
        if (tmpList.size() > 0)
        {
            QVariantList itemtellActiveList(tmpList[0].toList());
            int itemtellActiveListSize = itemtellActiveList.size();
            for (int i = 0; i < itemtellActiveListSize; i++)
            {
                 QVariant &item(itemtellActiveList[i]);
                 if (Variant::Map == item.type())
                 {
                     xmlrpc::XmlRPC x;
                     x.ParseResponseXML(item);
                     qint64 gid = x.getGID();
                     responseTellActive[gid] = x;
                 }
            }
        }

        tmpList = values[1].toList();
        if (tmpList.size() > 0)
        {
            QVariantList itemtellStoppedList(tmpList[0].toList());
            int itemtellStoppedListSize = itemtellStoppedList.size();
            for (int i = 0; i < itemtellStoppedListSize; i++)
            {
                 QVariant &item(itemtellStoppedList[i]);
                 if (Variant::Map == item.type())
                 {
                     xmlrpc::XmlRPC x;
                     x.ParseResponseXML(item);
                     qint64 gid = x.getGID();
                     responseTellStopped[gid] = x;
                 }
            }
        }

        tmpList = values[2].toList();
        if (tmpList.size() > 0)
        {
            QVariantList itemtellWaitingList(tmpList[0].toList());
            int itemtellWaitingListSize = itemtellWaitingList.size();
            for (int i = 0; i < itemtellWaitingListSize; i++)
            {
                 QVariant &item(itemtellWaitingList[i]);
                 if (Variant::Map == item.type())
                 {
                     xmlrpc::XmlRPC x;
                     x.ParseResponseXML(item);
                     qint64 gid = x.getGID();
                     responseTellWaiting[gid] = x;
                 }
            }
        }

        if (values.size() == 5)
        {
            if (Variant::Map == values[3].type())
            {
                //there is no peer list
            }
            else
            {
                QVariantList itemtellPeersList(values[3].toList()[0].toList());
                int itemtellPeersListSize = itemtellPeersList.size();

                for (int i = 0; i < itemtellPeersListSize; i++)
                {
                     QVariant &item(itemtellPeersList[i]);
                     if (Variant::Map == item.type())
                     {
                         if (responseTellActive.contains(iGID))
                         {
                             responseTellActive[iGID].setPeers(item);
                         } else if (responseTellStopped.contains(iGID))
                         {
                             responseTellStopped[iGID].setPeers(item);
                         } else if (responseTellWaiting.contains(iGID))
                         {
                             responseTellWaiting[iGID].setPeers(item);
                         }
                     }
                }
            }

            if (Variant::Map == values[4].type())
            {
                //there is no server list
            }
            else
            {
                QVariantList itemtellServersList(values[4].toList()[0].toList());
                int itemtellServersListSize = itemtellServersList.size();
                for (int i = 0; i < itemtellServersListSize; i++)
                {
                     QVariant &item(itemtellServersList[i]);
                     if (Variant::Map == item.type())
                     {
                         if (responseTellActive.contains(iGID))
                         {
                             responseTellActive[iGID].setServers(item);
                         } else if (responseTellStopped.contains(iGID))
                         {
                             responseTellStopped[iGID].setServers(item);
                         } else if (responseTellWaiting.contains(iGID))
                         {
                             responseTellWaiting[iGID].setServers(item);
                         }
                     }
                }
            }
        }
        emit Response(responseTellActive, responseTellStopped, responseTellWaiting);
    }
}

void workThread::processFault( int requestId, int errorCode, QString errorString )
{
    if (!m_exit)
    {
        emit processFaultToUI(requestId, errorCode, errorString );
    }
}
