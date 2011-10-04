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

#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>
#include <QMap>
#include "xmlrpc.h"
#include "download.h"

typedef QMap<qint64, xmlrpc::XmlRPC> XML_RPC_RESPONSE_MAP;

using namespace xmlrpc;

class workThread : public QThread
{
    Q_OBJECT
public:
    workThread();
    void run();
    void stop();
    void SetSleep(quint32 sleep) { m_sleep = sleep;}
    void setRequestResponseSynchronize(QMutex *Syncronize) {m_syncronize = Syncronize; }
    void setCurrentGID(int gid) { m_currentGID = gid; }
    void setConnection(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort);
    void SetGZipEnabled() { client.setGZipEnabled(); }
    void wakeUp() {condition.wakeOne();}

signals:
    void Response(XML_RPC_RESPONSE_MAP tellActive, XML_RPC_RESPONSE_MAP tellStopped, XML_RPC_RESPONSE_MAP tellWaiting);
    void processFaultToUI( int requestId, int errorCode, QString errorString );

public slots:
    void processReturnValue( int iTypes, qint64 iGID, int requestId, QVariant value );
    void processFault( int requestId, int errorCode, QString errorString );

private:
    volatile bool m_exit;
    quint32 m_sleep;
    QMutex *m_syncronize;
    xmlrpc::Client client;

    QString m_host;
    QString m_user;
    QString m_password;
    int m_port;
    QString m_proxyServer;
    QString m_proxyUser;
    QString m_proxyPassword;
    int m_proxyPort;

    qint64 m_currentGID;
    QWaitCondition condition;
    QMutex conditionMutex;
};

#endif // WORKTHREAD_H
