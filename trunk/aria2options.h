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

#ifndef ARIA2OPTIONS_H
#define ARIA2OPTIONS_H

#include <QDialog>
#include <QDomDocument>
#include "util.h"

using namespace util;
namespace Ui {
    class Aria2Options;
}

class Aria2Options : public QDialog {
    Q_OBJECT
public:
    Aria2Options(QWidget *parent = 0);
    ~Aria2Options();
    void LoadConnectionList();
    void SaveConnectionList();
    void getConnection(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort, bool &enableProxy)
    {
        host = m_host;
        user = m_user;
        password = m_password;
        port = m_port;

        proxyServer = m_proxyServer;
        proxyUser = m_proxyUser;
        proxyPassword = m_proxyPassword;
        proxyPort = m_proxyPort;
        enableProxy = m_enableProxy;
    }

    void setConnection(const QString &host, const QString &user, const QString &password, const int &port, const QString &proxyServer, const QString &proxyUser, const QString &proxyPassword, const int &proxyPort, const bool &enableProxy);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Aria2Options *ui;
    QString m_host;
    QString m_user;
    QString m_password;
    int m_port;
    QString m_proxyServer;
    QString m_proxyUser;
    QString m_proxyPassword;
    int m_proxyPort;
    bool m_enableProxy;

private slots:
    void on_checkBox_Enable_Proxy_stateChanged(int );
    void on_buttonBox_accepted();
};

#endif // ARIA2OPTIONS_H
