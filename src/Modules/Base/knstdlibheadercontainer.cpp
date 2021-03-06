#include <QTimeLine>
#include <QResizeEvent>

#include "knlibheaderswitcher.h"
#include "knstdlibcategorybutton.h"

#include "knstdlibheadercontainer.h"

KNStdLibHeaderContainer::KNStdLibHeaderContainer(QWidget *parent) :
    KNLibHeaderContainer(parent)
{
    setAutoFillBackground(true);

    int minGrey=0x10;
    m_backgroundColor=QColor(minGrey, minGrey, minGrey);
    m_palette=palette();
    m_palette.setColor(QPalette::Base, m_backgroundColor);
    m_palette.setColor(QPalette::Window, QColor(0x10, 0x10, 0x10));
    m_palette.setColor(QPalette::Button, QColor(0x10, 0x10, 0x10));
    m_palette.setColor(QPalette::Text, QColor(0x9f, 0x9f, 0x9f));
    setPalette(m_palette);

    m_mouseIn=new QTimeLine(200, this);
    m_mouseIn->setUpdateInterval(5);
    m_mouseIn->setEndFrame(0x50);
    connect(m_mouseIn, &QTimeLine::frameChanged,
            this, &KNStdLibHeaderContainer::changeBackground);

    m_mouseOut=new QTimeLine(200, this);
    m_mouseOut->setUpdateInterval(5);
    m_mouseOut->setEndFrame(minGrey);
    connect(m_mouseOut, &QTimeLine::frameChanged,
            this, &KNStdLibHeaderContainer::changeBackground);
}

void KNStdLibHeaderContainer::addCategorySwitcher(KNStdLibCategoryButton *button)
{
    button->setParent(this);
    button->move(0,0);
    connect(button, &KNStdLibCategoryButton::requireResetLeftSpace,
            this, &KNStdLibHeaderContainer::resetLeftSpace);
    m_button=button;
    if(m_switcher!=NULL)
    {
        linkButtonAndSwitcher();
    }
}

void KNStdLibHeaderContainer::addHeaderSwitcher(KNLibHeaderSwitcher *switcher)
{
    m_switcher=switcher;
    m_switcher->setParent(this);
    setFixedHeight(m_switcher->height());
    if(m_button!=NULL)
    {
        linkButtonAndSwitcher();
    }
}

void KNStdLibHeaderContainer::enterEvent(QEvent *e)
{
    m_mouseOut->stop();
    m_mouseIn->stop();
    m_mouseIn->setStartFrame(m_backgroundColor.red());
    m_mouseIn->start();
    KNLibHeaderContainer::enterEvent(e);
}

void KNStdLibHeaderContainer::leaveEvent(QEvent *e)
{
    m_mouseIn->stop();
    m_mouseOut->stop();
    m_mouseOut->setStartFrame(m_backgroundColor.red());
    m_mouseOut->start();
    KNLibHeaderContainer::leaveEvent(e);
}

void KNStdLibHeaderContainer::resizeEvent(QResizeEvent *event)
{
    KNLibHeaderContainer::resizeEvent(event);
    resetSwitcherPosition();
}

void KNStdLibHeaderContainer::resetLeftSpace(const int &leftMargin)
{
    m_leftMargin=leftMargin;
    resetSwitcherPosition();
}

void KNStdLibHeaderContainer::changeBackground(int frameData)
{
    m_backgroundColor=QColor(frameData, frameData, frameData);
    m_palette.setColor(QPalette::Window, m_backgroundColor);
    setPalette(m_palette);
}

void KNStdLibHeaderContainer::linkButtonAndSwitcher()
{
    connect(m_button, &KNStdLibCategoryButton::requireDisableHeader,
            [=]{m_switcher->setEnabled(false);});
    connect(m_button, &KNStdLibCategoryButton::requireEnableHeader,
            [=]{m_switcher->setEnabled(true);});
}

void KNStdLibHeaderContainer::resetSwitcherPosition()
{
    m_switcher->setGeometry(m_leftMargin,
                            0,
                            width()-m_leftMargin,
                            m_switcher->height());
}

