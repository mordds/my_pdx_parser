#define QT_NO_FOREACH
#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QtPlugin>
#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <thread>
#include "pdx_includes.h"

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

void init(){
    using namespace std;
    auto start = std::chrono::system_clock::now();
	registerInternalScopes();
    std::thread& th = readLocalizations("");
    loadInternalModifier();
    registerGood();
    log_info(current_location(),"Modifier Loaded!");
	registerTriggerItems();
	log_info(current_location(),"Trigger Loaded!");
	loadScriptedTrigger();
	log_info(current_location(),"Scripted Trigger Phase 1 Loaded!");
	registerEffectItems();
	log_info(current_location(),"Effect Loaded!");
	loadNationalIdea();
	log_info(current_location(),"Ni Loaded!");
	th.join();
	loadScriptedTrigger_POST();
	log_info(current_location(),"Scripted Trigger Phase 2 Loaded!");
	auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	log_info(current_location(),"Load Completed! ",duration.count(), " us consumed.");    
}



int main(int argc, char *argv[])
{
    init();
    QApplication a(argc, argv);
    
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
    //QPushButton *button = new QPushButton("Hello!",&w);
    //button->setGeometry(20,600,200,220);
    w.show();
    
    return QApplication::exec();
}


