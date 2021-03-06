#ifndef KNMUSICTAGM4A_H
#define KNMUSICTAGM4A_H

#include <QMap>
#include <QList>
#include <QStringList>
#include <QPixmap>
#include <QByteArray>

#include "knmusictagbase.h"

class KNMusicTagM4A : public KNMusicTagBase
{
    Q_OBJECT
public:
    explicit KNMusicTagM4A(QObject *parent = 0);
    QString metaDataName(const int &index) const;
    QString textData(const int &key) const;
    QByteArray metaData(const QString &name) const;
    QByteArray metaData(const int &index) const;
    QStringList keyList() const;
    QString frameTextData(const QString &name) const;
    bool readTag(const QFile &mediaFile,
                 QDataStream &mediaData);
    QImage albumArt() const;
    void clearCache();
    enum M4AMetaData
    {
        Album,
        Artist,
        AlbumArtist,
        Comment,
        Year,
        Title,
        Genre,
        Tracknumber,
        Disknumber,
        Composer,
        Encoder,
        BPM,
        Copyright,
        Compilation,
        Artwork,
        Rating,
        Grouping,
        MediaType,
        Podcast,
        Category,
        Keyword,
        PodcastURL,
        EpisodeGlobalUniqueID,
        Description,
        Lyrics,
        TVNetworkName,
        TVShowName,
        TVEpisodeNumber,
        TVSeason,
        TVEpisode,
        PurchaseDate,
        GaplessPlayback,
        M4AMetaDataCount
    };

signals:

public slots:

private:
    struct tagData
    {
        int start;
        int end;
        tagData()
        {
            start=0;
            end=0;
        }
    };
    QString m_m4aCharCode[M4AMetaDataCount];

    void parseMeta(const int &start,
                   const int &end);
    void searchIn(const int &start,
                  const int &end,
                  bool searchDeep=false);
    char m_m4aHeader[4]={'f', 't', 'y', 'p'}, m_idCache[5], *m_rawTagData;
    QMap<QString, tagData> m_tagData;
    QStringList m_keys;
    QList<QByteArray> m_data;
    QImage m_albumArt;
};

#endif // KNMUSICTAGM4A_H
