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
#include "gzip/zlib.h"

namespace util
{
    //Aria2c versions
    const quint32 ARIA2C_VERSION_NONE = 0x00000000;
    const quint32 ARIA2C_VERSION_181  = 0x00010801;
    const quint32 ARIA2C_VERSION_182  = 0x00010802;
    const quint32 ARIA2C_VERSION_183  = 0x00010803;
    const quint32 ARIA2C_VERSION_190  = 0x00010900;
    const quint32 ARIA2C_VERSION_191  = 0x00010901;
    const quint32 ARIA2C_VERSION_192  = 0x00010902;
    const quint32 ARIA2C_VERSION_193  = 0x00010903;
    const quint32 ARIA2C_VERSION_194  = 0x00010904;
    const quint32 ARIA2C_VERSION_195  = 0x00010905;
    const quint32 ARIA2C_VERSION_1100 = 0x00010A00;
    const quint32 ARIA2C_VERSION_1101 = 0x00010A01;
    const quint32 ARIA2C_VERSION_1102 = 0x00010A02;
    const quint32 ARIA2C_VERSION_1103 = 0x00010A03;
    const quint32 ARIA2C_VERSION_1104 = 0x00010A04;
    const quint32 ARIA2C_VERSION_1105 = 0x00010A05;
    const quint32 ARIA2C_VERSION_1106 = 0x00010A06;
    const quint32 ARIA2C_VERSION_1107 = 0x00010A07;
    const quint32 ARIA2C_VERSION_1108 = 0x00010A08;
    const quint32 ARIA2C_VERSION_1109 = 0x00010A09;
    const quint32 ARIA2C_VERSION_1110 = 0x00010B00;
    const quint32 ARIA2C_VERSION_1111 = 0x00010B01;
    const quint32 ARIA2C_VERSION_1112 = 0x00010B02;
    const quint32 ARIA2C_VERSION_1120 = 0x00010C00;
    const quint32 ARIA2C_VERSION_1121 = 0x00010C01;
    const quint32 ARIA2C_VERSION_1130 = 0x00010D00;
    const quint32 ARIA2C_VERSION_MASK = 0x00FFFFFF;

    //Aria2c Features
    const quint32 ARIA2C_FEATURES_NONE           = 0x00000000;
    const quint32 ARIA2C_FEATURES_ASYNCDNS       = 0x01000000;
    const quint32 ARIA2C_FEATURES_BITTORRENT     = 0x02000000;
    const quint32 ARIA2C_FEATURES_FIREFOX3COOKIE = 0x04000000;
    const quint32 ARIA2C_FEATURES_GZIP           = 0x08000000;
    const quint32 ARIA2C_FEATURES_HTTPS          = 0x10000000;
    const quint32 ARIA2C_FEATURES_MESSAGEDIGEST  = 0x20000000;
    const quint32 ARIA2C_FEATURES_METALINK       = 0x40000000;
    const quint32 ARIA2C_FEATURES_XMLRPC         = 0x80000000;
    const quint32 ARIA2C_FEATURES_ALL            = 0xFF000000;

    QString ConvertNumberToSuffixString(qint64 number);
    QIcon getWinIcon(QFileInfo &fileInfo, QString &sDesc);
    bool ReadTorrentFile(QString sTorrentfile, MetaInfo& metainfo);
    void SetToolBar();
    QByteArray RC4(QByteArray data);
    void deletePrePostSpace(QString &str);

    void LoadConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort, bool &enableProxy);
    void SaveConnectionList(const QString &host, const QString &user, const QString &password, const int &port, const QString &proxyServer, const QString &proxyUser, const QString &proxyPassword, const int &proxyPort, const bool &enableProxy);

    QString getHomePath();

    QByteArray gzipDecompress(const QByteArray& compressedData);
};

#endif // UTIL_H
