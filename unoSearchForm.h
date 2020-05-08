/* @brief
 * Форма поиска по таблицам через механизм uno
 *  (C) НИИ Рубин
 *
 * @author
 *   Ю.Л.Русинов
 */

#pragma once

#include <QDialog>

class QAbstractItemModel;
namespace Ui {
    class uno_search_form;
}

class unoSearchForm : public QDialog {
public:
    unoSearchForm(QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    virtual ~unoSearchForm();

    QAbstractItemModel* getDocTablesModel() const;
    void setDocTablesModel(QAbstractItemModel* docTablesModel);
    QString getSearchString() const;

private slots:
    void startSearch();
    
signals:

private:
    Ui::uno_search_form* UI;

private:
    Q_OBJECT
};
