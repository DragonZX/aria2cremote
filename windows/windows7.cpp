#include "windows7.h"

Windows7::Windows7():
    m_taskbarInterface(NULL)
{
}

Windows7::~Windows7()
{
    if (m_taskbarInterface)
    {
        m_taskbarInterface->Release();
    }
}

void Windows7::initTaskbarButton(QWidget *parent)
{
    m_parent = parent;
    HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast<void**> (&(m_taskbarInterface)));

    if (SUCCEEDED(hr))
    {
        hr = m_taskbarInterface->HrInit();

        if (FAILED(hr))
        {
            m_taskbarInterface->Release();
            m_taskbarInterface = NULL;
        }
    }
}

void Windows7::updatePorgressBarState(TBPFLAG state)
{
    if (m_taskbarInterface)
    {
         m_taskbarInterface->SetProgressState(m_parent->winId(), state);
    }
}

void Windows7::updateProgressBarValue(int value)
{
    if (m_taskbarInterface)
    {
        m_taskbarInterface->SetProgressValue(m_parent->winId(), value, 100);
    }
}

void Windows7::updateOverlayIcon(bool isConnected)
{
    if (m_taskbarInterface)
    {
        QString icon_res(":/icon/toolbars/" + QString(isConnected ? "connected.png" : "disconnected.png"));
        // a NULL icon will reset the current overlay icon
        SetOverlayIcon(QIcon(icon_res), "Description");
    }
}

void Windows7::SetOverlayIcon(const QIcon& icon, QString description)
{
    if (m_taskbarInterface)
    {

        HICON overlay_icon = icon.isNull() ? NULL : icon.pixmap(48).toWinHICON();
        m_taskbarInterface->SetOverlayIcon(m_parent->winId(), overlay_icon, description.toStdWString().c_str());

        if (overlay_icon)
        {
            DestroyIcon(overlay_icon);
        }
    }

    return;
}
