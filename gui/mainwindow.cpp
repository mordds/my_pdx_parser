#define QT_NO_FOREACH
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "pdx_includes.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{       ui->setupUi(this);
        //this->nmv = new MissionView(this);
        //this->nmv->setGeometry(0,0,800,600);
        //this->nsv = new SegmentView(this);
        //this->nsv->setGeometry(0,0,800,300);
        this->vmt = new VisualMissionTree(this);
        this->vmt->setGeometry(0,0,1200,600);
        this->hint_frame = new HintFrame(this);
        this->hint_frame->addHintItem("scripted trigger",new ScriptedTriggerHint("scripted trigger",this->hint_frame));
        this->hint_frame->setGeometry(0,600,1200,220);
    }

MainWindow::~MainWindow()
{
    delete ui;
}
/*
void MissionTreeVisualizer::paintEvent(QPaintEvent *event){
    
}
*/

