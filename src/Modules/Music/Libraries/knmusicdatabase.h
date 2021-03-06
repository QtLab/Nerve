#ifndef KNMUSICDATABASE_H
#define KNMUSICDATABASE_H

#include <QModelIndex>
#include <QDateTime>

#include "../Base/knmusicdatabasebase.h"

class QAbstractItemModel;
class KNMusicLibraryModelBase;
class KNGlobal;
class KNMusicDatabase : public KNMusicDatabaseBase
{
    Q_OBJECT
public:
    explicit KNMusicDatabase(QObject *parent = 0);
    void setModel(QAbstractItemModel *model);
    void recoverData();

signals:

protected slots:
    void onActionRowAppend(QModelIndex index);
    void onActionRowUpdate(QModelIndex index);
    void onActionRowRemove(QModelIndex index);
    void onActionUpdateCoverImage(const int &index);

private:
    QJsonObject createRowObject(const int &row);
    QString dateTimeToString(const QDateTime &dateTime);
    QDateTime stringToDateTime(const QString &string);
    KNMusicLibraryModelBase *m_model;
    KNGlobal *m_global;
};

#endif // KNMUSICDATABASE_H
