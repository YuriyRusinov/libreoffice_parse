/* @brief
 * Widget, выводящий необходимые параметры загруженного файла и список
 * таблиц
 *
 * (C) НИИ Рубин
 * @author:
 *   Ю.Л.Русинов
 */

#pragma once

#include <QModelIndex>
#include <QUrl>
#include <QWidget>
#include <com/sun/star/text/XTextTable.hpp>

#include <vector>

using namespace com::sun::star::text;
using namespace com::sun::star::uno;

using std::vector;

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

    void setTablesModel(QAbstractItemModel* tableListModel);
    enum tableCellParams {
        tableRow,
        tableColumn
    };

    enum tableActions {
        tableAdd,
        tableDel
    };

private slots:
    void slotFileOpen();
    void slotFileClose();
    void slotSearch();
    void slotAddRowToTable();
    void slotDelRowFromTable();
    void slotAddColumnToTable();
    void slotDelColumnFromTable();
    void slotEditCellInTable();
    void updateTableModel(QModelIndex tabIndex, Reference< XTextTable > wTable);
    void slotSaveFile();

signals:
    void search(QString searchStr, vector< Reference< XTextTable > > searchTables, QStringList tableNames);
    void tableActSignal(QModelIndex tabIndex, Reference< XTextTable > wTable, int tableActCode, int tableCoordPar, int iPar);
    void saveWriterFile(QUrl saveFileUrl);
    void closeFile(QUrl fileUrl);
    void editTableCell( Reference< XTextTable >, int iRow, int jColumn );

private:
    QModelIndexList getTableIndexes() const;
    Reference< XTextTable > getTable();
    int getTableParameter(const QModelIndexList& selIndexes, tableActions tabActCode, tableCellParams tabParam);
    void initActions();

private:
    QUrl _fileUrl;
    QToolBar* _tbActions;
    QWidget* _wTables;
    QTreeView* _tvTables;

    friend class unoFileObject;

private:
    Q_OBJECT
};
