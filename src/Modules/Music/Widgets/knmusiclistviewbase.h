#ifndef KNMUSICLISTVIEWBASE_H
#define KNMUSICLISTVIEWBASE_H

#include <QMimeData>

#include <QTreeView>

class QDrag;
class QSortFilterProxyModel;
class KNMusicModel;
class KNMusicDetailTooltip;
class KNMusicListViewHeader;
class KNMusicListViewBase : public QTreeView
{
    Q_OBJECT
public:
    explicit KNMusicListViewBase(QWidget *parent = 0);
    ~KNMusicListViewBase();
    virtual void resetHeader();
    void moveToFirst(const int &logicalHeaderIndex);
    void setHeaderAlignment(const int &logicalHeaderIndex,
                            Qt::Alignment alignment);
    void setModel(QAbstractItemModel *model);
    void setSourceModel(KNMusicModel *musicModel);

signals:
    void requireShowContextMenu(const QPoint &position,
                                const QModelIndex &index);
    void requireOpenUrl(const QModelIndex &index);
    void requireHalfVolume();
    void requireRestoreHalfVolume();

public slots:
    virtual void retranslate();
    virtual void retranslateAndSet();

protected:
    void closeEvent(QCloseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void startDrag(Qt::DropActions supportedActions);
    bool event(QEvent *event);

protected slots:
    virtual void onActionSort(int logicalIndex, Qt::SortOrder order);

private slots:
    void onSectionVisibleChanged(const int &index,
                                 const bool &visible);
    void onItemActived(const QModelIndex &index);

private:
    KNMusicListViewHeader *m_headerWidget;
    QModelIndex m_detailIndex;
    KNMusicDetailTooltip *m_musicDetailTooltip;
    KNMusicModel *m_musicModel;
    QSortFilterProxyModel *m_proxyModel;
    QDrag *m_dragAction;
    QMimeData *m_mimeData;
};

#endif // KNMUSICLISTVIEWBASE_H
