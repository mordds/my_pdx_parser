#ifndef PDX_DB_OBJ
#define PDX_DB_OBJ
#include<string>
enum DataBaseObjType{
    GOOD,
    INSTITUTION,
    ADVISOR_TYPE,
    BUILDING,
    RELIGION,
    SUBJECT_TYPE,
    IDEA_GROUP
};

struct ParadoxDataBaseObj{
    virtual DataBaseObjType getType() = 0;   
    std::string localizedName;
};
struct Good : ParadoxDataBaseObj{
    virtual DataBaseObjType getType() {
        return DataBaseObjType::GOOD;
    }
    int defaultPrice;
};
struct Religion : ParadoxDataBaseObj{
    virtual DataBaseObjType getType() {
        return DataBaseObjType::RELIGION;
    }
};
struct SubjectType : ParadoxDataBaseObj {
    virtual DataBaseObjType getType() {
        return DataBaseObjType::SUBJECT_TYPE;
    }
};
struct IdeaGroup : ParadoxDataBaseObj {
    virtual DataBaseObjType getType() {
        return DataBaseObjType::IDEA_GROUP;
    }
};
struct Building : ParadoxDataBaseObj {
    virtual DataBaseObjType getType(){
        return DataBaseObjType::BUILDING;
    }
};
struct AdvisorType : ParadoxDataBaseObj{
    virtual DataBaseObjType getType(){
        return DataBaseObjType::ADVISOR_TYPE;
    }
};
struct Institution : ParadoxDataBaseObj{
    virtual DataBaseObjType getType(){
        return DataBaseObjType::INSTITUTION;
    }
};
#endif