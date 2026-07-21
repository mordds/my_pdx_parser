#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "../paradox_type.h"
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
    w.show();
    return QApplication::exec();
}
