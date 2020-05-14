/* @brief
 * Класс фильтрации событий закрытия дочернего окна QMdiSubWindow
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#pragma once

#include <QObject>

class unoMdiSubWindowCloseFilter : public QObject {
protected:
    bool eventFilter( QObject* obj, QEvent* event ) override;
private:
    Q_OBJECT
};
