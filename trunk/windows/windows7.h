#ifndef WINDOWS7_H
#define WINDOWS7_H
#include "winutils.h"
#include <QIcon>
#include <QString>
#include <QWidget>

class Windows7
{
public:
    Windows7();
    ~Windows7();
    void initTaskbarButton(QWidget *parent);
    void updateOverlayIcon(bool isConnected);
    void updateProgressBarValue(int value);
    void updatePorgressBarState(TBPFLAG state);
private:
    ITaskbarList3* m_taskbarInterface;
    QWidget *m_parent;
    void SetOverlayIcon(const QIcon &icon, QString description);
};

#endif // WINDOWS7_H
