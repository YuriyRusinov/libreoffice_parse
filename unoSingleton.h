/*
 * Синглтон библиотеки, запускающий необходимое фоновое приложение 
 * e.g. soffice
 *
 */
#pragma once

#include <QObject>
#include <QProcess>

class QProcess;

class UnoSingleton : public QObject {
public:
    static UnoSingleton* getUnoS (QObject* parent=nullptr);

    void initProc(const QString& program, const QStringList& arguments);
    void startProc();

    QProcess* getProc() const;

private slots:
    void slotProcEnd(int exitCode, QProcess::ExitStatus exitStatus);

private:
    UnoSingleton(QObject* parent=nullptr);
    UnoSingleton(const UnoSingleton& S) = delete;
    UnoSingleton& operator=(const UnoSingleton& S) = delete;
    ~UnoSingleton();

    QProcess* _sofficeProc;

    static UnoSingleton* _instance;
private:
    Q_OBJECT
};
