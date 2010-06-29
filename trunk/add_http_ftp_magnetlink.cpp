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

#include "add_http_ftp_magnetlink.h"
#include "ui_add_http_ftp_magnetlink.h"

add_http_ftp_magnetlink::add_http_ftp_magnetlink(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_http_ftp_magnetlink)
{
    ui->setupUi(this);
}

add_http_ftp_magnetlink::~add_http_ftp_magnetlink()
{
    delete ui;
}

void add_http_ftp_magnetlink::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString add_http_ftp_magnetlink::GetURI()
{
    return ui->lineEdit->text();
}

void add_http_ftp_magnetlink::SetURI(QString& s)
{
    ui->lineEdit->setText(s);
}

void add_http_ftp_magnetlink::SetTypeText(QString s)
{
    ui->label->setText(s);
}

