/*
 * Widget, выводящий необходимые параметры загруженного файла и список
 * таблиц
 *
 * (C) НПО Рубин
 * @author:
 *   Ю.Л.Русинов
 */

#pragma once

#include <QWidget>

class QToolBar;
class QTextEdit;
class QTreeView;
class QAbstractItemModel;

class unoFileWidget : public QWidget {
public:
    explicit unoFileWidget(QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    virtual ~unoFileWidget();

    void setText(const QString& text);
    void setTablesModel(QAbstractItemModel* tableListModel);

private slots:
    void slotFileOpen();
    void slotFileClose();

private:
    QToolBar* _tbActions;
    QTextEdit* _fileEditW;
    QTreeView* _tvTables;

private:
    Q_OBJECT
};
