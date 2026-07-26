#include "visual_mission_tree.h"
#include "../utils/string_util.h"
#include <algorithm>
#include <QPainter>
#include <QMessageBox>
#include <QColorDialog>
#include <QApplication>
#include <QFileDialog>
#include <QTextBlock>
#include <QDebug>
#include <fstream>

void VisualMissionTreeNode::buttonClicked(){
    emit clicked(this->name);
}
void MissionTreeVisualizer::resize_to_fit(){
    int y_max = -1;
    for(auto[name,node] : nodes){
        if(node == nullptr) continue;
        if(y_max < node->y) y_max = node->y;
    }
    int size = y_max * 75;
    if(size < 600) size = 600;
    this->resize(this->width(),size);
    this->update();
}
void MissionTreeVisualizer::paintEvent(QPaintEvent *event) {
    const static int offset[] = {30,37,45};
    const static int y_offset = 75;
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for(auto[name,node] : this->nodes){
        if(node->required_mission.size() == 0) continue;
        for(auto required : node->required_mission){
            if(required->x < node->x){ 
                QPoint points[4] = { {required->x * 75 - 30,required->y * 75 - 7},
                                     {required->x * 75 - 30,required->y * 75},
                                     {node->x * 75 - 45, required->y * 75},
                                     {node->x * 75 - 45, node->y * 75 - 68}
                                    };
                painter.drawPolyline(points,4);
            }
            else if(required->x == node->x){
                QPoint points[4] = { {required->x * 75 - 38,required->y * 75 - 7},
                                     {required->x * 75 - 38,required->y * 75},
                                     {node->x * 75 - 38, required->y * 75},
                                     {node->x * 75 - 38, node->y * 75 - 68}
                                    };
                painter.drawPolyline(points,4);                
            }
            else {
                QPoint points[4] = { {required->x * 75 - 45,required->y * 75 - 7},
                                     {required->x * 75 - 45,required->y * 75},
                                     {node->x * 75 - 30, required->y * 75},
                                     {node->x * 75 - 30, node->y * 75 - 68}
                                    };
                painter.drawPolyline(points,4);                              
            }
        }
    }
};
MissionView::MissionView(QWidget* parent) : QFrame(parent){
    this->name_label = new QLabel(this);
    name_label->setText("Mission Name");
    name_label->setGeometry(15,10,150,25);
    this->name_edit = new QLineEdit(this);
    name_edit->setGeometry(10,40,150,25);
    this->segment_label = new QLabel(this);
    segment_label->setText("Segment");
    segment_label->setGeometry(15,70,150,25);
    this->segment_combo = new QComboBox(this);
    segment_combo->setGeometry(10,100,150,25);
    this->x_label = new QLabel(this);
    x_label->setText("x");
    x_label->setGeometry(15,130,25,25);
    this->y_label = new QLabel(this);
    y_label->setText("y");
    y_label->setGeometry(105,130,25,25);   
    this->x_edit = new QLineEdit(this);
    x_edit->setGeometry(10,160,50,25);
    this->y_edit = new QLineEdit(this);
    y_edit->setGeometry(100,160,50,25);

    this->icon_label = new QLabel("Mission Icon",this);
    icon_label->setGeometry(10,190,150,25);
    this->icon_edit = new QLineEdit(this);
    icon_edit->setGeometry(10,220,150,25);

    this->required_label = new QLabel(this);
    required_label->setText("Mission Required");
    required_label->setGeometry(175,10,150,25);
    this->required_list = new QListWidget(this);
    required_list->setGeometry(170,40,150,210);
    this->mission_combo = new QComboBox(this);
    mission_combo->setGeometry(330,40,180,25);
    this->add_requirement_button = new QPushButton(this);
    add_requirement_button->setGeometry(330,90,180,25);
    add_requirement_button->setText("Add");
    this->remove_requirement_button = new QPushButton(this);
    remove_requirement_button->setGeometry(330,140,180,25);
    remove_requirement_button->setText("Remove");
    this->highlight_label = new QLabel(this);
    highlight_label->setText("Province to Highlight");
    highlight_label->setGeometry(525,10,200,25);
    this->highlight_edit = new AutoTabTextEdit(this);
    highlight_edit->setGeometry(520,40,200,210);
    highlight_edit->setTabStopDistance(20);
    highlight_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    highlight_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->trigger_label = new QLabel(this);
    trigger_label->setGeometry(15,260,330,25);
    trigger_label->setText("trigger");
    this->trigger_edit = new AutoTabTextEdit(this);
    trigger_edit->setGeometry(10,290,330,200);
    trigger_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    trigger_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    trigger_edit->setTabStopDistance(20);

    this->effect_label = new QLabel(this);
    effect_label->setGeometry(365,260,360,25);
    effect_label->setText("effect");
    this->effect_edit = new AutoTabTextEdit(this);
    effect_edit->setGeometry(360,290,360,200);
    effect_edit->setTabStopDistance(20);
    trigger_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    trigger_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    this->confirm_button = new QPushButton(this);
    confirm_button->setGeometry(610,500,80,25);
    confirm_button->setText("Confirm");
    this->cancel_button = new QPushButton(this);
    cancel_button->setGeometry(510,500,80,25);
    cancel_button->setText("Cancel");

    //connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancel_button_click()));
    connect(x_edit,SIGNAL(textChanged(const QString&)),this,SLOT(x_edit_change()));
    connect(y_edit,SIGNAL(textChanged(const QString&)),this,SLOT(y_edit_change()));
    connect(icon_edit,SIGNAL(textChanged(const QString&)),this,SLOT(icon_edit_change()));
    activeNode = nullptr;
} 

void MissionView::cancel_button_click(){
    if(!this->isEditOnly){
        delete activeNode;
        activeNode = nullptr;
    }

    this->name_edit->clear();
    this->effect_edit->clear();
    this->x_edit->clear();
    this->y_edit->clear();
    this->icon_edit->clear();
    this->trigger_edit->clear();
    this->highlight_edit->clear();
    this->required_list->clear();
    this->segment_combo->clear();
    this->mission_combo->clear();
    this->hide();
}
void MissionView::x_edit_change(){
    if(this->activeNode == nullptr) return;
    bool success = false;
    int x = this->x_edit->text().toInt(&success);
    if(!success) return;
    if(x > 5 || x <= 0) return;
    this->activeNode->x = x;
    if(this->activeNode->button != nullptr) {
        this->activeNode->button->move(75 * x - 68,this->activeNode->y * 75 - 68);
    }
}
void MissionView::y_edit_change(){
    if(this->activeNode == nullptr) return;
    bool success = false;
    int y = this->y_edit->text().toInt(&success);
    if(!success) return;
    if(y <= 0 || y > 30) return;
    this->activeNode->y = y;
    if(this->activeNode->button != nullptr) {
        this->activeNode->button->move(75 * this->activeNode->x - 68,y * 75 - 68);
    }
}
void MissionView::icon_edit_change(){
    if(this->activeNode == nullptr) return;
    this->activeNode->icon_name = this->icon_edit->text();
}
void MissionView::confirm_button_click(){
    activeNode = nullptr;

    this->name_edit->clear();
    this->effect_edit->clear();
    this->x_edit->clear();
    this->y_edit->clear();
    this->icon_edit->clear();
    this->trigger_edit->clear();
    this->highlight_edit->clear();
    this->required_list->clear();
    this->segment_combo->clear();
    this->mission_combo->clear();
    this->hide();
}

SegmentView::SegmentView(QWidget *parent) : QFrame(parent){
    this->name_label = new QLabel(this);
    name_label->setText("Segment Name");
    name_label->setGeometry(15,10,180,25);
    this->name_edit = new QLineEdit(this);
    name_edit->setGeometry(10,40,180,25);   
    this->generic_checkbox = new QCheckBox(this);
    generic_checkbox->setText("Is Generic");
    generic_checkbox->setGeometry(10,70,180,25);
    this->ai_checkbox = new QCheckBox(this);
    ai_checkbox->setText("Enabled for AI");
    ai_checkbox->setGeometry(10,100,180,25);     
    this->country_box = new QCheckBox(this);
    country_box->setText("Use Country Shield");
    country_box->setGeometry(10,130,180,25);
    this->potential_label = new QLabel(this);
    potential_label->setGeometry(200,10,200,25);
    potential_label->setText("Potential");
    this->potential_edit = new AutoTabTextEdit(this);
    potential_edit->setGeometry(200,40,200,120);
    potential_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
    potential_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    potential_edit->setTabStopDistance(20);
    this->color_label = new QLabel("color",this);
    color_label->setGeometry(410,10,50,25);
    this->color_preview = new QLabel(this);
    color_preview->setGeometry(410,40,25,25);
    color_preview->setStyleSheet("background-color: #D0D0D0;border:2px solid #000000");
    this->color_edit = new QLineEdit(this);
    color_edit->setGeometry(410,70,70,25);
    this->color_button = new QPushButton("C",this);
    color_button->setGeometry(450,40,25,25);
    
    this->activeSegment = nullptr;
    this->confirm_button = new QPushButton(this);
    confirm_button->setGeometry(320,180,80,25);
    confirm_button->setText("Confirm");
    this->cancel_button = new QPushButton(this);
    cancel_button->setGeometry(220,180,80,25);
    cancel_button->setText("Cancel");

    connect(country_box,SIGNAL(checkStateChanged(Qt::CheckState)),this,SLOT(country_checked()));
    connect(ai_checkbox,SIGNAL(checkStateChanged(Qt::CheckState)),this,SLOT(ai_checked()));
    connect(generic_checkbox,SIGNAL(checkStateChanged(Qt::CheckState)),this,SLOT(generic_checked()));
    connect(color_button,SIGNAL(clicked()),this,SLOT(color_clicked()));
    connect(color_edit,SIGNAL(textChanged(const QString&)),this,SLOT(color_changed()));
}

void SegmentView::cancel_button_click(){
    if(this->activeSegment != nullptr) {
        delete this->activeSegment;
        this->activeSegment = nullptr;
    }
    
    this->name_edit->clear();
    this->potential_edit->clear();
    this->hide();
}

void SegmentView::confirm_button_click(){
    this->activeSegment = nullptr;
    this->country_box->setChecked(false);
    this->ai_checkbox->setChecked(false);
    this->generic_checkbox->setChecked(false);
    this->name_edit->clear();
    this->potential_edit->clear();
    this->hide();
}
void SegmentView::ai_checked(){
    if(this->activeSegment != nullptr) this->activeSegment->avaliableForAI = this->ai_checkbox->isChecked();
}
void SegmentView::country_checked(){
    if(this->activeSegment != nullptr) this->activeSegment->hasShield = this->country_box->isChecked();
}
void SegmentView::generic_checked(){
    if(this->activeSegment != nullptr) this->activeSegment->isGeneric = this->generic_checkbox->isChecked();
}
void SegmentView::color_clicked(){
    QColor selected = QColorDialog::getColor();
    if(!selected.isValid()) return;
    QString color = "background-color:#";
    QRgb mRgb = qRgb(selected.red(), selected.green(), selected.blue());
    mRgb &= 0xFFFFFF;
    QString u = "";
    if(mRgb < 0x100000) u.append("0");
    if(mRgb < 0x10000) u.append("0");
    if(mRgb < 0x1000) u.append("0");
    if(mRgb < 0x100) u.append("0");
    if(mRgb < 0x10) u.append("0");
    u.append(QString::number(mRgb,16));
    this->color_edit->setText(u);
    color.append(u);
    color.append(";border:2px solid #000000");
    this->color_preview->setStyleSheet(color);
    if(this->activeSegment != nullptr) this->activeSegment->backgroundColor = mRgb;
}
void SegmentView::color_changed(){
    QString color = this->color_edit->text();
    bool success = false;
    int color_rgb = color.toInt(&success,16);
    if(!success) return;
    if(success > 0xFFFFFF) return;
    QString colorSheet = "background-color:#";
    QString u = QString::number(color_rgb,16);
    if(color_rgb < 0x100000) colorSheet.append("0");
    if(color_rgb < 0x10000) colorSheet.append("0");
    if(color_rgb < 0x1000) colorSheet.append("0");
    if(color_rgb < 0x100) colorSheet.append("0");
    if(color_rgb < 0x10) colorSheet.append("0");
    colorSheet.append(u);
    colorSheet.append(";border:2px solid #000000");
    this->color_preview->setStyleSheet(colorSheet);
    if(this->activeSegment != nullptr) this->activeSegment->backgroundColor = color_rgb;
}

VisualMissionTree::VisualMissionTree(QWidget *parent) : QFrame(parent){
    this->add_segment_view = new SegmentView(this);
    this->add_mission_view = new MissionView(this);
    this->add_segment_view->setGeometry(400,0,800,300);
    this->add_mission_view->setGeometry(400,0,800,600);
    this->tree_frame = new MissionTreeVisualizer(this,this->missionMap);
    this->tree_frame->setGeometry(12,0,375,600);
    this->tree_frame->show();
    this->tree_area = new QScrollArea(this);
    this->tree_area->setGeometry(0,0,400,610);
    this->tree_area->setWidget(tree_frame);
    this->add_mission_view->hide();
    this->add_segment_view->hide();
    this->isActive = false;
    this->segmentMode = false;
    this->segments_list = new QListWidget(this);
    this->segments_list->setGeometry(0,0,375,400);
    this->edit_segment = new QPushButton("Edit Segment",this);
    this->edit_segment->setGeometry(100,450,100,25);
    this->segments_list->hide();
    this->edit_segment->hide();
    connect(this->add_segment_view->cancel_button,SIGNAL(clicked()),this,SLOT(add_segment_cancel_callback()));
    connect(this->add_mission_view->cancel_button,SIGNAL(clicked()),this,SLOT(add_mission_cancel_callback()));
    connect(this->add_segment_view->confirm_button,SIGNAL(clicked()),this,SLOT(add_segment_confirm_callback()));
    connect(this->add_mission_view->confirm_button,SIGNAL(clicked()),this,SLOT(add_mission_confirm_callback()));
    connect(this->add_mission_view->y_edit,SIGNAL(textChanged(const QString&)),tree_frame,SLOT(resize_to_fit()));
    connect(this->add_mission_view->segment_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(add_mission_segment_callback()));
    connect(this->add_mission_view->add_requirement_button,SIGNAL(clicked()),this,SLOT(add_mission_add_require_callback()));
    connect(this->add_mission_view->remove_requirement_button,SIGNAL(clicked()),this,SLOT(add_mission_remove_require_callback()));
    connect(this->add_mission_view->y_edit,SIGNAL(textChanged(const QString&)),this,SLOT(add_mission_y_change()));
    this->add_segment = new QPushButton(this);
    add_segment->setGeometry(420,560,100,25);
    add_segment->setText("Add Segment");
    this->add_mission = new QPushButton(this);
    add_mission->setGeometry(540,560,100,25);
    add_mission->setText("Add Mission");
    this->switch_mode = new QPushButton("switch mode",this);
    switch_mode->setGeometry(660,560,100,25);
    this->save_to_file = new QPushButton("Save to File",this);
    save_to_file->setGeometry(420,530,100,25);
    this->load_from_file = new QPushButton("Load from File",this);
    load_from_file->setGeometry(540,530,100,25);
    this->write_to_file = new QPushButton("Export as PDX",this);
    write_to_file->setGeometry(660,530,100,25);
    connect(add_segment,SIGNAL(clicked()),this,SLOT(add_segment_click()));
    connect(add_mission,SIGNAL(clicked()),this,SLOT(add_mission_click()));
    connect(switch_mode,SIGNAL(clicked()),this,SLOT(switch_mode_click()));
    connect(edit_segment,SIGNAL(clicked()),this,SLOT(edit_segment_click()));
    connect(save_to_file,SIGNAL(clicked()),this,SLOT(save_file_click()));
    connect(load_from_file,SIGNAL(clicked()),this,SLOT(load_file_click()));
    connect(write_to_file,SIGNAL(clicked()),this,SLOT(write_file_click()));
}

void VisualMissionTree::hideButtons(){
    this->add_mission->hide();
    this->add_segment->hide();
    this->switch_mode->hide();
    this->save_to_file->hide();
    this->load_from_file->hide();
    this->write_to_file->hide();
}

void VisualMissionTree::showButtons(){
    this->add_mission->show();
    this->add_segment->show();
    this->switch_mode->show();
    this->save_to_file->show();
    this->load_from_file->show();
    this->write_to_file->show();
}
void VisualMissionTree::save_file_click(){
    QString file = QFileDialog::getSaveFileName(
        this,
        "Save Mission File",
        QDir::homePath() + "/untitled.json",
        "Json File(*.json);;All File (*.*)"
    );
    if(file.isEmpty()) return;
    json j = *this;
    std::string path = file.toStdString();
    std::ofstream fout(path);
    fout << j;
}
void VisualMissionTree::load_file_click(){
    if(!this->segments.empty() || !this->missions.empty()) {
        QMessageBox::StandardButton rb = QMessageBox::question(nullptr,
             "Open Mission File", "Open Mission File will erase ALL current data, are you sure you want to do that?"
             ,QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::No) return;
    }

    QString file = QFileDialog::getOpenFileName(
        this,
        "Select Mission File",
        QDir::homePath(),
        "Json File(*.json);;All File (*.*)"
    );
    if(file.isEmpty()) return;
    std::string path = file.toStdString();
    std::ifstream fin(path);
    json j;
    fin >> j;
    from_json(j,*this);
}
void VisualMissionTree::write_file_click(){
    QString file = QFileDialog::getSaveFileName(
        this,
        "Write PDX Mission File",
        QDir::homePath() + "/untitled_mission.txt",
        "Mission Tree File(*.txt);;All File (*.*)"
    );
    if(file.isEmpty()) return;
    std::string path = file.toStdString();
    export_as_pdx(path,*this);
}

void VisualMissionTree::add_segment_cancel_callback(){
    this->add_segment_view->cancel_button_click();
    showButtons();
    this->isActive = false;
}
void VisualMissionTree::add_mission_cancel_callback(){
    this->add_mission_view->cancel_button_click();
    showButtons();
    this->isActive = false;
}
void VisualMissionTree::add_mission_click(){
    if(this->isActive) return;
    if(this->segments.empty()) return;
    this->add_mission_view->segment_combo->addItem("");
    for(QString qstr : segments) {
        this->add_mission_view->segment_combo->addItem(qstr);
    }
    for(QString qstr : missions) {
        this->add_mission_view->mission_combo->addItem(qstr);
    }
    hideButtons();
    this->isActive = true;
    this->add_mission_view->isEditOnly = false;
    this->add_mission_view->activeNode = new VisualMissionTreeNode();
    this->add_mission_view->show();
    this->add_mission_view->cancel_button->show();

}
void VisualMissionTree::add_segment_click(){
    if(this->isActive) return;
    hideButtons();
    this->isActive = true;
    this->add_segment_view->activeSegment = new VisualMissionTreeSegment();
    this->add_segment_view->color_edit->setText("D0D0D0");
    this->add_segment_view->show();
    this->add_segment_view->isEditOnly = false;
    this->add_segment_view->cancel_button->show();
}

void VisualMissionTree::add_segment_confirm_callback(){
    if(!this->add_segment_view->isEditOnly){
        QString name = this->add_segment_view->name_edit->text();
        this->add_segment_view->activeSegment->name = name;
        this->add_segment_view->activeSegment->potential_string = this->add_segment_view->potential_edit->toPlainText();
        if(this->segments.find(name) != this->segments.end()) return;
        this->segments.insert(name);
        this->segmentMap[name] = this->add_segment_view->activeSegment;
        QListWidgetItem *item = new QListWidgetItem(name);
        
        item->setBackground(QColor(this->add_segment_view->activeSegment->backgroundColor));
        this->segments_list->addItem(item);
        this->add_segment_view->confirm_button_click();

    }
    else {
        QString prev_name = this->add_segment_view->activeSegment->name;
        QString name = this->add_segment_view->name_edit->text();
        this->add_segment_view->activeSegment->name = name;
        if(this->segments.find(name) != this->segments.end() && prev_name != name) return;
     
        
        if(prev_name != name){
            this->segments.erase(prev_name);
            this->segmentMap.erase(prev_name);
            for(int i = this->segments_list->count() - 1;i >= 0;i--){
                auto item = segments_list->item(i);
                if(item->text() == prev_name){
                    item = segments_list->takeItem(i);
                    delete item;
                }
            }
            this->segments.insert(name);
            QListWidgetItem *item = new QListWidgetItem(name);
            
            item->setBackground(QColor(this->add_segment_view->activeSegment->backgroundColor));
            this->segments_list->addItem(item);
            this->segmentMap[name] = this->add_segment_view->activeSegment;
        }
        else {
            for(int i = this->segments_list->count() - 1;i >= 0;i--){
                auto item = segments_list->item(i);
                if(item->text() == name){
                    //QMessageBox::information(nullptr,"test","Find!");
                    item->setBackground(QColor(this->add_segment_view->activeSegment->backgroundColor));
                }
            }           
        }
        auto segment = this->add_segment_view->activeSegment;
        for(auto[name,node] : this->missionMap){
            if(node == nullptr) continue;
            if(node->button == nullptr) continue;
            else if(node->segment == segment){
                QString colorSheet = "background-color:#";
                int color_rgb = segment->backgroundColor;
                QString colorString = QString::number(color_rgb,16);
                if(color_rgb < 0x100000) colorSheet.append("0");
                if(color_rgb < 0x10000) colorSheet.append("0");
                if(color_rgb < 0x1000) colorSheet.append("0");
                if(color_rgb < 0x100) colorSheet.append("0");
                if(color_rgb < 0x10) colorSheet.append("0");
                colorSheet.append(colorString);
                node->button->setStyleSheet(colorSheet);
            }
        }
        this->add_segment_view->activeSegment->potential_string = this->add_segment_view->potential_edit->toPlainText();
        
        this->add_segment_view->confirm_button_click();
        
    }
    this->isActive = false;
    showButtons();
    this->tree_frame->update();
}

void VisualMissionTree::add_mission_confirm_callback(){
    QString name = this->add_mission_view->name_edit->text();
    auto view = this->add_mission_view;
    if(this->missions.find(name) != this->missions.end() && name != view->activeNode->name) {
        QString info = "The Mission with ID \"";
        info.append(name);
        info.append("\" already exist!");
        QMessageBox::critical(nullptr,"Error",info);
        return;
    }
    if(this->add_mission_view->activeNode->segment == nullptr){
        QMessageBox::critical(nullptr,"error","You should assign a segment to this mission.");
        return;
    }
    if(view->activeNode->x == -1 || view->activeNode->y == -1){
        QMessageBox::critical(nullptr,"error","You should assign the position of this mission.");
        return;        
    }
    this->add_mission_view->activeNode->trigger_string = this->add_mission_view->trigger_edit->toPlainText();
    this->add_mission_view->activeNode->effect_string = this->add_mission_view->effect_edit->toPlainText();
    this->add_mission_view->activeNode->province_to_highlight = this->add_mission_view->highlight_edit->toPlainText();
    if(!this->add_mission_view->isEditOnly){
        //QString name = this->add_mission_view->name_edit->text();
        this->add_mission_view->activeNode->name = name;

        
        this->missions.insert(name);
        this->missionMap[name] = this->add_mission_view->activeNode;
        this->add_mission_view->activeNode->button = new QPushButton(this->tree_frame);
        auto *node = this->add_mission_view->activeNode;
        node->button->setGeometry(node->x * 75 - 68, node->y * 75 - 68, 60, 60);
        node->button->show();
        node->button->setText(name);
        node->button->setToolTip(name);
        QString colorSheet = "background-color:#";
        int color_rgb = node->segment->backgroundColor;
        QString colorString = QString::number(color_rgb,16);
        if(color_rgb < 0x100000) colorSheet.append("0");
        if(color_rgb < 0x10000) colorSheet.append("0");
        if(color_rgb < 0x1000) colorSheet.append("0");
        if(color_rgb < 0x100) colorSheet.append("0");
        if(color_rgb < 0x10) colorSheet.append("0");
        colorSheet.append(colorString);
        node->button->setStyleSheet(colorSheet);
        connect(node,SIGNAL(clicked(QString&)),this,SLOT(tree_button_callback(QString&)));
        connect(node->button,SIGNAL(clicked()),node,SLOT(buttonClicked()));
        this->add_mission_view->confirm_button_click();

    }
    else {
        QString prev_name = this->add_mission_view->activeNode->name;
        //QString name = this->add_mission_view->name_edit->text();
        if(this->missions.find(name) != this->missions.end() && name != prev_name) return;
        this->missions.erase(prev_name);
        this->missionMap.erase(prev_name);
        this->missions.insert(name);
        this->missionMap[name] = this->add_mission_view->activeNode;
        this->add_mission_view->activeNode->name = name;

        QString colorSheet = "background-color:#";
        this->add_mission_view->activeNode->button->setText(this->add_mission_view->activeNode->name);
        this->add_mission_view->activeNode->button->setToolTip(this->add_mission_view->activeNode->name);
        int color_rgb = this->add_mission_view->activeNode->segment->backgroundColor;
        QString colorString = QString::number(color_rgb,16);
        if(color_rgb < 0x100000) colorSheet.append("0");
        if(color_rgb < 0x10000) colorSheet.append("0");
        if(color_rgb < 0x1000) colorSheet.append("0");
        if(color_rgb < 0x100) colorSheet.append("0");
        if(color_rgb < 0x10) colorSheet.append("0");
        colorSheet.append(colorString);
        this->add_mission_view->activeNode->button->setStyleSheet(colorSheet);
        this->add_mission_view->activeNode->button->setText(name);
        this->add_mission_view->activeNode->button->setToolTip(name);
        this->add_mission_view->confirm_button_click();
    }
    showButtons();
    this->isActive = false;
}

void VisualMissionTree::add_mission_add_require_callback(){

    MissionView* view = this->add_mission_view;
    if(view->mission_combo->count() == 0) return;
    QString str = view->mission_combo->currentText();
    if(!this->missionMap.contains(str)) return;
    auto mission_ptr = this->missionMap[str];
    
    view->mission_combo->removeItem(view->mission_combo->currentIndex());
    for(int i = 0;i < view->activeNode->required_mission.size();i++){
        if(view->activeNode->required_mission[i] == mission_ptr) return;
    }
    view->activeNode->required_mission.push_back(mission_ptr);
    this->tree_frame->update();
    view->required_list->addItem(str);
}
void VisualMissionTree::add_mission_remove_require_callback(){
    MissionView* view = this->add_mission_view;
    if(view->required_list->currentRow() == -1) return;
    QString str = view->required_list->currentItem()->text();
    int row = view->required_list->currentRow();
    QListWidgetItem *item = view->required_list->takeItem(row);
    if(item != nullptr)  {
        delete item;
        item = nullptr;
    }
    view->mission_combo->addItem(str);
    int i;
    for(i = 0;i < view->activeNode->required_mission.size();i++){
        if(view->activeNode->required_mission[i]->name == str) break;
    }
    view->activeNode->required_mission.erase(view->activeNode->required_mission.begin() + i);
    this->tree_frame->update();
}
void VisualMissionTree::add_mission_combo_callback(){
    if(this->add_mission_view->activeNode == nullptr) return;
    MissionView* view = this->add_mission_view;
    if(view->segment_combo->count() == 0) return;
    QString str = view->segment_combo->currentText();
    if(!this->segmentMap.contains(str)) return;
    auto segment_ptr = this->segmentMap[str];    
    view->activeNode->segment = segment_ptr;
}
void VisualMissionTree::add_mission_segment_callback(){
    if(this->add_mission_view->activeNode == nullptr) return;
    QString name = this->add_mission_view->segment_combo->currentText();
    if(!this->segmentMap.contains(name)) return;
    auto seg = segmentMap[name];
    this->add_mission_view->activeNode->segment = seg;
}
void VisualMissionTree::tree_button_callback(QString& str){
    if(this->isActive) return;
    if(!missionMap.contains(str)) return;
    auto mission = missionMap[str];
    hideButtons();
    this->add_mission_view->segment_combo->addItem("");
    for(QString qstr : segments) {
        auto combo = this->add_mission_view->segment_combo;
        combo->addItem(qstr);
        if(mission->segment != nullptr && mission->segment->name == qstr) combo->setCurrentIndex(combo->count() - 1);
    }
    for(auto[qstr,node] : missionMap) {
        if(node == mission) continue;
        if(node->y >= mission->y) continue;
        if(std::find(mission->required_mission.begin(),mission->required_mission.end(),node) != mission->required_mission.end()) {
            this->add_mission_view->required_list->addItem(qstr);
        }
        else this->add_mission_view->mission_combo->addItem(qstr);
    }
    this->isActive = true;
    this->add_mission_view->name_edit->setText(mission->name);
    this->add_mission_view->x_edit->setText(QString::number(mission->x));
    this->add_mission_view->y_edit->setText(QString::number(mission->y));
    this->add_mission_view->highlight_edit->setPlainText(mission->province_to_highlight);
    this->add_mission_view->trigger_edit->setPlainText(mission->trigger_string);
    this->add_mission_view->effect_edit->setPlainText(mission->effect_string);
    this->add_mission_view->icon_edit->setText(mission->icon_name);
    this->add_mission_view->isEditOnly = true;
    this->add_mission_view->activeNode = mission;
    this->add_mission_view->cancel_button->hide();
    this->add_mission_view->show();    
}

void VisualMissionTree::switch_mode_click(){
    if(this->isActive) return;
    if(this->segmentMode){
        this->segmentMode = false;
        this->tree_area->show();
        this->segments_list->hide();
        this->edit_segment->hide();
    }
    else {
        this->segmentMode = true;
        this->tree_area->hide();
        this->segments_list->show();
        this->edit_segment->show();
    }
}
void VisualMissionTree::edit_segment_click(){
    if(this->isActive) return;
    if(segments_list->count() == 0) return;
    if(segments_list->currentRow() == -1) return;
    QString seg_name = segments_list->currentItem()->text();
    if(!segmentMap.contains(seg_name)) return;
    auto seg_ptr = segmentMap[seg_name];
    this->isActive = true;
    auto view = this->add_segment_view;
    view->isEditOnly = true;
    view->show();
    view->cancel_button->hide();
    view->name_edit->setText(seg_ptr->name);
    view->potential_edit->setPlainText(seg_ptr->potential_string);
    view->country_box->setChecked(seg_ptr->hasShield);
    view->generic_checkbox->setChecked(seg_ptr->isGeneric);
    view->ai_checkbox->setChecked(seg_ptr->avaliableForAI);
    view->color_edit->setText(QString::number(seg_ptr->backgroundColor,16));
    QString color = "background-color:#";
    color.append(QString::number(seg_ptr->backgroundColor,16));
    color.append(";border:2px solid #000000");
    view->color_preview->setStyleSheet(color);
    view->activeSegment = seg_ptr;
    hideButtons();
}

void VisualMissionTree::add_mission_y_change(){
    
    auto view = this->add_mission_view;
    if(view->activeNode == nullptr) return;
    bool success = false;
    int y = view->y_edit->text().toInt(&success);
    if(!success) return;
    if(y <= 0 || y > 30) return;
    view->mission_combo->clear();
    for(auto [name,mission] : this->missionMap){
        if(mission == view->activeNode) continue;
        if(mission->y >= view->activeNode->y) continue;
        if(std::find(mission->required_mission.begin(),mission->required_mission.end(),mission) != mission->required_mission.end()) {
            this->add_mission_view->required_list->addItem(name);
        }
        else view->mission_combo->addItem(name);
    }
}

void to_json(json& data,const VisualMissionTree& vmt){
    std::vector<std::string> missions;
    std::vector<std::string> segments;
    segments.reserve(8); //for most mt this is enough
    missions.reserve(vmt.missions.size());
    for(auto str : vmt.missions){
        missions.push_back(str.toStdString());
    }
    for(auto str: vmt.segments){
        segments.push_back(str.toStdString());
    }
    data["mission_names"] = missions;
    data["segment_names"] = segments;
    for(auto [name,node] : vmt.missionMap){
        std::vector<std::string> required_mission;
        for(auto str : node->required_mission) required_mission.push_back(str->name.toStdString());
        std::string sName = name.toStdString();
        data["missions"][sName]["required_mission"] = required_mission;
        data["missions"][sName]["segment"] = node->segment->name.toStdString();
        data["missions"][sName]["icon_name"] = node->icon_name.toStdString();
        data["missions"][sName]["x"] = node->x;
        data["missions"][sName]["y"] = node->y;
        data["missions"][sName]["highlight"] = node->province_to_highlight.toStdString();
        data["missions"][sName]["trigger"] = node->trigger_string.toStdString();
        data["missions"][sName]["effect"] = node->effect_string.toStdString();
    }
    for(auto [name,segment] : vmt.segmentMap){
        std::string sName = name.toStdString();
        data["segment"][sName]["ai"] = segment->avaliableForAI;
        data["segment"][sName]["generic"] = segment->isGeneric;
        data["segment"][sName]["shield"] = segment->hasShield;
        data["segment"][sName]["potential"] = segment->potential_string.toStdString();
        data["segment"][sName]["color"] = segment->backgroundColor;
    }
}

void from_json(const json& data,VisualMissionTree& vmt){
    //first clear visual mission tree
    vmt.tree_area->hide();
    vmt.segments_list->hide();
    vmt.edit_segment->hide();
    vmt.segments.clear();
    vmt.missions.clear();
    for(auto [name,node] : vmt.missionMap){
        if(node == nullptr) continue;
        if(node->button != nullptr){
            node->button->setParent(nullptr);
            delete node->button;
        }
        delete node;
    }
    vmt.missionMap.clear();
    vmt.segments_list->clear();
    for(auto [name,segment] : vmt.segmentMap){
        delete segment;
    }
    vmt.segmentMap.clear();
    //create base object
    for(std::string segment_name : data["segment_names"]){
        auto segment = new VisualMissionTreeSegment();
        segment->name = QString::fromStdString(segment_name);
        segment->avaliableForAI = data["segment"][segment_name]["ai"];
        segment->isGeneric = data["segment"][segment_name]["generic"];
        segment->hasShield = data["segment"][segment_name]["shield"];
        std::string tmp = data["segment"][segment_name]["potential"];
        segment->potential_string = QString::fromStdString(tmp);
        segment->backgroundColor = data["segment"][segment_name]["color"];
        auto item = new QListWidgetItem(segment->name);
        item->setBackground(QColor(segment->backgroundColor));
        vmt.segments_list->addItem(item);
        vmt.segments.insert(segment->name);
        vmt.segmentMap[segment->name] = segment;
    }
    for(std::string mission_name : data["mission_names"]){
        auto mission = new VisualMissionTreeNode();
        mission->name = QString::fromStdString(mission_name);
        vmt.missionMap[mission->name] = mission;
        vmt.missions.insert(mission->name);
    }
    for(QString mission_name : vmt.missions){
        std::string name = mission_name.toStdString();
        auto mission = vmt.missionMap[mission_name];
        mission->button = new QPushButton(mission->name,vmt.tree_frame);
        mission->button->setToolTip(mission->name);
        std::string segment_name = data["missions"][name]["segment"];
        QString q_segment_name = QString::fromStdString(segment_name);
        auto segment = vmt.segmentMap[q_segment_name];
        mission->segment = segment;
        QString colorSheet = "background-color:#";
        int color_rgb = mission->segment->backgroundColor;
        QString colorString = QString::number(color_rgb,16);
        if(color_rgb < 0x100000) colorSheet.append("0");
        if(color_rgb < 0x10000) colorSheet.append("0");
        if(color_rgb < 0x1000) colorSheet.append("0");
        if(color_rgb < 0x100) colorSheet.append("0");
        if(color_rgb < 0x10) colorSheet.append("0");
        colorSheet.append(colorString);
        mission->button->setStyleSheet(colorSheet);
        
        QObject::connect(mission,SIGNAL(clicked(QString&)),&vmt,SLOT(tree_button_callback(QString&)));
        QObject::connect(mission->button,SIGNAL(clicked()),mission,SLOT(buttonClicked()));
        std::vector<std::string> required_mission_raw = data["missions"][name]["required_mission"];
        for(auto str : required_mission_raw){
            mission->required_mission.push_back(vmt.missionMap[QString::fromStdString(str)]);
        }
        mission->x = data["missions"][name]["x"];
        mission->y = data["missions"][name]["y"];
        mission->button->setGeometry(mission->x * 75 - 68,mission->y * 75 - 68,60,60);
        std::string tmp = data["missions"][name]["icon_name"];
        mission->icon_name = QString::fromStdString(tmp);
        tmp = data["missions"][name]["highlight"];
        mission->province_to_highlight = QString::fromStdString(tmp);
        tmp = data["missions"][name]["trigger"];
        mission->trigger_string= QString::fromStdString(tmp);
        tmp = data["missions"][name]["effect"];
        mission->effect_string = QString::fromStdString(tmp);
    }
    vmt.tree_area->show();
    vmt.tree_frame->resize_to_fit();
}

void export_as_pdx(std::string& path,VisualMissionTree& vmt){
    std::ofstream fout(path);
    auto comparator = [](VisualMissionTreeNode* a,VisualMissionTreeNode* b){
        return a->y < b->y;
    };
    for(auto [name,seg] : vmt.segmentMap){
        if(seg == nullptr) continue;
        std::vector<VisualMissionTreeNode*> nodes[5];
        for(auto [_name,node] : vmt.missionMap){
            if(node == nullptr) continue;
            if(node->segment == seg) nodes[node->x - 1].push_back(node);
        }
        for(int i = 0;i < 5;i++){
            if(nodes[i].size() <= 1) continue;
            std::sort(nodes[i].begin(),nodes[i].end(),comparator);
        }
        for(int i = 0;i < 5;i++){
            if(nodes[i].empty()) continue;
            fout << seg->name.toStdString() << i + 1 << " = {\n";
            fout << "\tslot = " << i + 1 << '\n' ;
            fout << "\tai = " << (seg->avaliableForAI ? "yes\n" : "no\n"); 
            fout << "\tgeneric = " << (seg->isGeneric ? "yes\n" : "no\n"); 
            fout << "\thas_country_shield = " << (seg->hasShield ? "yes\n" : "no\n");
            std::string out = seg->potential_string.toStdString();
            replaceWith(out,"\n","\n\t\t");
            while(out.back() == '\t') out.pop_back();
            if(out.back() == '\n') out.pop_back();
            fout << "\tpotential = {\n\t\t" << out << "\n\t}\n";
            fout.flush();
            for(int j = 0;j < nodes[i].size();j++){
                out = nodes[i][j]->name.toStdString();
                fout << "\t" << out << " = {\n";
                fout << "\t\tposition = " << nodes[i][j]->y << "\n";
                if(!nodes[i][j]->icon_name.isEmpty()){
                    fout << "\t\ticon = " << nodes[i][j]->icon_name.toStdString() << "\n";
                }
                if(!nodes[i][j]->required_mission.empty()){
                    fout <<  "\t\trequired_missions = {\n";
                    for(auto required : nodes[i][j]->required_mission){
                        fout << "\t\t\t" << required->name.toStdString() << "\n";
                    }
                    fout << "\t\t}\n";
                }

                if(!nodes[i][j]->province_to_highlight.isEmpty()){
                    out = nodes[i][j]->province_to_highlight.toStdString();
                    replaceWith(out,"\n","\n\t\t\t");
                    while(out.back() == '\t') out.pop_back();
                    if(out.back() == '\n') out.pop_back();
                    fout << "\t\tprovince_to_highlight = {\n\t\t\t" << out << "\n\t\t}\n";
                }
                if(nodes[i][j]->trigger_string.isEmpty()) fout << "\t\ttrigger = {}\n";
                else {
                    out = nodes[i][j]->trigger_string.toStdString();
                    replaceWith(out,"\n","\n\t\t\t");
                    while(out.back() == '\t') out.pop_back();
                    if(out.back() == '\n') out.pop_back();
                    fout << "\t\ttrigger = {\n\t\t\t" << out << "\n\t\t}\n";
                }
                if(nodes[i][j]->effect_string.isEmpty()) fout << "\t\teffect = {}\n";
                else {
                    out = nodes[i][j]->effect_string.toStdString();
                    replaceWith(out,"\n","\n\t\t\t");
                    while(out.back() == '\t') out.pop_back();
                    if(out.back() == '\n') out.pop_back();
                    fout << "\t\teffect = {\n\t\t\t" << out << "\n\t\t}\n";
                }
                fout << "\t}" << std::endl;
            }
            fout << "}" << std::endl; 
        }
    }
}

AutoTabTextEdit::AutoTabTextEdit(QWidget *parent) : QPlainTextEdit(parent){
    connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(handle_new_block(int)));
};
void AutoTabTextEdit::keyPressEvent(QKeyEvent *e){
    if(e->key() != Qt::Key_Tab) {
        QPlainTextEdit::keyPressEvent(e);
        return;
    }
    if(!this->textCursor().hasSelection()){
        QPlainTextEdit::keyPressEvent(e);
        return;        
    }
    QTextCursor cursor = this->textCursor();

    cursor.beginEditBlock();

    int endNumber = cursor.selectionEnd();
    cursor.setPosition(cursor.selectionStart());
    
    cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock);
    
    while(cursor.position() <= endNumber + 1){
        
        cursor.insertText("\t");
        if(cursor.blockNumber() == this->blockCount()) break;
        cursor.movePosition(QTextCursor::MoveOperation::NextBlock);
        //cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock);
    }
    cursor.insertText("\t");
    cursor.endEditBlock();
}
void AutoTabTextEdit::handle_new_block(int block_count){
    if(old_block <= 0) {
        old_block = block_count;
        return;
    }
    if(block_count == old_block + 1){
        QTextCursor cursor = this->textCursor();
        int current_block = cursor.blockNumber();
        //qDebug() << "called!";
        if(current_block == 0) return;
        QString tabString = "";
        QTextBlock block =  this->document()->findBlockByNumber(current_block - 1);
        QString str = block.text();
        block = block.next();
        QString current_string = block.text();
        for(int i = 0;i < str.length();i++){
            if(str[i] == '\t') tabString.append('\t');
            else break;
        }   
        if(str.endsWith("{")) {
            if(!current_string.endsWith("}")) tabString.append('\t');
        }
        cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock);
        cursor.insertText(tabString);
    }
    old_block = block_count;
}