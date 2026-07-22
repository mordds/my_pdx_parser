#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QtPlugin>
#include <QWidget>
#include <QLabel>
#include "../paradox_type.h"

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParadoxBase* test;
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "pdx_gui_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    
    QLabel q(&w);
    q.setText("<h1>Stupid Khet!</h1>");
    q.setGeometry(100,100,300,30);
    w.show();
    return QApplication::exec();
}
