#ifndef PDX_GUI_HINT
#define PDX_GUI_HINT
#include "pdx_includes.h"
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <vector>
#include <map>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QLineEdit>

struct Hint : public QFrame {
    Q_OBJECT;
    public:
        QString name;
        QPushButton* linkedButton;
        explicit Hint(QString name,QWidget *parent = nullptr);
    signals:
        void selected(const QString& name);
    public slots:
        void linkedButtonSelected();
};

struct HintFrame : public QFrame {
    Q_OBJECT;
    public:
        explicit HintFrame(QWidget *parent = nullptr);
        void addHintItem(QString str,Hint* item);
        QScrollArea* select_area; 
        QHBoxLayout* select_layout;
        Hint* active_hint;
    private:
        std::map<QString,Hint*> items;
    public slots:
        void switchHint(const QString& hintName);

};

struct ScriptedTriggerHint : public Hint {
    Q_OBJECT;
    public:
        QString name;
        QLineEdit* search_st;
        QListWidget* st_list;
        QPlainTextEdit* st_description;
        explicit ScriptedTriggerHint(QString name,QWidget *parent = nullptr);
    public slots:
        void do_search();
        void show_st_info();
};
#endif