#ifndef KNMUSICVIEWERPLAYLISTITEMBASE_H
#define KNMUSICVIEWERPLAYLISTITEMBASE_H

#include <QObject>

class KNMusicPlaylistManager;
class KNMusicViewerPlaylistItemBase : public QObject
{
    Q_OBJECT
public:
    explicit KNMusicViewerPlaylistItemBase(QObject *parent = 0);
    virtual void applyPlugin()=0;

signals:
    void requirePlayMusic(const QString &filePath);
    void requireAddCategory(const QPixmap &icon,
                            const QString &title,
                            QWidget *widget);

public slots:
    virtual void setPlaylistManager(KNMusicPlaylistManager *manager)=0;

};

#endif // KNMUSICVIEWERPLAYLISTITEMBASE_H