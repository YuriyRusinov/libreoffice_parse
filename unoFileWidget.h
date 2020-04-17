#pragma once

#include <QWidget>

class QToolBar;
class QTextEdit;

class unoFileWidget : public QWidget {
public:
    explicit unoFileWidget(QWidget* parent=nullptr, Qt::WindowFlags flags=Qt::WindowFlags());
    virtual ~unoFileWidget();

    void setText(const QString& text);

private slots:
    void slotFileOpen();
    void slotFileClose();

private:
    QToolBar* _tbActions;
    QTextEdit* _fileEditW;

private:
    Q_OBJECT
};
