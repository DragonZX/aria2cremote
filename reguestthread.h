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
 
#ifndef REGUESTTHREAD_H
#define REGUESTTHREAD_H

#include <QtGui/QApplication>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include "download.h"
#include "transfer.h"

class reguestThread : public QThread
{
    Q_OBJECT
public:
    reguestThread();
    void addRequest(Download &d);
    void addRequest(QList<Download> &d);
    void run();
    void stop() { m_exit = true; }
    void setRequestResponseSynchronize(QMutex *Syncronize) {m_syncronize = Syncronize; }
    void setDescriptionText(QString s) {m_sDescription = s; }
    void wakeUp() {m_waitCondition.wakeOne();}
    void setConnection(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort);

signals:
    void Response(int iValue, QString sError);
    void ResponseGetGlobalOptions(QVariant params);
    void ResponseGetLocalOptions(QVariant params);
    void ResponseVersionInfo(QVariant params);
    void processFaultToUI( int requestId, int errorCode, QString errorString );
    void ShowTransferDialog(QString sDescription);
    void HideTransferDialog();

public slots:
    void processReturnValue( int iTypes, qint64 iGID, int requestId, QVariant value );
    void processFault( int requestId, int errorCode, QString errorString );
private:
    QQueue<Download> m_request;
    QMutex m_mutex;
    QMutex *m_syncronize;
    QWaitCondition m_waitCondition;
    volatile bool m_exit;
    xmlrpc::Client client;
    QString m_sDescription;

    QString m_host;
    QString m_user;
    QString m_password;
    int m_port;
    QString m_proxyServer;
    QString m_proxyUser;
    QString m_proxyPassword;
    int m_proxyPort;
};

#endif // REGUESTTHREAD_H
