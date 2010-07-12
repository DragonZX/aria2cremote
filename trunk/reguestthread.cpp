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

#include "reguestthread.h"

reguestThread::reguestThread():
        m_exit(false),
        m_host("localhost"),
        m_user(""),
        m_password(""),
        m_port(6800),
        m_proxyServer(""),
        m_proxyUser(""),
        m_proxyPassword(""),
        m_proxyPort(8080)
{
    client.setHost(m_host, m_port, "/rpc");
    client.setUser(m_user, m_password);

    connect( &client, SIGNAL(done( int, qint64, int, QVariant )), this, SLOT(processReturnValue( int, qint64, int, QVariant )) );
    connect( &client, SIGNAL(failed( int, int, QString )), this, SLOT(processFault( int, int, QString )) );
}

void reguestThread::setConnection(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort)
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

void reguestThread::addRequest(Download &d)
{
    m_mutex.lock();
        m_request.enqueue(d);
    m_mutex.unlock();
}

void reguestThread::addRequest(QList<Download> &d)
{
    m_mutex.lock();
    foreach(Download down, d)
    {
        m_request.enqueue(down);
    }
    m_mutex.unlock();
}

void reguestThread::run()
{
    forever
    {
        m_mutex.lock();
        m_waitCondition.wait(&m_mutex);
        QList<Variant> RequestCall;
        QVariantList v;
        int iRequestTypes = 0;

        while (!m_request.isEmpty())
        {
            Download d = m_request.dequeue();
            d.toXML();
            iRequestTypes |= d.getType();
            v += d.getParamList();
        }
        if (v.size() != 0)
        {
            if (m_sDescription.size() != 0)
            {
                emit ShowTransferDialog(m_sDescription);
            }

            RequestCall << Variant(v);
            //send new request to Aria2c
            if (RequestCall.size() != 0)
            {
                m_syncronize->lock();
                client.request(RequestCall,"system.multicall", iRequestTypes);
                m_syncronize->unlock();
            }
        }

        m_mutex.unlock();
        if (m_exit)
            break;
    }
}

void reguestThread::processReturnValue( int iTypes, qint64 iGID, int requestId, QVariant value )
{
    QVariantList vl;
    QString errorMessage;

    //transfer dialog done
    emit HideTransferDialog();

    vl << value;
    QFile kiiras(QApplication::applicationDirPath() + "/aria_response.xml");
    if (kiiras.open(QIODevice::ReadWrite))
    {
        kiiras.seek(kiiras.size());
        kiiras.write(xmlrpc::Response(vl).composeResponse(1));
        kiiras.close();
    }

    if (iTypes & GET_GLOBAL_OPTIONS)
    {
        emit ResponseGetGlobalOptions(value);
    }
    else if (iTypes & GET_LOCAL_OPTIONS)
    {
        emit ResponseGetLocalOptions(value);
    }
}

void reguestThread::processFault( int requestId, int errorCode, QString errorString )
{
    emit HideTransferDialog();
    emit processFaultToUI(requestId, errorCode, errorString );
}