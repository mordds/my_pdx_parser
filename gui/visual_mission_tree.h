#ifndef PDX_GUI_MT
#define PDX_GUI_MT
#include <QWidget>
#include <nlohmann/json.hpp>
#include <QString>
#include <vector>
#include <map>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFrame>
#include <QLabel>
#include <QComboBox>
#include <QListView>
#include <QListWidget>
#include <QCheckBox>
#include <QScrollArea>
#include <QObject>

using json = nlohmann::json;

struct VisualMissionTree;
struct VisualMissionTreeSegment;
struct VisualMissionTreeNode : public QObject{
    Q_OBJECT;
    public:
    QString name;
    VisualMissionTreeSegment* segment;
    QString icon_name;
    QString province_to_highlight;
    QString trigger_string;
    QString effect_string;
    std::vector<VisualMissionTreeNode*> required_mission;
    int x = -1;
    int y = -1;
    QPushButton* button;
    signals:
    void clicked(QString& name);
    public slots:
    void buttonClicked();
};

struct VisualMissionTreeSegment{
    QString name;
    QString potential_string;
    int backgroundColor = 0xD0D0D0;
    bool isGeneric;
    bool hasShield;
    bool avaliableForAI;
    

};
//mission_view & segment view
struct AutoTabTextEdit : public QPlainTextEdit {
    Q_OBJECT;
    public:
        explicit AutoTabTextEdit(QWidget *parent = nullptr);
        size_t old_block = -1;
    
    protected:
        virtual void keyPressEvent(QKeyEvent *e) override;
    public slots:
        void handle_new_block(int block_count);
};
//mission_edit
struct MissionEditView : public QFrame{
    Q_OBJECT;
    public:
    VisualMissionTree* link;
    QPlainTextEdit* province_to_highlight;
    QPlainTextEdit* trigger;
    QPlainTextEdit* effect;
    QListView* required_missions;
    QComboBox* select_mission;
    QPushButton* add_required;
    explicit MissionEditView(QWidget *parent = nullptr) : QFrame(parent){};
    public slots:    
};
struct MissionView : public QFrame{
    Q_OBJECT;
    public:
    QLabel* name_label;
    QLabel* segment_label;
    QLabel* required_label;
    QLabel* highlight_label;
    QLabel* x_label;
    QLabel* y_label;
    QLabel* trigger_label;
    QLabel* effect_label;
    QLabel* icon_label;
    QLineEdit* icon_edit;
    QLineEdit* name_edit;
    QComboBox* segment_combo;
    QListWidget* required_list;
    QComboBox* mission_combo;
    QPushButton* add_requirement_button;
    QPushButton* remove_requirement_button;
    QPlainTextEdit* highlight_edit;
    QLineEdit* x_edit;
    QLineEdit* y_edit;
    QPlainTextEdit* trigger_edit;
    QPlainTextEdit* effect_edit;
    QPushButton *confirm_button;
    QPushButton *cancel_button;
    VisualMissionTreeNode* activeNode;
    bool isEditOnly;
    explicit MissionView(QWidget *parent = nullptr);
    public slots:
    void cancel_button_click(); 
    void confirm_button_click();
    void x_edit_change();
    void y_edit_change();
    void icon_edit_change();

};
struct SegmentView : public QFrame{
    Q_OBJECT;
    public:
    VisualMissionTreeSegment* activeSegment;
    QLabel* name_label;
    QLabel* potential_label;
    QCheckBox* generic_checkbox;
    QCheckBox* ai_checkbox;
    QCheckBox* country_box;
    QLineEdit* name_edit;
    QLabel* color_label;
    QLabel* color_preview;
    QLineEdit* color_edit;
    QPushButton* color_button;
    QPlainTextEdit* potential_edit;
    QPushButton *confirm_button;
    QPushButton *cancel_button;
    bool isEditOnly;
    explicit SegmentView(QWidget *parent = nullptr);
    ~SegmentView(){};
    public slots:    
    void cancel_button_click();
    void confirm_button_click();
    void ai_checked();
    void country_checked();
    void generic_checked();
    void color_clicked();
    void color_changed();

};
struct MissionTreeVisualizer : public QFrame{
    Q_OBJECT;
    public:
    std::map<QString,VisualMissionTreeNode*>& nodes;
    explicit MissionTreeVisualizer(QWidget *parent,std::map<QString,VisualMissionTreeNode*>& Nodes) : QFrame(parent),nodes(Nodes){};
    virtual void paintEvent(QPaintEvent *event) override;
    public slots:
    void resize_to_fit();

    
};
struct VisualMissionTree : public QFrame{
    Q_OBJECT;
    public:
    std::set<QString> missions;
    std::set<QString> segments;
    std::map<QString,VisualMissionTreeNode*> missionMap;
    std::map<QString,VisualMissionTreeSegment*> segmentMap;
    SegmentView* add_segment_view;
    MissionView* add_mission_view;
    MissionTreeVisualizer* tree_frame;
    QScrollArea* tree_area;
    QListWidget* segments_list;
    QPushButton* load_from_file;
    QPushButton* save_to_file;
    QPushButton* write_to_file;
    QPushButton* switch_mode;
    QPushButton* add_mission;
    QPushButton* add_segment;
    QPushButton* edit_segment;
    bool isActive;
    bool segmentMode;
    explicit VisualMissionTree(QWidget *parent = nullptr);
    void hideButtons();
    void showButtons();
    public slots:
    void add_segment_cancel_callback();
    void add_mission_cancel_callback();
    void add_segment_confirm_callback();
    void add_mission_confirm_callback();
    void add_mission_combo_callback();
    void add_mission_segment_callback();
    void add_mission_add_require_callback();
    void tree_button_callback(QString& str);
    void add_mission_y_change();
    void add_mission_remove_require_callback();
    void add_segment_click();
    void add_mission_click();
    void switch_mode_click();
    void edit_segment_click();
    void save_file_click();
    void load_file_click();
    void write_file_click();
};

void to_json(json& data,const VisualMissionTree& vmt);

void from_json(const json& data,VisualMissionTree& vmt);

void export_as_pdx(std::string& path,VisualMissionTree& vmt);
#endif
