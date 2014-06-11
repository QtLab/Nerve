#ifndef KNMUSICINFOCOLLECTORMANAGER_H
#define KNMUSICINFOCOLLECTORMANAGER_H

#include <QThread>
#include <QStringList>
#include <QList>
#include <QModelIndex>

#include "../knmusicglobal.h"

#include "../../Base/knlibinfocollectormanager.h"

class KNLibBass;
class KNMusicInfoCollector;
class KNMusicInfoCollectorManager : public KNLibInfoCollectorManager
{
    Q_OBJECT
public:
    explicit KNMusicInfoCollectorManager(QObject *parent = 0);
    ~KNMusicInfoCollectorManager();

    QStringList currentFileData() const;
    KNMusicGlobal::MusicDetailsInfo currentFileAppendData() const;
    int currentIndex() const;
    void removeFirstUpdateResult();
    bool isUpdateQueueEmpty();
    bool isWorking();
    void setMusicBackend(KNLibBass *backend);

signals:
    void requireAnalysis(const QString &filePath);

public slots:
    void addAnalysisList(int index,
                         QString filePath);

private slots:
    void analysisNext();
    void currentWorkDone(QStringList value,
                         KNMusicGlobal::MusicDetailsInfo datas);

private:
    struct AnalysisQueueItem
    {
        int index;
        QString filePath;
    };
    struct ResultQueueItem
    {
        int index;
        QStringList text;
        KNMusicGlobal::MusicDetailsInfo details;
    };

    KNMusicInfoCollector *m_collector;
    QThread m_collectThread;
    QList<AnalysisQueueItem> m_analysisQueue;
    QList<ResultQueueItem> m_resultQueue;
    bool m_working=false, m_noUpdating=true;
};

#endif // KNMUSICINFOCOLLECTORMANAGER_H
