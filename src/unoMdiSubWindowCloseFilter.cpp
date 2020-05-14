/* @brief
 * Класс фильтрации событий закрытия дочернего окна QMdiSubWindow
 * (C) НИИ Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */
#include <QEvent>
#include <QtDebug>
#include <QMdiSubWindow>
#include "unoMdiSubWindowCloseFilter.h"

bool unoMdiSubWindowCloseFilter::eventFilter( QObject* obj, QEvent* event ) {
    switch (event->type())
    {
        case QEvent::Close:
        {
            QWidget* w = qobject_cast< QWidget* >(obj);
            QMdiSubWindow* subWindow = qobject_cast< QMdiSubWindow* >(w->parentWidget());
            //Q_ASSERT (subWindow != nullptr);

            //
            // do what you want here
            //
            qDebug() << __PRETTY_FUNCTION__;
            if (w && subWindow && subWindow->widget() == w)
            {
                event->accept();
                subWindow->close();
            }
            else
                event->ignore();

            break;
        }
        default:
            qt_noop();
    }
    return QObject::eventFilter(obj, event);
}
