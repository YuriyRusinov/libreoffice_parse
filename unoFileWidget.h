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

class QLabel;
class QLineEdit;
class QToolBar;
class QTextEdit;
class QTreeView;
class QAbstractItemModel;
class QSplitter;

class unoFileWidget : public QWidget {
public:
    explicit unoFileWidget(QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    virtual ~unoFileWidget();

    void setText(const QString& text);
    void setTablesModel(QAbstractItemModel* tableListModel);

private slots:
    void slotFileOpen();
    void slotFileClose();
    void slotSearch();

private:
    QToolBar* _tbActions;
    QSplitter* _spView;
    QTextEdit* _fileEditW;
    QTreeView* _tvTables;
    QWidget* _wSearch;
    QLabel* _lSearch;
    QLineEdit* _leSearch;

private:
    Q_OBJECT
};
