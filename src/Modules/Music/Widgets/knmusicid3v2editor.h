#ifndef KNMUSICID3V2EDITOR_H
#define KNMUSICID3V2EDITOR_H

#include <QScopedPointer>
#include <QStandardItemModel>

#include "knmusictageditorbase.h"

class QTreeView;
class QComboBox;
class QPlainTextEdit;
class QLineEdit;
class KNVerticalWidgetSwitcher;
class KNMusicTagID3v2;
class KNMusicID3v2Editor : public KNMusicTagEditorBase
{
    Q_OBJECT
public:
    explicit KNMusicID3v2Editor(QWidget *parent = 0);
    void readTag(QFile &mediaFile,
                 QDataStream &mediaData);
    void resetEditor();
    QString title() const;
    QString album() const;
    QString artist() const;
    QPixmap albumArt() const;

signals:

public slots:
    void retranslate();
    void retranslateAndSet();
    void writeTag(QFile &mediaFile,
                  QDataStream &mediaData);

private slots:
    void toAdvancedMode();
    void toOverviewMode();

private:
    void setEditorText(const int &index, const QString &text);
    enum ID3v2Items
    {
        Name,
        Artist,
        Album,
        AlbumArtist,
        TrackNumber,
        TrackCount,
        DiscNumber,
        DiscCount,
        Year,
        Comment,
        Genre,
        ID3v2ItemsCount
    };
    enum ID3v2CaptionItems
    {
        CaptionName,
        CaptionArtist,
        CaptionAlbum,
        CaptionYear,
        CaptionAlbumArtist,
        CaptionTrack,
        CaptionDisc,
        CaptionGenre,
        CaptionComment,
        ID3v2CaptionItemsCount
    };
    KNMusicTagID3v2 *m_tagID3v2;
    KNVerticalWidgetSwitcher *m_switcher;
    QString m_caption[ID3v2CaptionItemsCount];
    QLabel *m_label[ID3v2CaptionItemsCount];
    QLineEdit *m_textEditor[9];
    QComboBox *m_genreList;
    QPlainTextEdit *m_commentEditor;
    QTreeView *m_advancedView;
    QScopedPointer<QStandardItemModel> m_advancedModel;
    QAction *m_toOverview, *m_toAdvanced;
};

#endif // KNMUSICID3V2EDITOR_H
