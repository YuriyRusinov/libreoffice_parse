/*
 * Заголовочный файл главного окна тестового приложения
 * (C) НПО Рубин
 *
 * @author:
 *     Ю.Л.Русинов
 */

#pragma once

#include <QMainWindow>
#include <QProcess>

class QMdiArea;

class UnoSingleton;
class unoFileObject;

namespace Ui{
    class uno_main_window;
};

class UnoMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit UnoMainWindow(QWidget *parent = nullptr);
    virtual ~UnoMainWindow();

signals:
    void initUnoComponents();

public slots:
private slots:
    void slotOpen();

private:
    Ui::uno_main_window* _UI;
    QMdiArea* _mdiArea;

    bool _isInit;
    unoFileObject* _unoFObj;
};
