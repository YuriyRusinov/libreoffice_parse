/*
 * Widget, выводящий необходимые параметры загруженного файла и список
 * таблиц
 *
 * (C) НПО Рубин
 * @author:
 *   Ю.Л.Русинов
 */

#pragma once

#include <QModelIndex>
#include <QUrl>
#include <QWidget>
#include <com/sun/star/text/XTextTable.hpp>

using namespace com::sun::star::text;
using namespace com::sun::star::uno;

class QLabel;
class QLineEdit;
class QToolBar;
class QTextEdit;
class QTreeView;
class QAbstractItemModel;
class QSplitter;

class unoFileWidget : public QWidget {
public:
    explicit unoFileWidget(const QUrl& fileUrl, QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    virtual ~unoFileWidget();

    void setText(const QString& text);
    void setTablesModel(QAbstractItemModel* tableListModel);

private slots:
    void slotFileOpen();
    void slotFileClose();
    void slotSearch();
    void slotAddRowToTable();
    void slotDelRowFromTable();
    void updateTableModel(Reference< XTextTable > wTable);
    void slotSaveFile();

signals:
    void search(QString searchStr);
    void addRowToTable(Reference< XTextTable > wTable, int iRow);
    void delRowFromTable(Reference< XTextTable > wTable, int iRow);
    void saveWriterFile(QUrl saveFileUrl);

private:
    QUrl _fileUrl;
    QToolBar* _tbActions;
    QSplitter* _spView;
    QTextEdit* _fileEditW;
    QWidget* _wTables;
    QTreeView* _tvTables;
    QToolBar* _tbTableActions;
    QWidget* _wSearch;
    QLabel* _lSearch;
    QLineEdit* _leSearch;
    QModelIndex wTableIndex;

    friend class unoFileObject;

private:
    Q_OBJECT
};
