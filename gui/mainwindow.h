#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define QT_NO_FOREACH
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QMainWindow>
#include "visual_mission_tree.h"
#include "hint.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    VisualMissionTree* vmt;
    HintFrame* hint_frame;
    //QListWidget* list;
    explicit MainWindow(QWidget *parent = nullptr);

    

    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
private slots:
    
};
#endif // MAINWINDOW_H
