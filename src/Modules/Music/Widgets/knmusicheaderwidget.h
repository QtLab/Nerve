#ifndef KNMUSICHEADERWIDGET_H
#define KNMUSICHEADERWIDGET_H

#include "../../Base/knstdlibheaderwidget.h"

class QBoxLayout;
class KNSearchBox;
class KNMusicHeaderWidget : public KNStdLibHeaderWidget
{
    Q_OBJECT
public:
    explicit KNMusicHeaderWidget(QWidget *parent = 0);

signals:

public slots:
    void retranslate();
    void retranslateAndSet();

private:
    QBoxLayout *m_mainLayout;
    KNSearchBox *m_searchBox;
    QString m_searchPlaceHolder;
};

#endif // KNMUSICHEADERWIDGET_H
