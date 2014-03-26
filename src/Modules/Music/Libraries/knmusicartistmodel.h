#ifndef KNMUSICARTISTMODEL_H
#define KNMUSICARTISTMODEL_H

#include "knmusicartistmodel.h"

#include <QIcon>

#include <QStandardItemModel>

class QSplitter;
class KNMusicModel;
class KNMusicArtistItem;
class KNMusicArtistModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit KNMusicArtistModel(QObject *parent = 0);
    KNMusicArtistItem *artistItem(const QModelIndex &index);

signals:

public slots:
    void onMusicAdded(const QModelIndex &index);
    void setSourceModel(QAbstractItemModel *sourceModel);

private:
    KNMusicModel *m_sourceModel;
    QIcon m_noAlbumIcon;
};

#endif // KNMUSICARTISTMODEL_H