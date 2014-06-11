#include <QStringList>
#include <QBoxLayout>

#include "knmusicplaylistlistview.h"
#include "../Libraries/knmusicplaylistmanager.h"
#include "knmusicplaylistlisteditor.h"

#include "knmusicplaylistview.h"

KNMusicPlaylistView::KNMusicPlaylistView(QWidget *parent) :
    QSplitter(parent)
{
    //Get the latest translation.
    retranslate();

    //Set properties.
    setContentsMargins(0,0,0,0);
    setHandleWidth(0);

    //Initial playlist view.
    QWidget *categoryList=new QWidget(this);
    QBoxLayout *categoryLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                              categoryList);
    categoryLayout->setContentsMargins(0,0,0,0);
    categoryLayout->setSpacing(0);
    categoryList->setLayout(categoryLayout);
    m_categories=new KNMusicPlaylistListview(this);
    for(int i=0; i<HeaderCount; i++)
    {
        m_headerIndex[i]=m_categories->addHeader(m_headerCaption[i]);
    }
    categoryLayout->addWidget(m_categories, 1);
    m_listEditor=new KNMusicPlaylistListEditor(this);
    categoryLayout->addWidget(m_listEditor);
    connect(m_listEditor, &KNMusicPlaylistListEditor::requireCreatePlaylist,
            this, &KNMusicPlaylistView::onActionCreatePlaylist);
    connect(m_listEditor, &KNMusicPlaylistListEditor::requireRemoveCurrent,
            this, &KNMusicPlaylistView::onActionRemoveCurrent);
    addWidget(categoryList);

    //Initial playlist displayer.
    QWidget *test=new QWidget(this);
    addWidget(test);

    //Set properties after initial all the widgets.
    setCollapsible(1, false);
    setStretchFactor(1, 1);
}

void KNMusicPlaylistView::setPlaylistManager(KNMusicPlaylistManager *manager)
{
    m_playlistManager=manager;
    connect(m_playlistManager, &KNMusicPlaylistManager::playlistListUpdated,
            this, &KNMusicPlaylistView::onActionUpdatePlaylists);
    connect(m_categories, &KNMusicPlaylistListview::requireAddPlaylist,
            m_playlistManager, &KNMusicPlaylistManager::addPlaylist);
}

void KNMusicPlaylistView::retranslate()
{
    m_headerCaption[Playlist]=tr("Playlist");
    m_headerCaption[Dirs]=tr("Directory");
}

void KNMusicPlaylistView::retranslateAndSet()
{
    retranslate();
}

void KNMusicPlaylistView::onActionUpdatePlaylists()
{
    m_categories->setCurrentHeader(m_headerIndex[Playlist]);
    m_categories->clearHeader();
    QStringList playlistNames=m_playlistManager->playlistNameList();
    int playlistSize=playlistNames.size();
    while(playlistSize--)
    {
        m_categories->addItem(playlistNames.takeFirst());
    }
}

void KNMusicPlaylistView::onActionCreatePlaylist()
{
    //Set to playlist header.
    m_categories->setCurrentHeader(Playlist);
    //Add an item.
    m_categories->createItem();
}

void KNMusicPlaylistView::onActionRemoveCurrent()
{
    //Should ask the user first.
    switch(m_categories->currentHeader())
    {
    case Playlist:
        m_playlistManager->removePlaylist(m_categories->currentItem());
        break;
    case Dirs:
        break;
    default:
        //Here should never comes.
        return;
    }
    m_categories->removeCurrentItem();
}
