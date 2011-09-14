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

#include "server.h"
#include <QUrl>
#include <QDir>

Server::Server():
    m_AllProxyEnabled(false)
{
}

void Server::LoadServerList()
{
    QDomDocument doc("ServerConfiguration");
    QFile file(util::getHomePath() + "serverconfig.xml");
    if (file.open(QIODevice::ReadOnly))
    {
        QString errorMsg;
        int errorline;
        int errorColumn;
        if (doc.setContent(&file, &errorMsg, &errorline, &errorColumn))
        {
            QDomElement root = doc.documentElement();
            if (root.tagName() == "Serverlist")
            {
                m_serverList.clear();
                QDomNode n = root.firstChild();
                while(!n.isNull())
                {
                    QDomElement element = n.toElement(); // try to convert the node to an element.
                    if(!element.isNull())
                    {
                        if (element.nodeName() == "Server")
                        {
                            SERVER_ITEM s;

                            s.server = QString( QByteArray::fromPercentEncoding(element.attribute("Name", "").toAscii()) );
                            s.user = QString( QByteArray::fromPercentEncoding(element.attribute("User", "").toAscii()) );
                            s.password = QString( RC4( QByteArray::fromBase64( element.attribute("Password", "").toAscii() ) ) );
                            QString sType = element.attribute("Type", "");

                            if (sType == Server_Type_String[SERVER_ALL_PROXY])
                                s.type = SERVER_ALL_PROXY;
                            else if (sType == Server_Type_String[SERVER_HTTP_PROXY])
                                s.type = SERVER_HTTP_PROXY;
                            else if (sType == Server_Type_String[SERVER_HTTPS_PROXY])
                                s.type = SERVER_HTTPS_PROXY;
                            else if (sType == Server_Type_String[SERVER_FTP_PROXY])
                                s.type = SERVER_FTP_PROXY;
                            else if (sType == Server_Type_String[SERVER_HTTP_FTP])
                                s.type = SERVER_HTTP_FTP;

                            s.port = element.attribute("Port", "0").toInt();
                            m_serverList << s;
                        } else if (element.nodeName() == "Property")
                        {
                            m_AllProxyEnabled = element.attribute("AllProxyEnabled", "false").compare("true") == 0;
                        }
                    }
                    n = n.nextSibling();
                }
            }
        }
        file.close();
    }
}

void Server::SaveServerList()
{
    QDomDocument doc("ServerConfiguration");
    QDomElement root = doc.createElement("Serverlist");
    doc.appendChild(root);

    foreach (SERVER_ITEM si, m_serverList)
    {
        QDomElement elem = doc.createElement("Server");

        elem.setAttribute("Password", QString( RC4(si.password.toAscii()).toBase64()) );
        elem.setAttribute("User", QString( si.user.toAscii().toPercentEncoding() ) );
        elem.setAttribute("Name", QString( si.server.toAscii().toPercentEncoding() ) );
        elem.setAttribute("Type", Server_Type_String[si.type]);
        if (si.port != 0)
        {
            elem.setAttribute("Port", QString::number(si.port));
        }

        root.appendChild(elem);
    }

    QDomElement elemProperty = doc.createElement("Property");

    elemProperty.setAttribute("AllProxyEnabled", QVariant(m_AllProxyEnabled).toString());
    root.appendChild(elemProperty);


    QFile file(util::getHomePath() + "serverconfig.xml");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toByteArray());
        file.close();
    }
}

QList<SERVER_ITEM> Server::GetServers(SERVER_TYPE type)
{
    QList<SERVER_ITEM> ret;

    foreach (SERVER_ITEM si, m_serverList)
    {
        if (si.type == type)
            ret << si;
    }

    return ret;
}

void Server::AddServer(SERVER_ITEM &si)
{
    m_serverList << si;
    SaveServerList();
}

void Server::DeleteServer(SERVER_TYPE type, QString server, QString user)
{
    QList<SERVER_ITEM>::iterator i = m_serverList.begin();
    while (i != m_serverList.end())
    {
        SERVER_ITEM si = *i;
        if ( (si.type == type) && (si.server == server) && (si.user == user) )
        {
            i = m_serverList.erase(i);
        }
        else
            ++i;
    }
    SaveServerList();
}

void Server::DeleteServer(SERVER_TYPE type, int iIndex)
{
    int iCounter = 0;
    QList<SERVER_ITEM>::iterator i = m_serverList.begin();
    while (i != m_serverList.end())
    {
        SERVER_ITEM si = *i;
        if (si.type == type)
        {
            if (iCounter == iIndex )
            {
                m_serverList.erase(i);
                break;
            }
            iCounter++;
        }
        ++i;
    }
    SaveServerList();
}

void Server::ModifyServer(SERVER_TYPE type, int iIndex, SERVER_ITEM &si)
{
    int iCounter = 0;
    QList<SERVER_ITEM>::iterator i = m_serverList.begin();
    while (i != m_serverList.end())
    {
        SERVER_ITEM &sitem = *i;
        if (sitem.type == type)
        {
            if (iCounter == iIndex )
            {
                sitem = si;
                break;
            }
            iCounter++;
        }
        ++i;
    }
    SaveServerList();
}

bool Server::MatchedServer(QString sURI, SERVER_ITEM &si)
{
    bool bRet = false;
    QUrl sURL(sURI);

    QList<SERVER_ITEM> list = GetServers(SERVER_HTTP_FTP);
    foreach (SERVER_ITEM sitem, list)
    {
        QUrl serverURL(sitem.server);
        if ( (sURL.scheme().size() != 0) && (sURL.host().size() != 0) && (sURL.scheme() == serverURL.scheme()) && (sURL.host() == serverURL.host()) )
        {
            si = sitem;
            bRet = true;
            break;
        }
    }
    return bRet;
}
