#include <QBoxLayout>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QToolButton>
#include <QTimeLine>
#include <QFileInfo>

#include <QDebug>

#include "../../Public/Base/knmusicbackend.h"

#include "../Libraries/knmusiclibrarymodel.h"
#include "../knmusicglobal.h"
#include "../../Base/knplayerprogress.h"

#include "knmusicdetailtooltip.h"

KNMusicDetailTooltipPlay::KNMusicDetailTooltipPlay(QWidget *parent) :
    QLabel(parent)
{
    setFixedSize(20,20);
    setScaledContents(true);
    m_play=QPixmap(":/Music/Resources/Music/preivew/play.png");
    m_pause=QPixmap(":/Music/Resources/Music/preivew/pause.png");
    setPixmap(m_play);
}

void KNMusicDetailTooltipPlay::reset()
{
    setPixmap(m_play);
    m_isButtonPlay=true;
}

bool KNMusicDetailTooltipPlay::isPaused() const
{
    return m_isButtonPlay;
}

void KNMusicDetailTooltipPlay::setState(const bool &shownPlay)
{
    if(shownPlay)
    {
        reset();
    }
    else
    {
        setPixmap(m_pause);
        m_isButtonPlay=false;
    }
}

void KNMusicDetailTooltipPlay::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    if(m_isButtonPlay)
    {
        emit requirePlay();
        m_isButtonPlay=false;
        setPixmap(m_pause);
    }
    else
    {
        emit requirePause();
        m_isButtonPlay=true;
        setPixmap(m_play);
    }
}

KNMusicDetailTooltip::KNMusicDetailTooltip(QWidget *parent) :
    QWidget(parent)
{
    retranslate();
    setFixedSize(m_fixedWidth, m_fixedHeight);
    m_palette=palette();
    m_background=QColor(0x28,0x28,0x28);
    m_palette.setColor(QPalette::Window, m_background);
    m_palette.setColor(QPalette::WindowText, QColor(0x5f, 0x5f, 0x5f));
    setPalette(m_palette);

    setWindowFlags(Qt::ToolTip);
    m_tooltipDisapper=new QTimer;
    m_tooltipDisapper->setSingleShot(true);
    m_tooltipDisapper->setInterval(1500);
    connect(m_tooltipDisapper, SIGNAL(timeout()),
            this, SLOT(onActionHide()));

    m_mainLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                this);
    m_mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(m_mainLayout);
    QBoxLayout *albumLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                           m_mainLayout->widget());
    m_mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_mainLayout->addLayout(albumLayout);
    m_albumArt=new QLabel(this);
    m_albumArt->setFixedSize(128, 128);
    m_albumArt->setScaledContents(true);
    albumLayout->addWidget(m_albumArt);

    m_labelContainer=new QWidget(this);
    m_labelContainer->setFixedSize(292, 128);
    m_labelContainer->setMaximumSize(292, 128);
    m_labelLayout=new QBoxLayout(QBoxLayout::TopToBottom);
    m_labelLayout->setContentsMargins(0,0,0,0);
    m_labelContainer->setLayout(m_labelLayout);
    for(int i=0; i<ToolTipItemsCount; i++)
    {
        m_labels[i]=new QLabel(this);
        m_labelLayout->addWidget(m_labels[i]);
    }
    QFont nameFont=m_labels[Title]->font();
    nameFont.setBold(true);
    m_labels[Title]->setFont(nameFont);
    m_labelLayout->addStretch();
    albumLayout->addWidget(m_labelContainer);

    QBoxLayout *previewPlayer=new QBoxLayout(QBoxLayout::LeftToRight,
                                             m_mainLayout->widget());
    previewPlayer->setContentsMargins(0,0,0,0);
    m_control=new KNMusicDetailTooltipPlay(this);
    previewPlayer->addWidget(m_control);
    m_playerStatus=new KNPlayerProgress(this);
    connect(m_playerStatus, &KNPlayerProgress::sliderReleased,
            this, &KNMusicDetailTooltip::onActionSliderReleased);
    connect(m_playerStatus, &KNPlayerProgress::sliderPressed,
            this, &KNMusicDetailTooltip::onActionSliderPressed);
    m_playerStatus->setMinimumWidth(400);
    previewPlayer->addWidget(m_playerStatus, 1);
    m_mainLayout->addLayout(previewPlayer);

    m_mouseIn=new QTimeLine(200, this);
    m_mouseIn->setEasingCurve(QEasingCurve::OutCubic);
    m_mouseIn->setUpdateInterval(5);
    m_mouseIn->setEndFrame(0x50);
    connect(m_mouseIn, &QTimeLine::frameChanged,
            this, &KNMusicDetailTooltip::onActionChangeBackground);
    m_mouseOut=new QTimeLine(200, this);
    m_mouseOut->setEasingCurve(QEasingCurve::OutCubic);
    m_mouseOut->setUpdateInterval(5);
    m_mouseOut->setEndFrame(0x28);
    connect(m_mouseOut, &QTimeLine::frameChanged,
            this, &KNMusicDetailTooltip::onActionChangeBackground);
}

KNMusicDetailTooltip::~KNMusicDetailTooltip()
{
    m_tooltipDisapper->deleteLater();
    m_labelLayout->deleteLater();
}

void KNMusicDetailTooltip::setTooltip(const QModelIndex &index,
                                      const QPoint &point)
{
    m_mouseOut->stop();
    m_mouseIn->stop();
    onActionChangeBackground(0x28);
    m_tooltipDisapper->setInterval(1500);
    resetPlayStatus();
    if(m_currentRow==index.row())
    {
        return;
    }
    m_currentRow=index.row();
    m_albumArt->setPixmap(QPixmap::fromImage(m_musicModel->artwork(m_currentRow)));

    QFileInfo musicFileInfo(m_musicModel->filePathFromIndex(index));
    setEliedText(m_labels[Title], m_musicModel->itemText(m_currentRow, KNMusicGlobal::Name));
    setEliedText(m_labels[FileName], m_inFile.arg(musicFileInfo.fileName()));
    setEliedText(m_labels[Time], m_musicModel->itemText(m_currentRow, KNMusicGlobal::Time));
    setEliedText(m_labels[Artist], m_musicModel->itemText(m_currentRow, KNMusicGlobal::Artist));

    m_filePath=m_musicModel->filePathFromIndex(m_currentRow);
    move(bestPosition(point));
}

void KNMusicDetailTooltip::setMusicModel(KNMusicLibraryModelBase *model)
{
    m_musicModel=model;
}

void KNMusicDetailTooltip::showTooltip()
{
    if(!isVisible())
    {
        show();
    }
    m_tooltipDisapper->stop();
    m_tooltipDisapper->start();
}

void KNMusicDetailTooltip::forceQuit()
{
    m_preview->stopPreview();
    m_tooltipDisapper->stop();
    hide();
}

void KNMusicDetailTooltip::setMusicBackend(KNMusicBackend *backend)
{
    m_preview=backend;
    connect(m_control, &KNMusicDetailTooltipPlay::requirePlay,
            [=]
            {
                m_preview->playPreview();
            });
    connect(m_control, &KNMusicDetailTooltipPlay::requirePause,
            [=]
            {
                m_preview->pausePreview();
            });
    connect(m_preview, &KNMusicBackend::previewPositionChanged,
            this, &KNMusicDetailTooltip::onActionPositionChanged);
    connect(m_preview, &KNMusicBackend::previewFinished,
            this, &KNMusicDetailTooltip::onActionPreviewFinished);
}

void KNMusicDetailTooltip::resetPlayStatus()
{
    m_control->reset();
    m_playerStatus->setValue(0);
    m_sliderPressed=false;
}

void KNMusicDetailTooltip::retranslate()
{
    m_inFile=tr("In file: %1");
}

void KNMusicDetailTooltip::retranslateAndSet()
{
    ;
}

void KNMusicDetailTooltip::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    m_tooltipDisapper->stop();
    m_tooltipDisapper->setInterval(300);
    m_mouseOut->stop();
    m_mouseIn->setStartFrame(m_background.red());
    m_mouseIn->start();
    m_preview->stopPreview();
    m_preview->loadPreview(m_filePath);
    m_playerStatus->setMaximum(m_preview->previewDuration());
}

void KNMusicDetailTooltip::leaveEvent(QEvent *event)
{
    m_mouseIn->stop();
    m_mouseOut->setStartFrame(m_background.red());
    m_mouseOut->start();
    QWidget::leaveEvent(event);
    m_tooltipDisapper->start();
}

void KNMusicDetailTooltip::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    m_preview->stopPreview();
}

void KNMusicDetailTooltip::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {
        hide();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void KNMusicDetailTooltip::onActionHide()
{
    hide();
}

void KNMusicDetailTooltip::onActionPositionChanged(qint64 test)
{
    if(m_sliderPressed)
    {
        return;
    }
    m_playerStatus->setValue(test);
}

void KNMusicDetailTooltip::onActionSliderReleased()
{
    m_sliderPressed=false;
    m_preview->pausePreview();
    m_preview->setPreviewPosition(m_playerStatus->value());
    if(!m_control->isPaused())
    {
        m_preview->playPreview();
        m_control->setState(false);
    }
}

void KNMusicDetailTooltip::onActionSliderPressed()
{
    m_sliderPressed=true;
}

void KNMusicDetailTooltip::onActionChangeBackground(const int &colorParam)
{
    m_background=QColor(colorParam, colorParam, colorParam);
    m_palette.setColor(QPalette::Window, m_background);
    int textParam=0x5f+(colorParam<<1);
    m_palette.setColor(QPalette::WindowText, QColor(textParam,textParam,textParam));
    setPalette(m_palette);
}

void KNMusicDetailTooltip::onActionPreviewFinished()
{
    resetPlayStatus();
}

void KNMusicDetailTooltip::setEliedText(QLabel *widget, const QString &rawTextData)
{
    if(widget->fontMetrics().width(rawTextData)>widget->width())
    {
        widget->setText(widget->fontMetrics().elidedText(rawTextData,
                                                          Qt::ElideRight,
                                                          widget->width()));
        widget->setToolTip(rawTextData);
    }
    else
    {
        widget->setText(rawTextData);
        widget->setToolTip("");
    }
}

QPoint KNMusicDetailTooltip::bestPosition(const QPoint &pos)
{
    int tweakX=pos.x()+14, tweakY=pos.y()-60,
        screenHeight=QApplication::desktop()->height(),
        screenWidth=QApplication::desktop()->width(),
        realWidth=sizeHint().width();
    if(tweakX+realWidth>screenWidth)
    {
        tweakX=screenWidth-realWidth-5;
    }
    if(tweakY+height()>screenHeight)
    {
        tweakY=screenHeight-height();
    }
    return QPoint(tweakX, tweakY);
}
