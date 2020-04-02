#pragma once

#include <QMainWindow>

class QMdiArea;
class QProcess;

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

public slots:
private slots:
    void slotOpen();

private:
    Ui::uno_main_window* UI;
    QMdiArea* _mdiArea;
    QProcess* _sofficeProc;
};
