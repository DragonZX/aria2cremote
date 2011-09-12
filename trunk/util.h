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
    //Aria2c versions
    const quint32 ARIA2C_VERSION_181 = 0x010801;
    const quint32 ARIA2C_VERSION_182 = 0x010802;
    const quint32 ARIA2C_VERSION_183 = 0x010803;
    const quint32 ARIA2C_VERSION_190 = 0x010900;
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

    QString ConvertNumberToSuffixString(qint64 number);
    QIcon getWinIcon(QFileInfo &fileInfo, QString &sDesc);
    bool ReadTorrentFile(QString sTorrentfile, MetaInfo& metainfo);
    void SetToolBar();
    QByteArray RC4(QByteArray data);
    void deletePrePostSpace(QString &str);

    void LoadConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort);
    void SaveConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort);

	QString getHomePath();
};

#endif // UTIL_H
