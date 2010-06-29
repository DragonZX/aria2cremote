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
 
#ifndef UTIL_H
#define UTIL_H

#include <QtGui/QApplication>
#include <QString>
#include <QIcon>
#include <QFileInfo>
#include <QByteArray>
#include <QDomDocument>
#include "metainfo.h"

namespace util
{
    QString ConvertNumberToSuffixString(qint64 number);
    QIcon getWinIcon(QFileInfo &fileInfo, QString &sDesc);
    bool ReadTorrentFile(QString sTorrentfile, MetaInfo& metainfo);
    void SetToolBar();
    QByteArray RC4(QByteArray data);
    void deletePrePostSpace(QString &str);

    void LoadConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort);
    void SaveConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort);


};

#endif // UTIL_H
