#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label_versionNumber->setText(QString("<big><b>Aria2c Remote Control v%1</b></big>").arg(QCoreApplication::applicationVersion()));
    QIcon icon = windowIcon();
    QSize size = icon.actualSize(QSize(32, 32));
    ui->label_Icon->setPixmap(icon.pixmap(size));
}

About::~About()
{
    delete ui;
}
