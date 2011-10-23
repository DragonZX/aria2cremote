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

#include "util.h"
#include <QSettings>
#include <QPixmap>
#include <QPixmapCache>
#include <QDir>
#include <QDomDocument>
#include <QSysInfo>

#if defined(Q_WS_WIN)
#  define _WIN32_IE 0x0500
#  include <qt_windows.h>
#  include <commctrl.h>
#  include <objbase.h>
#elif defined(Q_WS_MAC)
#  include <private/qt_cocoa_helpers_mac_p.h>
#endif

#if defined(Q_WS_WIN)

void util::SetToolBar()
{
    if (QSysInfo::windowsVersion() == QSysInfo::WV_VISTA)
    {
        if (QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS7)
        {

        }
    }
}
#endif

QString util::ConvertNumberToSuffixString(qint64 number)
{
    int iCounter = 0;
    QString sRet;
    double dNumber =(double)number;

    if (dNumber < 0) dNumber = -dNumber;

    while ( (dNumber > 1024.0) && (iCounter <= 3) )
    {
        dNumber /= 1024.0;
        iCounter++;
    }

    sRet = QString::number(dNumber, 'f', (iCounter == 0) ? (0) : (2));
    switch (iCounter)
    {
    case 0:
        sRet += (" B");
        break;
    case 1:
        sRet += (" KiB");
        break;
    case 2:
        sRet += (" MiB");
        break;
    case 3:
        sRet += (" GiB");
        break;
    default:
        sRet += (" TiB");
        break;
    }
    return sRet;
}

QIcon util::getWinIcon(QFileInfo &fileInfo, QString &sDesc)
{
    QIcon retIcon;
    const QString fileExtension = QLatin1Char('.') + fileInfo.suffix().toUpper();

    QString key;
    if (fileInfo.isFile() && !fileInfo.isExecutable() && !fileInfo.isSymLink())
        key = QLatin1String("qt_") + fileExtension;

    QPixmap pixmap;
    if (!key.isEmpty()) {
        QPixmapCache::find(key, pixmap);
    }

    if (!pixmap.isNull()) {
        retIcon.addPixmap(pixmap);
        if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
            retIcon.addPixmap(pixmap);
        return retIcon;
    }

    SHFILEINFO info;
    unsigned long val = 0;

    //Get the small icon
#ifndef Q_OS_WINCE
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_TYPENAME);
#else
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
#endif
    if (val) {
        if (fileInfo.isDir() && !fileInfo.isRoot()) {
            //using the unique icon index provided by windows save us from duplicate keys
            key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
            QPixmapCache::find(key, pixmap);
            if (!pixmap.isNull()) {
                retIcon.addPixmap(pixmap);
                if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
                    retIcon.addPixmap(pixmap);
                DestroyIcon(info.hIcon);
                return retIcon;
            }
        }
        if (pixmap.isNull()) {
#ifndef Q_OS_WINCE
            pixmap = QPixmap::fromWinHICON(info.hIcon);
#else
            pixmap = QPixmap::fromWinHICON(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
#endif
            if (!pixmap.isNull()) {
                retIcon.addPixmap(pixmap);
                if (!key.isEmpty())
                    QPixmapCache::insert(key, pixmap);
            }
            else {
              qWarning("QFileIconProviderPrivate::getWinIcon() no small icon found");
            }
        }
        DestroyIcon(info.hIcon);
    }

    //Get the big icon
#ifndef Q_OS_WINCE
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES|SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_TYPENAME);
#else
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
#endif
    if (val) {
        if (fileInfo.isDir() && !fileInfo.isRoot()) {
            //using the unique icon index provided by windows save us from duplicate keys
            key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
        }
#ifndef Q_OS_WINCE
        pixmap = QPixmap::fromWinHICON(info.hIcon);
#else
        pixmap = QPixmap::fromWinHICON(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
#endif
        sDesc = QString::fromWCharArray(info.szTypeName);
        if (!pixmap.isNull()) {
            retIcon.addPixmap(pixmap);
            if (!key.isEmpty())
                QPixmapCache::insert(key + QLatin1Char('l'), pixmap);
        }
        else {
            qWarning("QFileIconProviderPrivate::getWinIcon() no large icon found");
        }
        DestroyIcon(info.hIcon);
    }
    return retIcon;
}

bool util::ReadTorrentFile(QString sTorrentfile, MetaInfo& metainfo)
{
    if (sTorrentfile.isEmpty()) {
        return false;
    }

    QFile torrent(sTorrentfile);
    if (!torrent.open(QFile::ReadOnly) || !metainfo.parse(torrent.readAll())) {
        return false;
    }
    return true;
}

QByteArray key("axGmYg__ppdVx0iUEFJ__nd8cCC_3okgFBOeF0uujCy_st1_fcyWRRf2C7m1hVkQ");

void swap_RC4( unsigned int *S, unsigned int i, unsigned int j)
{
  unsigned int tmp = S[i];
  S[i] = S[j];
  S[j] = tmp;

  return;
}

QByteArray util::RC4(QByteArray data)
{
  QByteArray Ret;
  unsigned int i, j;
  unsigned int S[256];

  /** HIDDEN PROTOTYPE */
  void swap_RC4( unsigned int *, unsigned int, unsigned int );

  for( i = 0; i < 256; i++ )
          S[i] = i;

  j=0;
  for( i = 0; i < 256; i++ )
  {
     j = (j + S[i] + key[i % key.size()]) % 256;
     swap_RC4( S, i, j );
  }

  i=0; j=0;
  size_t iSize = data.size();
  for( size_t l = 0; l < iSize; l++)
  {
    i = (i + 1) % 256;
    j = (j + S[i]) % 256;
    swap_RC4( S, i, j );
    Ret += S[(S[i] + S[j]) % 256] ^ data[l];
  }
  return Ret;
}

void util::deletePrePostSpace(QString &str)
{
    //pre space
    while ( (str.size() > 0) && (str.at(0) == QChar(' ')) )
        str.remove(0, 1);

    //post space
    while ( (str.size() > 0) && (str.at(str.size() - 1) == QChar(' ')) )
        str.remove(str.size() - 1, 1);
}

void util::LoadConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort)
{
    QDomDocument doc("Aria2cRemoteConfiguration");
    QFile file(util::getHomePath() + "configuration.xml");
    if (file.open(QIODevice::ReadOnly))
    {
        QString errorMsg;
        int errorline;
        int errorColumn;
        if (doc.setContent(&file, &errorMsg, &errorline, &errorColumn))
        {
            QDomElement root = doc.documentElement();
            if (root.tagName() == "Connections")
            {

                QDomNode n = root.firstChild();
                while(!n.isNull())
                {
                    QDomElement element = n.toElement(); // try to convert the node to an element.
                    if(!element.isNull())
                    {
                        password = QString( RC4( QByteArray::fromBase64( element.attribute("Password", "").toAscii() ) ) );
                        user = QString( QByteArray::fromPercentEncoding(element.attribute("User", "").toAscii()) );
                        host = QString( QByteArray::fromPercentEncoding(element.attribute("Host", "localhost").toAscii()) );
                        port = element.attribute("Port", "6800").toInt();
                        proxyPassword = QString( RC4( QByteArray::fromBase64( element.attribute("ProxyPassword", "").toAscii() ) ) );
                        proxyUser = QString( QByteArray::fromPercentEncoding(element.attribute("ProxyUser", "").toAscii()) );
                        proxyServer = QString( QByteArray::fromPercentEncoding(element.attribute("ProxyServer", "").toAscii()) );
                        proxyPort = element.attribute("ProxyPort", "8080").toInt();
                    }
                    n = n.nextSibling();
                }
            }
        }
        file.close();
    }
}

void util::SaveConnectionList(QString &host, QString &user, QString &password, int &port, QString &proxyServer, QString &proxyUser, QString &proxyPassword, int &proxyPort)
{
    QDomDocument doc("Aria2cRemoteConfiguration");
    QDomElement root = doc.createElement("Connections");
    doc.appendChild(root);

    QDomElement elem = doc.createElement("Connection");

    elem.setAttribute("Password", QString( RC4(password.toAscii()).toBase64()) );
    elem.setAttribute("User", QString( user.toAscii().toPercentEncoding() ) );
    elem.setAttribute("Host", QString( host.toAscii().toPercentEncoding() ) );
    elem.setAttribute("Port", QString::number(port));
    if ( (proxyPassword.size() > 0) && (proxyUser.size() > 0) && (proxyServer.size() > 0))
    {
        elem.setAttribute("ProxyPassword", QString( RC4(proxyPassword.toAscii()).toBase64()) );
        elem.setAttribute("ProxyUser", QString( proxyUser.toAscii().toPercentEncoding() ) );
        elem.setAttribute("ProxyServer", QString( proxyServer.toAscii().toPercentEncoding() ) );
        elem.setAttribute("ProxyPort", QString::number(proxyPort));
    }

    root.appendChild(elem);

    QFile file(util::getHomePath() + "configuration.xml");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toByteArray());
        file.close();
    }
}

QString util::getHomePath()
{
    QString sRet(QDir::homePath() + QDir::separator());

    sRet = QDir::toNativeSeparators(sRet);
#if defined(Q_WS_WIN)
    sRet += (QString("Application Data") + QDir::separator());
#endif
#if defined(Q_WS_LINUX)
     sRet += (QString(".config") + QDir::separator());
#endif

    sRet += (QString("Aria2c Remote Control") + QDir::separator());
    return sRet;
}

QByteArray util::gzipDecompress(const QByteArray& compressedData)
{
    QByteArray uncompressed;
    const quint32 OUTBUF_LENGTH = 1024 * 16;
    quint8 outbuf[OUTBUF_LENGTH];
    z_stream cmpr_stream;

    cmpr_stream.zalloc = Z_NULL;
    cmpr_stream.zfree = Z_NULL;
    cmpr_stream.opaque = Z_NULL;
    cmpr_stream.avail_in = 0;
    cmpr_stream.next_in = Z_NULL;

    // initalize z_stream with gzip/zlib format auto detection enabled.
    if (inflateInit2(&cmpr_stream, 47) != Z_OK)
    {
        return QByteArray();
    }

    cmpr_stream.avail_in = compressedData.size();
    cmpr_stream.next_in = (unsigned char *)(compressedData.data());

    forever
    {
      cmpr_stream.avail_out = OUTBUF_LENGTH;
      cmpr_stream.next_out = outbuf;

      int ret = inflate(&cmpr_stream, Z_NO_FLUSH);
      if ( (ret != Z_OK) && (ret != Z_STREAM_END) )
      {
          return QByteArray();
      }

      uncompressed.append( QByteArray::fromRawData( (char*)outbuf, OUTBUF_LENGTH - cmpr_stream.avail_out) );
      if(cmpr_stream.avail_out > 0) {
        break;
      }
    }

    if (inflateEnd(&cmpr_stream) != Z_OK)
    {
        return QByteArray();
    }

    return uncompressed;
}
