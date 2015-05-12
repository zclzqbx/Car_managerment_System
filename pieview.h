#ifndef PIEVIEW_H
#define PIEVIEW_H

#include <QWidget>
#include <QAbstractItemView>

class PieView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit PieView(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);

    void setSelectionModel(QItemSelectionModel *selectionModel);
    QRegion itemRegion(QModelIndex index);

    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint=EnsureVisible);
    QModelIndex indexAt(const QPoint &point) const;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    int horizontalOffset()const;
    int verticalOffset()const;
    bool isIndexHidden(const QModelIndex &index) const;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;

signals:

public slots:

private:
    QItemSelectionModel *selections;
    QList<QRegion> RegionList;

};

#endif // PIEVIEW_H
