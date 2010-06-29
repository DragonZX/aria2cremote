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

#include "mainlistview.h"
#include "ui_mainlistview.h"
#include <QDir>
#include <QString>
#include <QTranslator>

MainListView::MainListView(QWidget *parent) :
    QListWidget(parent),
    ui(new Ui::MainListView)
{
    ui->setupUi(this);
}

MainListView::~MainListView()
{
    delete ui;
}

void MainListView::changeEvent(QEvent *e)
{
    QListWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainListView::createLanguageMenu()
{
//    languageMenu = new QMenu(this);
//    languageActionGroup = new QActionGroup(this);
//    connect(languageActionGroup, SIGNAL(triggered(QAction *)),
//    this, SLOT(switchLanguage(QAction *)));
//    QDir dir(qmPath);
//    QStringList fileNames = dir.entryList(QStringList("Aria2cRemoteControl_*.qm"));
//    for (int i = 0; i < fileNames.size(); ++i)
//    {
//        QString locale = fileNames[i];
//
//        //locale.remove(0, locale.indexOf(’_’) + 1);
//        //locale.truncate(locale.lastIndexOf(’.’));
//        QTranslator translator;
//        translator.load(fileNames[i], qmPath);
//        QString language = translator.translate("MainWindow", "English");
//        QAction *action = new QAction(tr("&%1 %2").arg(i + 1).arg(language), this);
//        action->setCheckable(true);
//        action->setData(locale);
//        languageMenu->addAction(action);
//        languageActionGroup->addAction(action);
//        if (language == "English")
//            action->setChecked(true);
//    }
}

