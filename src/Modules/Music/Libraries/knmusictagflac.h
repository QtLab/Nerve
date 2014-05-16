#ifndef KNMUSICTAGFLAC_H
#define KNMUSICTAGFLAC_H

#include <QMap>
#include <QImage>
#include "knmusictagbase.h"

class KNMusicTagFLAC : public KNMusicTagBase
{
    Q_OBJECT
public:
    enum FLACItems
    {
        Name,
        Artist,
        Album,
        Genre,
        Description,
        Composer,
        DiscCount,
        DiscNumber,
        Comments,
        AlbumArtist,
        TrackCount,
        Year,
        TrackNumber,
        FLACItemsCount
    };

    explicit KNMusicTagFLAC(QObject *parent = 0);
    bool readTag(const QString &filePath);
    void clearCache();
    QString textData(const int &key) const;
    QString metaData(const QString &index) const;
    QString rawMetaData(const QString &index);
    QImage tagImage(const int &index) const;
    QImage firstAvaliableImage() const;

signals:

public slots:

private:
    void parseVorbisComment(char *rawTagData, int length);
    void parsePicture(char *rawTagData);
    quint32 inverseCharToInt32(char *rawTagData);
    quint32 charToInt32(char *rawTagData);
    struct MetadataHeader
    {
        bool isLast=false;
        int type=127;
        quint32 size=0;
    };
    MetadataHeader analysisHeader(char *rawHeader);
    QMap<QString, QString> m_metadata;
    QMap<int, QImage> m_picture;
    QString m_frames[FLACItemsCount];
};

#endif // KNMUSICTAGFLAC_H