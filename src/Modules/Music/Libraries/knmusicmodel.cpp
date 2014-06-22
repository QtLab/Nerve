#include <QList>
#include <QStandardItem>
#include <QModelIndexList>
#include <QDataStream>
#include <QFileInfo>
#include <QDateTime>
#include <QBuffer>
#include <QTimer>
#include <QStringList>

#include <QTime>

#include <QDebug>

#include "../../Base/knlibdatabase.h"
#include "../../Base/knlibhashpixmaplist.h"
#include "../../knglobal.h"

#include "knmusicinfocollectormanager.h"

#include "knmusicmodel.h"

KNMusicModel::KNMusicModel(QObject *parent) :
    KNMusicModelBase(parent)
{
    //Initial the music global.
    m_musicGlobal=KNMusicGlobal::instance();

    //Reset header data.
    resetHeader();

    //The album art process list.
    //All the album art data will get from here.
    m_pixmapList=new KNLibHashPixmapList;
    m_pixmapList->moveToThread(&m_pixmapListThread);
    connect(m_pixmapList, &KNLibHashPixmapList::requireUpdatePixmap,
            this, &KNMusicModel::onActionUpdatePixmap);
    connect(m_pixmapList, &KNLibHashPixmapList::loadComplete,
            this, &KNMusicModel::onActionImageLoadComplete);
    connect(this, &KNMusicModel::requireLoadImage,
            m_pixmapList, &KNLibHashPixmapList::loadImages);

    //Start pixmap list thread.
    m_pixmapListThread.start();
}

KNMusicModel::~KNMusicModel()
{
    m_pixmapListThread.quit();
    m_pixmapListThread.wait();

    m_pixmapList->deleteLater();
}

QImage KNMusicModel::artwork(const int &row) const
{
    QString imageKey=itemArtworkKey(row);
    return imageKey.isEmpty()?m_musicGlobal->noAlbumImage():artworkFromKey(imageKey);
}

QImage KNMusicModel::artworkFromKey(const QString &key) const
{
    QImage pixmap=m_pixmapList->pixmap(key);
    return pixmap.isNull()?m_musicGlobal->noAlbumImage():pixmap;
}

QString KNMusicModel::itemArtworkKey(const int &row) const
{
    return data(index(row, KNMusicGlobal::Name),
                KNMusicGlobal::ArtworkKeyRole).toString();
}

void KNMusicModel::addRawFileItem(QString filePath)
{
    QModelIndexList fileCheck=match(index(0,0),
                                    KNMusicGlobal::FilePathRole,
                                    filePath);
    if(fileCheck.size()!=0)
    {
        //Find the same file in the model, update it's information.
        updateIndexInfo(fileCheck.at(0), filePath);
        return;
    }
    QList<QStandardItem *> songItemList;
    QStandardItem *songItem;
    for(int i=0; i<KNMusicGlobal::MusicDataCount; i++)
    {
        songItem=new QStandardItem();
        songItem->setEditable(false);
        songItemList.append(songItem);
    }
    QFileInfo rawFileInfo(filePath);
    QDateTime currentTime=QDateTime::currentDateTime();
    songItem=songItemList.at(KNMusicGlobal::DateAdded);
    songItem->setData(currentTime, Qt::UserRole);
    songItem=songItemList.at(KNMusicGlobal::Name);
    songItem->setText(rawFileInfo.fileName());
    songItem->setData(filePath, KNMusicGlobal::FilePathRole);
    songItem->setData(1);
    appendRow(songItemList);
    m_rawFileCount++;
    updateIndexInfo(songItem->index(), filePath);
}

void KNMusicModel::addRawFileItems(QStringList fileList)
{
    int listFileCount=fileList.size();
    //***Speed Test***
    qDebug()<<"***Speed Test***"<<endl<<"Files Count: "<<listFileCount;
    m_startTime=QTime::currentTime().msecsSinceStartOfDay();
    //****************
    while(listFileCount--)
    {
        addRawFileItem(fileList.takeFirst());
    }
}

void KNMusicModel::setAlbumArtPath(const QString &path)
{
    m_pixmapList->setAlbumArtPath(path);
    emit requireLoadImage();
}

void KNMusicModel::recoverFile(QStringList textList,
                               KNMusicGlobal::MusicDetailsInfo currentDetails)
{
    emit musicRecover(appendMusicItem(textList, currentDetails));
}

void KNMusicModel::setInfoCollectorManager(KNLibInfoCollectorManager *infoCollectorManager)
{
    KNModel::setInfoCollectorManager(infoCollectorManager);
    m_infoCollectorManager=qobject_cast<KNMusicInfoCollectorManager *>(infoCollectorManager);
}

void KNMusicModel::retranslate()
{
    ;
}

void KNMusicModel::retranslateAndSet()
{
    KNModel::retranslate();
}

void KNMusicModel::onActionRecoverComplete()
{
    m_dataRecoverComplete=true;
    emit requireResort();
    if(m_imageRecoverComplete)
    {
        emit requireUpdateImage();
    }
}

void KNMusicModel::onActionUpdateRowInfo()
{
    QStringList currentText=m_infoCollectorManager->currentFileData();
    KNMusicGlobal::MusicDetailsInfo currentDetails=
            m_infoCollectorManager->currentFileAppendData();
    int currentRow=m_infoCollectorManager->currentIndex();
    QStandardItem *songItem;
    for(int i=0; i<KNMusicGlobal::MusicDataCount; i++)
    {
        songItem=item(currentRow, i);
        songItem->setText(currentText.at(i));
    }
    setMusicDetailsInfo(currentRow, currentDetails);
    if(!currentDetails.coverImage.isNull())
    {
        m_pixmapList->append(currentRow, currentDetails.coverImage);
    }
    songItem=item(currentRow,KNMusicGlobal::Name);
    songItem->setData(currentDetails.filePath, KNMusicGlobal::FilePathRole);
    if(songItem->data().toInt()==1)
    {
        //This is a new file, never add to list.
        songItem->setData(0);
        m_rawFileCount--;
        emit musicAppend(songItem->index());
        if(m_rawFileCount==0)
        {
            //***Speed Test***
            qDebug()<<"Time Use(ms): "<<
                       QTime::currentTime().msecsSinceStartOfDay()-m_startTime;
            //****************
        }
    }
    else
    {
        emit musicUpdate(songItem->index());
    }
    m_infoCollectorManager->removeFirstUpdateResult();
    if(!m_infoCollectorManager->isUpdateQueueEmpty())
    {
        emit requireUpdateNextRow();
    }
}

void KNMusicModel::onActionImageLoadComplete()
{
    m_imageRecoverComplete=true;
    if(m_dataRecoverComplete)
    {
        emit requireUpdateImage();
    }
}

void KNMusicModel::onActionUpdatePixmap()
{
    setData(index(m_pixmapList->currentRow(),
                  KNMusicGlobal::Name),
            m_pixmapList->currentKey(),
            KNMusicGlobal::ArtworkKeyRole);
    emit musicAlbumArtUpdate(m_pixmapList->currentRow());
    m_pixmapList->removeCurrentUpdate();
}

void KNMusicModel::updateIndexInfo(const QModelIndex &index,
                                   const QString &filePath)
{
    infoCollectorManager()->addAnalysisList(index.row(),
                                            filePath);
}

void KNMusicModel::prepareRemove(const QModelIndex &index)
{
    emit musicAboutToRemove(index);
}
