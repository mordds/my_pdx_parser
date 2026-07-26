#include "hint.h"

extern std::map<std::string,ScriptedTrigger*> loadedSTs;

Hint::Hint(QString _name,QWidget *parent) : name(_name), QFrame(parent) {

}
void Hint::linkedButtonSelected(){
    emit this->selected(this->name);
}
HintFrame::HintFrame(QWidget* parent) : QFrame(parent) {
    active_hint = nullptr;
    this->select_area = new QScrollArea(this);
    this->select_layout = new QHBoxLayout();
    this->select_area->setLayout(this->select_layout);
    this->select_area->setGeometry(0,190,800,30);
}
void HintFrame::addHintItem(QString name,Hint* item){
    if(item->linkedButton != nullptr) return;
    if(items.contains(name)) return;
    item->setParent(this);
    item->setGeometry(0,0,800,180);
    item->linkedButton = new QPushButton(this->select_area);
    item->setFixedSize(100,25);
    this->select_layout->addWidget(item->linkedButton);
    connect(item->linkedButton,SIGNAL(clicked()),item,SLOT(linkedButtonSelected()));
    connect(item,SIGNAL(selected(const QString&)),this,SLOT(switchHint(QString&)));
    items[name] = item;
}
void HintFrame::switchHint(const QString& name){
    if(!items.contains(name)) return;
    this->active_hint->hide();
    this->active_hint = this->items[name];
    this->active_hint->show();
}

ScriptedTriggerHint::ScriptedTriggerHint(QString name,QWidget *parent) : Hint(name,parent){
    this->st_list = new QListWidget(this);
    st_list->setGeometry(0,30,150,150);
    this->search_st = new QLineEdit(this);
    search_st->setGeometry(0,0,150,25);
    for(auto [name,st] : loadedSTs){
        st_list->addItem(QString::fromStdString(name));
    }
    this->st_description = new QPlainTextEdit(this);
    this->st_description->setReadOnly(true);
    this->st_description->setGeometry(180,0,150,180);
    connect(this->search_st,SIGNAL(textChanged(const QString&)),this,SLOT(do_search()));
    connect(this->st_list,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(show_st_info()));
}

void ScriptedTriggerHint::do_search(){
    const QString& text = this->search_st->text();
    for(int i = 0;i < st_list->count();i++){
        auto item = st_list->item(i);
        bool match = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
}
void ScriptedTriggerHint::show_st_info(){
    if(st_list->currentRow() == -1) return;
    std::string name = st_list->currentItem()->text().toStdString();
    if(!loadedSTs.contains(name)) return;
    ScriptedTrigger* st = loadedSTs[name];
    if(st->isFixed()) {
        FixedScriptedTrigger* fst = static_cast<FixedScriptedTrigger*>(st);
        this->st_description->setPlainText(QString::fromStdString(fst->instance->toString(false)));
    }
    else {
        auto cst = static_cast<ComplicateScriptedTrigger*>(st);
        std::string u = "format:\n";
        u.append(cst->name);
        u.append(" = {\n");
        for(auto name : cst->parameterName){
            u.append("\t");
            u.append(name);
            u.append(" = <");
            u.append(name);
            u.append(">");
            u.append("\n");
        }
        u.append("}");
        this->st_description->setPlainText(QString::fromStdString(u));
    }

}