#include<vector>
#include "paradox_type.h"
#include "trigger.h"
#include "pattern.h"
#include "scope.h"
#include "localization.h"
#include "paradox_macro.h"
#include<map>
#include<iostream>
#include<set>

using OverrideHandler = bool(*)(std::map<std::string,ParadoxBase*>&);

std::map<std::string,std::string> numberRequiredItems;
std::set<std::string> simpleTriggers;
std::set<std::string> registeredTriggers;
std::map<std::string,OverrideHandler> overrideHandlers;
std::map<std::string,TriggerItem*> items;

extern std::map<std::string,ScriptedTrigger*> loadedSTs;

bool parseConditionalTrigger(ParadoxTag*,ConditionalTrigger*);

extern ParadoxString* createString(std::string str);

const ParadoxType INTEGER_MATCH_SEQUENCE[] = {ParadoxType::INTEGER,ParadoxType::SCOPE};
const ParadoxType STRING_MATCH_SEQUENCE[] = {ParadoxType::SCOPE,ParadoxType::STRING};
//No arg means only yes/no or even only allowing yes.
//those trigger will be registered as 1 BOOL type args.
//and no args will be used in localization text
void registerNoArgTrigger(std::string name,std::string pattern,std::string reversePattern,ScopeType scopeType = ScopeType::COUNTRY){
	TriggerItem* item = new TriggerItem(registerShortString(name));
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(ParadoxType::BOOLEAN);
	item->usable_scope = scopeType;
	items[name] = item;
	simpleTriggers.insert(name);
	registeredTriggers.insert(name);
}
//1 arg type.
//arg 0 will be used in patterns.
void registerSimpleTrigger(std::string name,std::string pattern,std::string reversePattern,ParadoxType type,ScopeType scopeType = ScopeType::COUNTRY){
	
	TriggerItem* item = new TriggerItem(registerShortString(name));
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(type);
	item->usedParameter.push_back(0);
	item->usable_scope = scopeType;
	items[name] = item;
	registeredTriggers.insert(name);
	simpleTriggers.insert(name);
}

void registerSimpleClauseTrigger(std::string name,TriggerItem* triggerItem){
	simpleTriggers.insert(registerShortString(name));
	items[name] = triggerItem;
	registeredTriggers.insert(name);
}
void registerNumberRequiredTrigger(std::string name,std::string amountKey,std::string pattern,std::string reversePattern,ScopeType scopeType = ScopeType::COUNTRY){
	TriggerItem* item = new TriggerItem(registerShortString(name));
	item->reversePattern = reversePattern;
	item->pattern = pattern;
	item->parameterType.push_back(ParadoxType::INTEGER);
	item->usedParameter.push_back(0);
	item->usable_scope = scopeType;
	numberRequiredItems[name] = amountKey;
	items[name] = item;
	registeredTriggers.insert(name);
}
void registerBooleanTrigger(std::string name,std::string pattern,std::string reversePattern,ScopeType scopeType = ScopeType::COUNTRY){
	std::string actual_name(name);
	actual_name.append("@");
	actual_name.append(std::to_string(static_cast<int>(ParadoxType::BOOLEAN)));
	TriggerItem* item = new TriggerItem(registerShortString(name));
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->usable_scope = scopeType;
	item->parameterType.push_back(ParadoxType::BOOLEAN);
	items[actual_name] = item;
	registeredTriggers.insert(name);

}
void registerSingleArgTrigger(std::string name,std::string pattern,std::string reversePattern,ParadoxType type,ScopeType scopeType = ScopeType::COUNTRY){
	if(type == ParadoxType::BOOLEAN){
		registerBooleanTrigger(name,pattern,reversePattern);
		return;
	}
	
	std::string actual_name(name);
	actual_name.append("@");
	actual_name.append(std::to_string(static_cast<int>(type)));
	TriggerItem* item = new TriggerItem(registerShortString(name));
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(type);
	item->usedParameter.push_back(0);
	item->usable_scope = scopeType;
	items[actual_name] = item;
	registeredTriggers.insert(name);
}
void registerClausedTrigger(std::string name,TriggerItem* item,OverrideHandler handler){
	items[name] = item;
	overrideHandlers[name] = handler;
	registeredTriggers.insert(name);
}

void registerTriggerItems(){
	registerNoArgTrigger("ai","是AI","不是AI");
	registerNoArgTrigger("allows_female_emperor","允许女性皇帝","不允许女性皇帝");
	registerNoArgTrigger("always","总是为真","总是为假");
	registerNoArgTrigger("at_war_with_religious_enemy","与宗教敌人处于战争状态","没有与宗教敌人处于战争状态");
	registerNoArgTrigger("can_heir_be_child_of_consort","继承人可能是配偶的孩子","继承人不可能是配偶的孩子");
	registerNoArgTrigger("can_migrate","可以移民","不可以移民");
	registerNoArgTrigger("exist","存在","不存在");
	registerNoArgTrigger("has_active_debate","有正在进行中的辩论","没有正在进行中的辩论");
	registerBooleanTrigger("has_active_fervor","激活了一个热情效果","没有激活热情效果");
	registerSingleArgTrigger("has_active_fervor","拥有已激活的\"%s\"热情效果","没有已激活的\"%s\"热情效果",ParadoxType::STRING);
	registerNoArgTrigger("has_advisor","已经雇佣了一个顾问","已经雇佣了一个顾问");
	registerNoArgTrigger("has_any_active_estate_agenda","有进行中的阶层议程","没有进行中的阶层议程");
	registerNoArgTrigger("has_any_disaster","当前处于灾难状态","当前没有处于灾难状态");
	registerNoArgTrigger("has_cardinal","拥有在职的真知者","没有在职的真知者");
	registerNoArgTrigger("has_changed_nation","改变过游玩国家","从未改变过游玩国家");
	registerNoArgTrigger("has_colonist","有一个活跃的殖民队","没有活跃的殖民队");
	registerNoArgTrigger("has_commanding_three_star","有正在指挥的三星陆军或海军将领","没有正在指挥的三星陆军或海军将领");
	registerNoArgTrigger("has_consort","有配偶","没有配偶");
	registerNoArgTrigger("has_consort_regency","处于配偶摄政","没有处于配偶摄政");
	registerNoArgTrigger("has_custom_ideas","使用了自定义国家理念","没有使用自定义国家理念");
	registerNoArgTrigger("has_divert_trade","已经向宗主国转移贸易力量","没有向宗主国转移贸易力量");
	registerNoArgTrigger("has_embargo_rivals","已禁运宗主国的宿敌","没有禁运宗主国的宿敌");
	registerNoArgTrigger("has_estate_loans","有阶层贷款","没有阶层贷款");
	registerNoArgTrigger("has_factions","有派系","没有派系");
	registerNoArgTrigger("has_first_revolution_started","世界上已经爆发过革命","世界上没有爆发过革命");
	registerNoArgTrigger("has_female_consort","有女性配偶","没有女性配偶");
	registerNoArgTrigger("has_female_heir","有女性继承人","没有女性继承人");
	registerNoArgTrigger("has_flagship","拥有旗舰","没有拥有旗舰");
	registerNoArgTrigger("has_foreign_consort","有一个外国的配偶","没有一个外国的配偶");
	registerNoArgTrigger("has_foreign_heir","继承人是外国人","继承人不是外国人");
	registerNoArgTrigger("has_friendly_reformation_center","有一个当前宗教的宗教改革中心","没有一个当前宗教的宗教改革中心");
	registerNoArgTrigger("has_game_started","游戏已经开始","游戏尚未开始");
	registerNoArgTrigger("has_had_golden_age","曾经有过黄金时代","未曾有过黄金时代");
	registerNoArgTrigger("has_hostile_reformation_center","有一个敌对的宗教改革中心","没有一个敌对的宗教改革中心");
	registerNoArgTrigger("has_influencing_fort","拥有已激活的要塞","没有拥有已激活的要塞");
	registerNoArgTrigger("has_missionary","有正在进行的传教","没有");
	registerNoArgTrigger("has_new_dynasty","有新王朝","没有新王朝");
	registerNoArgTrigger("has_or_building_flagship","拥有或正在建造旗舰","尚未拥有且没有建造旗舰");
	registerNoArgTrigger("has_owner_accepted_culture","省份文化是拥有者的相容文化","省份文化不是拥有者的相容文化");
	registerNoArgTrigger("has_owner_culture","省份文化是拥有者的主流文化","省份文化不是拥有者的主流文化");
	registerNoArgTrigger("has_owner_religion","省份宗教是拥有者的宗教","省份宗教不是拥有者的宗教");
	registerNoArgTrigger("has_owner_secondary_religion","省份宗教是拥有者的相容宗教","省份宗教不是拥有者的相容宗教");
	registerNoArgTrigger("has_parliament","有议会","没有议会");
	registerNoArgTrigger("has_pasha","有一个帕夏","没有一个帕夏");
	registerNoArgTrigger("has_port","拥有港口","没有港口");
	registerNoArgTrigger("has_privateers","在任意贸易节点拥有私掠者","在所有贸易节点都没有私掠者");
	registerNoArgTrigger("has_regency","有摄政议会","没有摄政议会");
	registerNoArgTrigger("has_removed_fow","战争迷雾已经消除","战争迷雾尚未消除");
	registerNoArgTrigger("has_revolution_in_province","革命已经传播至该省份","革命尚未传播至该省份");
	registerNoArgTrigger("has_scutage","已经实行免服兵役税","尚未实行免服兵役税");
	registerNoArgTrigger("has_seat_in_parliament","在议会中拥有席位","在议会中没有席位");
	registerNoArgTrigger("has_secondary_religion","拥有相容宗教","没有相容宗教");
	registerNoArgTrigger("has_send_officers","已经实行派遣军官","尚未实行派遣军官");
	registerNoArgTrigger("has_siege","有正在进行中的围城","没有正在进行中的围城");
	registerNoArgTrigger("has_state_patriach","已经开创当地教派","尚未开创当地教派");
	registerNoArgTrigger("has_subsidize_armies","已经实行资助军队","尚未实行资助军队");
	registerNoArgTrigger("has_support_loyalists","已经实行支持效忠派","尚未实行支持效忠派");
	registerNoArgTrigger("has_switched_tag","改变过游玩国家","从未改变过游玩国家");
	registerNoArgTrigger("has_truce","拥有停战协议","没有停战协议");
	registerNoArgTrigger("has_wartaxes","有战争税","没有战争税");
	registerNoArgTrigger("heir_has_consort_dynasty","继承人与配偶相同王朝","继承人与配偶不同王朝");
	registerNoArgTrigger("has_unified_culture_group","已统一文化组","未统一文化组");
	registerNoArgTrigger("heir_has_ruler_dynasty","继承人与统治者相同王朝","继承人与统治者不同王朝");
	registerNoArgTrigger("highest_value_trade_node","是世界上价值最高的贸易节点","不是世界上价值最高的贸易节点");
	registerNoArgTrigger("hre_leagues_enabled","宗教同盟已经启用","宗教同盟尚未启用");
	registerNoArgTrigger("hre_religion_locked","安本纳尔帝国有不可改变的官方信仰","安本纳尔帝国没有不可改变的官方信仰");
	registerNoArgTrigger("hre_religion_treaty","埃斯玛里雅和约已经签署","埃斯玛里雅和约没有签署");
	registerNoArgTrigger("in_golden_age","当前正处于黄金时代","当前不处于黄金时代");
	registerNoArgTrigger("ironman","是铁人模式","不是铁人模式");
	registerNoArgTrigger("is_all_concessions_in_council_taken","揭秘教辩论会已经结束","揭秘教辩论会尚未结束");
	registerNoArgTrigger("is_at_war","处于战争状态","处于和平状态");
	registerNoArgTrigger("is_backing_current_issue","正反对当前议会议程","正支持当前议会议程");
	registerNoArgTrigger("is_bankrupt","已经破产","没有破产");
	registerNoArgTrigger("is_blockaded","省份被封锁","省份未被封锁");
	registerNoArgTrigger("is_capital","是首都","不是首都");
	registerNoArgTrigger("is_city","是城市","不是城市");
	registerNoArgTrigger("is_client_nation","是仆从国","不是仆从国");
	registerNoArgTrigger("is_colonial_nation","是殖民领","不是殖民领");
	registerNoArgTrigger("is_colony","是殖民地","不是殖民地");
	registerNoArgTrigger("is_council_enabled","揭秘教辩论会已经开始","揭秘教辩论会尚未开始");
	registerNoArgTrigger("is_crusade_target","是十字军目标","不是十字军目标");
	registerNoArgTrigger("is_defender_of_faith","是信仰守护者","不是信仰守护者");
	registerNoArgTrigger("is_dynamic_tag","是动态标签","不是动态标签");
	registerNoArgTrigger("is_elector","是选帝侯","不是选帝侯");
	registerNoArgTrigger("is_emperor","是安本纳尔帝国皇帝","不是安本纳尔帝国皇帝");
	registerNoArgTrigger("is_emperor_of_china","是哀伤河管家","不是哀伤河管家");
	registerNoArgTrigger("is_empty","省份可以被殖民","省份无法被殖民");
	registerSimpleTrigger("is_enemy","是%s的敌人","不是%s的敌人",ParadoxType::SCOPE);
	registerNoArgTrigger("is_excommunicated","已被绝罚","未被绝罚");
	registerNoArgTrigger("is_federation_leader","是联盟领袖","不是联盟领袖");
	registerNoArgTrigger("is_federation_nation","是联盟成员","不是联盟成员");
	registerNoArgTrigger("is_female","统治者是女性","统治者不是女性");
	registerNoArgTrigger("is_force_converted","已被强制转换宗教","未被强制转换宗教");
	registerNoArgTrigger("is_former_colonial_nation","是前殖民领国家","不是前殖民领国家");
	registerNoArgTrigger("is_foreign_claim","是其他国家的宣称省份","不是其他国家的宣称省份");
	registerNoArgTrigger("is_great_power","是列强","不是列强");
	registerNoArgTrigger("is_heir_leader","继承人是陆军将领","继承人不是陆军将领");
	registerNoArgTrigger("is_hegemon","是霸权","不是霸权");
	registerNoArgTrigger("is_imperial_ban_allowed","帝国禁令宣战理由已被启用","帝国禁令宣战理由未被启用");
	registerNoArgTrigger("is_in_capital_area","与首都陆路相连","未与首都陆路相连");
	registerNoArgTrigger("is_in_coalition","在包围网中","不在包围网中");
	registerNoArgTrigger("is_in_coalition_war","在一场包围网战争中","不在一场包围网战争中");
	registerNoArgTrigger("is_in_deficit","处于赤字状态","未处于赤字状态");
	registerNoArgTrigger("is_in_extended_regency","在延长摄政中","未在延长摄政中");
	registerNoArgTrigger("is_in_league_war","在宗教联盟战争中","不在宗教联盟战争中");
	registerNoArgTrigger("is_in_trade_league","是贸易联盟的一员","不是贸易联盟的一员");
	registerNoArgTrigger("is_island","是岛屿","不是岛屿");
	registerNoArgTrigger("is_lacking_institutions","缺乏思潮","没有缺乏思潮");
	registerNoArgTrigger("is_league_leader","是宗教联盟领袖","不是宗教联盟领袖");
	registerNoArgTrigger("is_lesser_in_union","是被联统国","不是被联统国");
	registerNoArgTrigger("is_looted","省份已被劫掠","省份未被劫掠");
	registerNoArgTrigger("is_monarch_leader","统治者是陆军将领","统治者不是陆军将领");
	registerNoArgTrigger("is_march","是卫戍国","不是卫戍国");
	registerNoArgTrigger("is_node_in_trade_company_region","是贸易公司区域内的贸易节点","不是贸易公司区域内的贸易节点");
	registerNoArgTrigger("is_nomad","是游牧国家","不是游牧国家");
	registerNoArgTrigger("is_orangists_in_power","奥兰治派正掌权","奥兰治派未掌权");
	registerNoArgTrigger("is_overseas","是海外省份","不是海外省份");
	registerNoArgTrigger("is_overseas_subject","是海外属国","不是海外属国");
	registerNoArgTrigger("is_owned_by_trade_company","省份在贸易公司中","省份不在贸易公司中");
	registerNoArgTrigger("is_papal_controller","是教廷监护","不是教廷监护");
	registerNoArgTrigger("is_part_of_hre","是安本纳尔帝国的一部分","不是安本纳尔帝国的一部分");
	registerNoArgTrigger("is_playing_custom_nation","正在游玩自定义国家","不在游玩自定义国家");
	registerNoArgTrigger("is_previous_papal_controller","之前是教廷监护","之前不是教廷监护");
	registerNoArgTrigger("is_prosperous","省份处于繁荣中","省份不处于繁荣中");
	registerNoArgTrigger("is_protectorate","是受保护国","不是受保护国");
	registerNoArgTrigger("is_random_new_world","使用了随机新世界","没有使用随机新世界");
	registerNoArgTrigger("is_reformation_center","是一个宗教改革中心","不是一个宗教改革中心");
	registerNoArgTrigger("is_religion_reformed","已经改革宗教","尚未改革宗教");
	registerNoArgTrigger("is_revolution_target","是革命目标","不是革命目标");
	registerNoArgTrigger("is_revolutionary","是革命国家","不是革命国家");
	registerNoArgTrigger("is_ruler_commanding_unit","统治者正在指挥单位","统治者没有指挥单位");
	registerNoArgTrigger("is_sea","省份是海洋","省份不是海洋");
	registerNoArgTrigger("is_state","在直属州内","不在直属州内");
	registerNoArgTrigger("is_statists_in_power","议会派正掌权","议会派没有掌权");
	registerNoArgTrigger("is_subject","是属国","不是属国");
	registerNoArgTrigger("is_territory","是自治领","不是自治领");
	registerNoArgTrigger("is_trade_league_leader","是贸易联盟领袖","不是贸易联盟领袖");
	registerNoArgTrigger("is_tribal","是原住民","不是原住民");
	registerNoArgTrigger("is_vassal","是该国的附庸国","不是该国的附庸国");
	registerNoArgTrigger("is_wasteland","是荒凉之地","不是荒凉之地");
	registerNoArgTrigger("island","是岛屿","不是岛屿");
	registerNoArgTrigger("luck","是幸运国家","不是幸运国家");
	registerNoArgTrigger("normal_or_historical_nations","使用了普通或史实国家设置","没有使用普通或史实国家设置");
	registerNoArgTrigger("normal_province_values","使用正常省份价值","没有使用正常省份价值");
	registerNoArgTrigger("papacy_active","已启用教廷","未启用教廷");
	registerNoArgTrigger("primitives","是原始国家","不是原始国家");
	registerNoArgTrigger("province_is_on_an_island","省份在岛屿上","省份不在岛屿上");
	registerNoArgTrigger("province_getting_expelled_minority","省份正在驱逐少数族群","省份没有驱逐少数族群");
	registerNoArgTrigger("revolution_target_exists","革命目标存在","革命目标不存在");
	registerNoArgTrigger("ruler_is_foreigner","统治者是外国人","统治者不是外国人");
	registerNoArgTrigger("unit_in_battle","有单位在战斗中","没有单位在战斗中");
	registerNoArgTrigger("unit_in_siege","有正在进行的围城","没有正在进行的围城");
	registerNoArgTrigger("uses_authority","使用权威机制","没有使用权威机制");
	registerNoArgTrigger("uses_church_aspects","使用教会信条机制","没有使用教会信条机制");
	registerNoArgTrigger("uses_blessings","使用牧首神赐机制","没有使用牧首神赐机制");
	registerNoArgTrigger("uses_cults","使用崇拜物机制","没有使用崇拜物机制");
	registerNoArgTrigger("uses_devotion","使用奉献度机制","没有使用奉献度机制");
	registerNoArgTrigger("uses_doom","使用末日值机制","没有使用末日值机制");
	registerNoArgTrigger("uses_fervor","使用宗教热情机制","没有使用宗教热情机制");
	registerNoArgTrigger("uses_isolationism","使用孤立主义机制","没有使用孤立主义机制");
	registerNoArgTrigger("uses_karma","使用科琳典范值机制","没有使用科琳典范值机制");
	registerNoArgTrigger("uses_papacy","使用教廷机制","使用教廷机制");
	registerNoArgTrigger("uses_patriarch_authority","使用恶魔力量机制","没有使用恶魔力量机制");
	registerNoArgTrigger("uses_personal_deities","使用个人神祇机制","没有使用个人神祇机制");
	registerNoArgTrigger("uses_piety","使用虔诚机制","没有使用虔诚机制");
	registerNoArgTrigger("uses_religious_icons","使用圣像机制","没有使用圣像机制");
	registerNoArgTrigger("uses_syncretic_faiths","使用相融信仰机制","没有使用相融信仰机制");
	registerNoArgTrigger("was_player","曾经是人类玩家","过去不是人类玩家");
	registerNoArgTrigger("will_back_next_reform","将反对下一项帝国改革","将同意下一项帝国改革");
	registerNoArgTrigger("is_incident_active","有任意事变处于活跃状态","没有任何事变处于活跃状态");
	registerBooleanTrigger("empire_of_china_has_active_decree","有生效中的圣旨","没有生效中的圣旨");
	registerSingleArgTrigger("empire_of_china_has_active_decree","圣旨%s处于生效状态","圣旨%s尚未处于生效状态",ParadoxType::STRING);
	registerSimpleTrigger("tag","是%s","不是%s",ParadoxType::SCOPE);
	registerSimpleTrigger("absolutism","专制度至少为%d","专制度少于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("accepted_culture","已经接纳%s的文化","尚未接纳%s的文化",ParadoxType::SCOPE);
	registerSingleArgTrigger("accepted_culture","已经接纳%s文化","尚未接纳%s文化",ParadoxType::STRING);
	registerSingleArgTrigger("adm","统治者的行政能力至少与%s相同","统治者的行政能力低于%s",ParadoxType::SCOPE);
	registerSingleArgTrigger("adm","统治者的行政能力至少为%d","统治者的行政能力低于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("adm_power","有至少与%s相同的行政点数","拥有有少于%s的行政点数",ParadoxType::SCOPE);
	registerSingleArgTrigger("adm_power","行政点数至少为%d","行政点数少于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("adm_tech"," 行政科技至少为%d","行政科技低于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("advisor","已经雇佣了%s","尚未雇佣%s",ParadoxType::STRING);
	registerSimpleTrigger("advisor_exists","id为%d的顾问存在","id为%d的顾问存在",ParadoxType::INTEGER);
	registerSimpleClauseTrigger("ai_attitude",new TriggerItem(registerShortString("ai_attitude"),
		{"%s对该国的态度为%s","%s为该国的态度不为%s"},
		{"who","attitude"},
		{ParadoxType::SCOPE,ParadoxType::STRING},
		{0,1}
	));
	registerSimpleTrigger("army_professionalism","陆军职业度%p%%","陆军职业度少于%p%%",ParadoxType::INTEGER);
	registerSingleArgTrigger("army_size","军队规模至少为%dK","军队规模小于%dK",ParadoxType::INTEGER);
	registerSingleArgTrigger("army_size","拥有至少和%s规模相同的军队","军队规模小于%s",ParadoxType::SCOPE);
	registerSimpleTrigger("army_size_percentage","军队规模至少为上限的%p%%","军队规模小于上限的%p%%",ParadoxType::INTEGER);
	registerSimpleClauseTrigger("army_strength",new TriggerItem(registerShortString("army_strength"),
		{"陆军实力至少为%s的%p%%","陆军实力少于%s的%p%%"},
		{"who","value"},
		{ParadoxType::SCOPE,ParadoxType::INTEGER},
		{0,1}
	));
	registerSingleArgTrigger("army_tradition","陆军传统至少为%d","陆军传统少于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("army_tradition","陆军传统不低于%s","陆军传统低于%s",ParadoxType::SCOPE);
	registerSimpleTrigger("artillery_fraction","炮兵比例至少为%p%%","炮兵比例小于%p%%",ParadoxType::INTEGER);
	registerSingleArgTrigger("artillery_in_province","有至少%d队炮兵", "炮兵的数量小于%d队",ParadoxType::INTEGER);
	registerSingleArgTrigger("artillery_in_province","有来自%s的炮兵", "没有来自%s的炮兵",ParadoxType::SCOPE);
	registerSimpleTrigger("authority","权威值至少为%d", "权威值小于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("authority","拥有至少与%s相同的权威值", "权威值小于%s",ParadoxType::SCOPE);
	registerSimpleTrigger("average_autonomy","平均自治度至少为%p%%","平均自治度低于%p%%",ParadoxType::INTEGER);
	registerSimpleTrigger("average_home_autonomy","直属州核心省份的平均自治度至少于%p%%","直属州核心省份的平均自治度低于%p%%",ParadoxType::INTEGER);
	registerSimpleTrigger("average_autonomy_above_min","最低限度以上的平均自治度至少为%p%%","最低限度以上的平均自治度低于%p%%",ParadoxType::INTEGER);
	registerSingleArgTrigger("base_production","基础生产至少为%d","基础生产少于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("base_production","基础生产至少为variable:%s","基础生产少于variable:%s",ParadoxType::STRING);
	registerSingleArgTrigger("base_manpower","基础人力至少为%d","基础人力少于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("base_manpower","基础人力至少为variable:%s","基础人力少于variable:%s",ParadoxType::STRING);

	registerSingleArgTrigger("base_tax","基础税收至少为%d","基础税收少于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("base_tax","基础税收至少为variable:%s","基础税收少于variable:%s",ParadoxType::STRING);
	registerSimpleTrigger("blockade","被封锁的港口至少为%p%%","被封锁的港口的少于%p%%",ParadoxType::INTEGER);
	
	registerSimpleClauseTrigger("border_distance",new TriggerItem(registerShortString("border_distance"),
		{"与%s的边境距离至少为%d","与%s的边境距离少于%d"},
		{"who","distance"},
		{ParadoxType::SCOPE,ParadoxType::INTEGER},
		{0,1}
	));
	registerNumberRequiredTrigger("calc_true_if","amount","至少%d个","少于%d个");
	registerSimpleTrigger("can_be_overlord","可以作为%s的宗主国","无法作为%s的宗主国",ParadoxType::STRING);
	registerSimpleTrigger("can_build","可以修建%s","不能修建%s",ParadoxType::STRING);
	registerNoArgTrigger("can_create_vassals","可以创建附庸","不能创建附庸");
	registerSimpleTrigger("can_justify_trade_conflict","可以正当化与%s的贸易争端","无法正当化与%s的贸易争端",ParadoxType::SCOPE);
	registerSimpleTrigger("can_spawn_rebel","当地有效的叛军类型为%s","当地有效的叛军类型不是%s",ParadoxType::STRING);
	
	registerSimpleClauseTrigger("can_use_peace_treaty",new TriggerItem(registerShortString("can_use_peace_treaty"),
		{"%s可以使用%s条款","%s不可以使用%s条款"},
		{"who","treaty"},
		{ParadoxType::SCOPE,ParadoxType::STRING},
		{0,1}
	));
	registerSimpleTrigger("capital","首都位于%s","首都不位于%s",ParadoxType::SCOPE);
	registerSimpleClauseTrigger("capital_distance",new TriggerItem(registerShortString("capital_distance"),
		{"与%s首都之间的距离至少为%d","与%s首都之间的距离小于%d"},
		{"who","distance"},
		{ParadoxType::SCOPE,ParadoxType::INTEGER},
		{0,1}
	));
	registerSimpleTrigger("cavalry_fraction","骑兵占军队比例至少为%p%%","骑兵占军队比例少于%p%%",ParadoxType::STRING);
	registerSingleArgTrigger("cavalry_in_province","有至少%d队骑兵", "骑兵的数量小于%d队",ParadoxType::INTEGER);
	registerSingleArgTrigger("cavalry_in_province","有来自%s的骑兵", "没有来自%s的骑兵",ParadoxType::SCOPE);
	registerSimpleTrigger("province_has_center_of_trade_of_level","省份至少有%d级的贸易中心","省份没有至少%d级的贸易中心",ParadoxType::INTEGER);
	registerClausedTrigger("check_variable", new TriggerItem(registerShortString("check_variable"),
		{"变量%s的值至少为%s","变量%s的值小于%s"},
		{"src","tar"},
		{ParadoxType::STRING,ParadoxType::STRING},
		{0,1}
	),
	[](std::map<std::string,ParadoxBase*>& map)-> bool {
		std::string src_string = "";
		std::string tar_string = "";
		if(map.size() != 1){
			src_string = map["which"]->getAsString()->getStringContent();
			if(map.find("value") != map.end()){
				tar_string = std::to_string(map["value"]->getAsInteger()->getIntegerContent() / 1000);
			}
			else if(map.find("which@2") != map.end()){
				if(isCastable(map["which@2"],ParadoxType::SCOPE)){
					std::string tar1 = "";
					if(map["which@2"]->getType() == ParadoxType::INTEGER) {
						tar1 = std::to_string(map["which@2"]->getAsInteger()->getIntegerContent() / 1000);
					}
					else tar1 = map["which@2"]->getAsString()->getStringContent();
					Scope* scope = createScopeFromString(tar1);
					if(scope == nullptr) return false;
					else tar_string = scope->toString();
				}
				else tar_string = map["which@2"]->getAsString()->getStringContent();
			}
			else return false;
			map.clear();
			map["src"] = createString(src_string);
			map["tar"] = createString(tar_string);
		}
		else {
			auto [k,v] = *map.begin();
			
			map.clear();
			map["src"] = createString(k);
			ParadoxString* str = v->getAsString();
			if(str != nullptr){
				if(Scope *scope = createScopeFromString(str->getStringContent());scope != nullptr){
					map["tar"] = createString(scope->toString());
				}
				else map["tar"] = str;
			}
			else if(ParadoxInteger* pi = v->getAsInteger();pi != nullptr){
				map["tar"] = createString(std::to_string(pi->getIntegerContent() / 1000));
			}
			else return false;
		}

		return true;
	}
	);
	registerSingleArgTrigger("church_power","教会力量至少为%d","教会力量小于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("church_power","教会力量不低于%s","教会力量少于%s",ParadoxType::SCOPE);
	registerSimpleTrigger("coalition_target","%s是包围网的目标","%s不是包围网的目标",ParadoxType::SCOPE);
	registerSimpleTrigger("colonial_region","省份位于%s殖民地区","省份不位于%s殖民地区",ParadoxType::STRING);
	registerSimpleTrigger("colony","拥有至少%d个殖民领","拥有少于%d个殖民领",ParadoxType::INTEGER);
	registerSimpleTrigger("colony_claim","%s拥有殖民领宣称","%s没有殖民地宣称",ParadoxType::SCOPE);
	registerSimpleTrigger("colonysize","殖民地规模至少为%d","殖民地规模小于%d",ParadoxType::SCOPE);
	registerSimpleTrigger("consort_adm","有一个行政能力至少为%d的配偶","没有一个行政能力至少为%d的配偶",ParadoxType::INTEGER);
	registerSimpleTrigger("consort_age","有一个至少%d岁的的配偶","没有一个至少%d岁的的配偶",ParadoxType::INTEGER);
	registerSimpleTrigger("consort_dip","有一个外交能力至少为%d的配偶","没有一个外交能力至少为%d的配偶",ParadoxType::INTEGER);
	registerSimpleTrigger("consort_mil","有一个军事能力至少为%d的配偶","没有一个军事能力至少为%d的配偶",ParadoxType::INTEGER);
	registerSimpleTrigger("consort_culture","有一个文化为%s的配偶","没有一个文化为%s的配偶",ParadoxType::STRING);
	registerSimpleTrigger("consort_has_personality","配偶拥有%s特质","配偶没有%s特质",ParadoxType::STRING);
	
	registerSingleArgTrigger("consort_religion","有一个信仰%s的配偶","没有一个信仰%s的配偶",ParadoxType::STRING);
	registerSingleArgTrigger("consort_religion", "有一个信仰%s正信的配偶","没有一个信仰%s正信的配偶",ParadoxType::SCOPE);

	registerSimpleTrigger("construction_progress","修建进度至少为%p%%","修建进度少于%p%%",ParadoxType::INTEGER);
	
	registerSingleArgTrigger("continent", "省份位于%s大陆","省份不位于%s大陆",ParadoxType::STRING);
	registerSingleArgTrigger("continent", "省份位于%s所在大陆","省份不位于%s所在大陆",ParadoxType::SCOPE);

	registerSimpleTrigger("controlled_by", "省份被%s所控制","省份未被%s所控制",ParadoxType::SCOPE);
	registerSimpleTrigger("controls","控制省份%s","没有控制省份%s",ParadoxType::SCOPE);
	
	registerSimpleTrigger("claim","拥有对%s的宣称","没有对%s的宣称",ParadoxType::SCOPE);
	registerSimpleTrigger("core_claim","拥有对%s的核心宣称","没有对%s的核心宣称",ParadoxType::SCOPE);

	registerSimpleTrigger("core_percentage","核心省份比例至少为%p%%","核心省份比例小于%p%%",ParadoxType::SCOPE);

	registerSimpleTrigger("corruption","腐败度至少为%d","腐败度小于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("council_position","在揭秘教辩论会中持有%s立场","没有在揭秘教辩论会中持有%s立场",ParadoxType::STRING);
	registerSimpleTrigger("country_or_non_sovereign_subject_holds","被%s或其非朝贡属国持有","没有被%s或其非朝贡属国持有",ParadoxType::SCOPE);
	registerSimpleTrigger("country_or_subject_holds","被%s或其属国持有","没有被%s或其属国持有",ParadoxType::SCOPE);

	registerSingleArgTrigger("crown_land_share","王室领地比例至少为%p%%","王室领地比例少于%p%%",ParadoxType::INTEGER);
	registerSingleArgTrigger("crown_land_share","王室领地比例大于%s阶级持有比例","王室领地比例大于%s阶级持有比例",ParadoxType::STRING);
	registerSingleArgTrigger("culture","省份文化为%s","省份文化不为%s",ParadoxType::STRING);
	registerSingleArgTrigger("culture","省份文化是%s的主流文化","省份文化不是%s的主流文化",ParadoxType::SCOPE);
	registerSimpleTrigger("culture_group","属于%s文化组","不属于%s文化组",ParadoxType::SCOPE);
	registerSimpleTrigger("culture_group_claim","%s拥有与我国主流文化相同文化组的省份","%s没有与我国主流文化相同文化组的省份",ParadoxType::SCOPE);
	
	registerSimpleTrigger("current_age","当前时代为%s","当前时代不是%s",ParadoxType::STRING);
	registerSimpleTrigger("current_bribe","该省份的议会席位想要%s类型的贿赂","该省份的议会席位不想要%s类型的贿赂",ParadoxType::STRING);
	registerSimpleTrigger("current_debate","当前议会正在辩论%s","当前议会没有辩论%s",ParadoxType::STRING);
	registerSimpleTrigger("current_icon","当前已激活%s","当前未激活%s",ParadoxType::STRING);
	registerSimpleTrigger("current_income_balance","上个月的净收入至少为%d","上个月的净收入少于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("current_institution","当地最早尚未接纳的思潮支持度达到%d%%","当地最早尚未接纳的思潮支持度不足%d%%",ParadoxType::INTEGER);
	registerSimpleTrigger("current_institution_growth","当地最早尚未接纳的思潮增长达到%d%%","当地最早尚未接纳的思潮增长不足%d%%",ParadoxType::INTEGER);
	registerSimpleTrigger("current_size_of_parliament","当前议会至少拥有%d个席位","当前议会不足%d个席位",ParadoxType::INTEGER);
	registerSimpleTrigger("defensive_war_with","当前正在防御战争中对抗%s","当前没有在防御战争中对抗%s",ParadoxType::SCOPE);
	registerSimpleTrigger("devastation","荒废度低于%d","荒废度达到%d",ParadoxType::INTEGER);

	registerSimpleTrigger("innovativeness","创新度至少为%d","创新度小于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("treasury","拥有至少%d克朗","拥有少于%d克朗",ParadoxType::INTEGER);
	registerNumberRequiredTrigger("num_of_owned_provinces_with","value","至少%d个拥有的省份满足下列条件:","少于%d个拥有的省份满足下列条件:");
	registerSimpleTrigger("has_country_flag","国家标签'%s'已被设置","国家标签'%s'未被设置",ParadoxType::STRING);
	registerSimpleTrigger("monthly_dip","每月外交点数至少为%d","每月外交点数少于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("monthly_adm","每月行政点数至少为%d","每月行政点数少于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("monthly_mil","每月军事点数至少为%d","每月军事点数少于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("was_tag","曾经是%s","以前不是%s",ParadoxType::SCOPE);
	
}


std::string TriggerItem::toString(std::vector<ParadoxBase*> base,bool reversed){
	
	std::string usePattern = reversed ? this->reversePattern : this->pattern;
	if(this->usedParameter.size() == 0) return usePattern;
	Pattern p(usePattern);

	for(int i = 0;i < usedParameter.size();i++){
		int index = usedParameter[i];
		if(index == -1){
			return "<ERROR>";
		}
		else{
			ParadoxBase* base1 = base[index];
			if(isCastable(base1,parameterType[index])){
				ParadoxBase* base2 = castTo(base1,parameterType[index]);
				bool success = false;

				if(parameterType[index] == ParadoxType::INTEGER){
					success = p.setNextInteger(base2->getAsInteger()->getIntegerContent());
				}
				else if(parameterType[index] == ParadoxType::STRING){
					success = p.setNextString(base2->getAsString()->getStringContent());
				}
				else if(parameterType[index] == ParadoxType::DATE){
					success = p.setNextString(base2->getAsDate()->getDateContent().toString());			
				}
				else if(parameterType[index] == ParadoxType::SCOPE){
					Scope* scope = base2->getAsScope()->getValue();
					if(scope == nullptr) return "<ERROR>";
					success = p.setNextString(scope->toString());
						
				}
				if(!success) return "<ERROR>";
			} 
			else return "<ERROR>";
		}
	}
	return p.getOutput();
}

std::string TriggerItem::toHtml(std::vector<ParadoxBase*> base,bool reversed){
	
	std::string usePattern = reversed ? this->reversePattern : this->pattern;
	if(this->usedParameter.size() == 0) return usePattern;
	Pattern p(usePattern);
	for(int i = 0;i < usedParameter.size();i++){
		int index = usedParameter[i];
		if(index == -1){
			return "<ERROR>";
		}
		else{
			ParadoxBase* base1 = base[index];
			if(isCastable(base1,parameterType[index])){
				ParadoxBase* base2 = castTo(base1,parameterType[index]);
				bool success = false;
				if(parameterType[index] == ParadoxType::INTEGER){
					success = p.setNextInteger(base2->getAsInteger()->getIntegerContent());
				}
				else if(parameterType[index] == ParadoxType::STRING){
					success = p.setNextString(base2->getAsString()->getStringContent());
				}
				else if(parameterType[index] == ParadoxType::DATE){
					success = p.setNextString(base2->getAsDate()->getDateContent().toString());			
				}
				else if(parameterType[index] == ParadoxType::SCOPE){
					Scope* scope = base2->getAsScope()->getValue();
					if(scope == nullptr) return "<ERROR>";
					success = p.setNextString(scope->toHtml());
						
				}
				if(!success) return "<ERROR>";
			} 
			else return "<ERROR>";
		}
	}
	return p.getOutput();
}


TriggerItem::TriggerItem(const std::string& _name,std::pair<std::string,std::string>&& patterns,std::vector<std::string>&& parameterName,std::vector<ParadoxType>&& parameterType,std::vector<int>&& usedParameter,ScopeType scope_type): name(_name){
	this->pattern = patterns.first;
	this->reversePattern = patterns.second;
	this->parameterType = parameterType;
	this->usedParameter = usedParameter;
	this->usable_scope = usable_scope;
	for(int i = 0;i < parameterName.size();i++){
		this->parameterName[parameterName[i]] = i;
	}
}

void preInit(const int depth,std::string& str){
	for(int i = 0;i < depth;i++){
		str.append("*");
	}	
}
ComplexTrigger* Trigger::getAsComplexTrigger(){
	if(this->getType() == TriggerType::COMMON) return nullptr;
	return static_cast<ComplexTrigger*>(this);
}

LogicTrigger* Trigger::getAsLogicTrigger(){
	if(this->getType() != TriggerType::LOGIC) return nullptr;
	return static_cast<LogicTrigger*>(this);
}
CommonTrigger* Trigger::getAsCommonTrigger(){
	if(this->getType() != TriggerType::COMMON) return nullptr;
	return static_cast<CommonTrigger*>(this);
}

void ignoreCurrentDepth(ComplexTrigger* trigger){
	for(int i = 0;i < trigger->subTriggers.size();i++){
		trigger->subTriggers[i]->depth--;
		ComplexTrigger* trigger2 = trigger->subTriggers[i]->getAsComplexTrigger();
		if(trigger2 != nullptr) ignoreCurrentDepth(trigger2);
	}
}

void ComplexTrigger::putTrigger(Trigger* trigger){
	trigger->depth = this->depth + 1;
	this->subTriggers.push_back(trigger);
}
void ComplexTrigger::takeOverLifeCycle(){
	if(this->copied) return;
	this->copied = true;
	for(Trigger* trigger : this->subTriggers){
		trigger->takeOverLifeCycle();
	}
}
bool ComplexTrigger::hasAnyTrigger(bool(*predicate)(Trigger*)){
	if(predicate(this)) return true;
	for(Trigger* trigger : this->subTriggers){
		if(predicate(trigger)) return true;
	}
	return false;
}

bool ComplexTrigger::foreach(std::function<bool(Trigger*)> action){
	if(!action(this)) return false;
	for(Trigger* trigger : this->subTriggers){
		if(!trigger->foreach(action)) return false;
	}
	return true;
}
ChangeScopeTrigger::ChangeScopeTrigger(Scope* scope){
	this->changedScope = scope;
	this->depth = 0;
	this->use_type = false;
}
CommonTrigger::CommonTrigger(TriggerItem* item){
	this->item = item;
	this->reversed = false;
	this->depth = 0;
}
void CommonTrigger::pushObject(ParadoxBase* obj){
	this->base.push_back(obj);
}


LogicTrigger::LogicTrigger(LogicType logic){
	this->type = logic;
	this->depth = 0;
	this->ignored = false;
	this->omitted = false;
}

std::string CommonTrigger::toHtml(bool reversed,int depth){
	std::string str("");
	preInit(depth,str);
	str.append(this->item->toString(this->base,Xor(reversed,this->reversed)));
	this->depth = 0;
	return str;
}
std::string CommonTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	preInit(depth,str);
	str.append(this->item->toString(this->base,Xor(reversed,this->reversed)));
	return str;
}
void CommonTrigger::takeOverLifeCycle(){
	if(this->copied) return;
	this->copied = true;
	for(int i = 0;i < this->base.size();i++){
		this->base[i] = deep_copy(this->base[i]);
	}
}
bool CommonTrigger::hasAnyTrigger(bool(*predicate)(Trigger*)){
	return predicate(this);
}
bool CommonTrigger::foreach(std::function<bool(Trigger*)> action){
	return action(this);
}
std::string ChangeScopeTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	if(this->subTriggers.empty()) return str;
	int cDepth = depth;
	if(this->changedScope != nullptr){
		preInit(depth,str);
		bool should_add_bracket = this->changedScope->getType() != ScopeType::ANY;
		if(should_add_bracket) str.append("(");
		if(use_type){
			str.append(trigger_type ? "所有" : "任意");
		}
		str.append(this->changedScope->toString());
		if(should_add_bracket) str.append(")");
		str.append(":\n");
		cDepth++;
	}

	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed,cDepth));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	return str;
}

std::string LogicTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	int size = this->subTriggers.size();
	if(size == 0) return str;
	if(size != 1) preInit(depth,str);
	LogicType actual_type = this->type;
	if(reversed){
		if(actual_type == LogicType::AND) actual_type = LogicType::OR;
		else if(actual_type == LogicType::OR) actual_type = LogicType::AND;
	}
	switch(actual_type){
		case LogicType::AND:
			if(size == 1){
				return this->subTriggers[0]->toString(reversed,depth);
			}
			else{
				if(!this->omitted) str.append("下列条件需全部满足:\n");
				for(int i = 0;i < size;i++){
					LogicTrigger* subtrigger = this->subTriggers[i]->getAsLogicTrigger();
					bool shouldOmit = subtrigger != nullptr && subtrigger->type != LogicType::OR;
					int cDepth = depth + 1;
					if(shouldOmit){
						subtrigger->omitted = true;
						//subtrigger->ignored = true; 
						//ignoreCurrentDepth(subtrigger);
						cDepth--;
					} 
					str.append(this->subTriggers[i]->toString(reversed,cDepth));
					if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
				}
			}
			break;
		case LogicType::OR:
			if(size == 1){
				return this->subTriggers[0]->toString(reversed,depth);
			}
			else{
				if(!this->omitted) str.append("下列条件至少满足一个:\n");
				for(int i = 0;i < size;i++){
					LogicTrigger* subtrigger = this->subTriggers[i]->getAsLogicTrigger();
					bool shouldOmit = subtrigger != nullptr && subtrigger->type == LogicType::OR;
					int cDepth = depth + 1;
					if(shouldOmit){
						subtrigger->omitted = true;
						cDepth--;
					} 
					str.append(this->subTriggers[i]->toString(reversed,cDepth));
					if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
				}
			}
			break;
		case LogicType::NOT:
			if(size == 1){
				return this->subTriggers[0]->toString(!reversed,depth).append("\n");
			}
			else {
				if(!this->omitted) str.append("下列条件需全部满足:\n");
				for(int i = 0;i < size;i++){
					LogicTrigger* subtrigger = this->subTriggers[i]->getAsLogicTrigger();
					bool shouldOmit = subtrigger != nullptr && subtrigger->type == LogicType::OR;
					int cDepth = depth + 1;
					if(shouldOmit){
						subtrigger->omitted = true;
						cDepth--;
					} 
					str.append(this->subTriggers[i]->toString(!reversed,cDepth));
					if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
				}
			}
			break;
	}
	return str;
}

std::string NumberRequiredTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	if(this->subTriggers.empty()) return str;
	preInit(depth,str);
	ParadoxInteger* tmp = new ParadoxInteger(this->amount);
	std::vector<ParadoxBase*> base;
	base.push_back(tmp);
	str.append(this->item->toString(base,reversed));
	str.append("\n");
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed,depth + 1));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	delete tmp; 
	return str;
}

std::string ConditionalTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	if(this->subTriggers.empty()) return str;
	if(this->condition->subTriggers.empty()) {
		if(!this->isElseTrigger) return str;
		preInit(depth,str);
		str.append("否则需满足:\n");
		for(int i = 0;i < this->subTriggers.size();i++){
			str.append(this->subTriggers[i]->toString(reversed,depth + 1));
			if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
		}
		return str;
	}
	preInit(depth,str);
	if(this->isElseTrigger) str.append("否则");
	str.append("当以下条件满足时:\n");
	for(int i = 0;i < this->condition->subTriggers.size();i++){
		str.append(this->condition->subTriggers[i]->toString(false,depth + 1));
		if(this->condition->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	preInit(depth,str);
	str.append("需满足下列要求:\n");
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed,depth + 1));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	//str.append("\n");
	
	return str;
}

std::string HiddenTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	if(this->subTriggers.empty()) return str;
	if(this->hidden_current){
		return str;
	}
	preInit(depth,str);
	str.append("(隐藏条件):\n");
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed,depth + 1));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	return str;
}

std::string CustomTooltipTrigger::toString(bool reversed,int depth) const{
	std::string str("");
	if(this->subTriggers.empty()) return str;
	if(!this->show_origin){
		preInit(depth,str);
		str.append(this->tooltip);
		str.append("\n");
	}
	else{
		for(int i = 0;i < this->subTriggers.size();i++){
			str.append(this->subTriggers[i]->toString(reversed,depth));
			if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
		}
	}
	return str;
}

std::string SpecialTrigger::toString(bool reversed,int depth) const{
	if(this->instance == nullptr && this->prototype->isFixed()) {
		std::map<std::string,ParadoxBase*> data;
		if(this->args.size() != 0) data["__REVERSED__"] = nullptr;
		this->instance = this->prototype->createInstance(data);
	}
	std::string ret("");
	preInit(depth,ret);
	std::string loc_pattern = this->prototype->getLocalizationPattern(reversed);
	if(!loc_pattern.empty()){
		NamedPattern np(loc_pattern);
		for(auto[key,value] : this->args){
			ParadoxType type = value->getType();
			if(type == ParadoxType::STRING) np.fillName(*key,value->toString()); 
			else if(type == ParadoxType::INTEGER) np.fillName(*key,value->getAsInteger()->getIntegerContent());
		}
		ret.append(np.getOutput());
		return ret;
	}
	return this->instance->toString(reversed,depth);
}

bool SpecialTrigger::hasAnyTrigger(bool(*predicate)(Trigger*)){
	if(predicate(this)) return true;
	if(predicate(this->instance)) return true;
	return false;
}

bool SpecialTrigger::foreach(std::function<bool(Trigger*)> action){
	if(!action(this)) return false;
	if(!action(this->instance)) return false;
	return true;
}
void parseTrigger(ParadoxTag* tag,ComplexTrigger* trigger){
	for(int i = 0;i < tag->seq.size();i++){
		std::string item = stripTag(tag->seq[i]);
		ParadoxBase* base = tag->get(i);
		ParadoxTag* subTag = base->getAsTag();
		//if it is complicate....
		if(subTag != nullptr){
			//consider if first~
			if(item == "if"){
				ConditionalTrigger* ct = new ConditionalTrigger();
				//if a if-statement without condition,then pass it directly.
				if(subTag->get("limit",1) == nullptr || subTag->get("limit",1)->getAsTag() == nullptr) {
					delete ct;
					continue;
				}
				trigger->putTrigger(ct);
				bool success = parseConditionalTrigger(subTag,ct);
				if(!success){
					delete ct;
					trigger->subTriggers.pop_back();
				}
				continue;
			}
			//just forget else_if and else....
			if(item == "else_if"){
				ConditionalTrigger* ct = new ConditionalTrigger();
				ct->isElseTrigger = true;
				if(subTag->get("limit",1) == nullptr || subTag->get("limit",1)->getAsTag() == nullptr) {
					delete ct;
					continue;
				}
				trigger->putTrigger(ct);
				bool success = parseConditionalTrigger(subTag,ct);
				if(!success){
					delete ct;
					trigger->subTriggers.pop_back();
				}
				continue;
			}
			if(item == "else"){
				ConditionalTrigger* ct = new ConditionalTrigger();
				ct->isElseTrigger = true;
				if(subTag->get("limit",1) != nullptr) {
					delete ct;
					continue;
				}
				trigger->putTrigger(ct);
				parseTrigger(subTag,ct);
				continue;
			}
			//then logic
			if(item == "NOT"){
				LogicTrigger* lt = new LogicTrigger(LogicType::NOT);
				trigger->putTrigger(lt);
				parseTrigger(subTag,lt);
				continue;
			}
			if(item == "AND"){
				LogicTrigger* lt = new LogicTrigger(LogicType::AND);
				trigger->putTrigger(lt);
				parseTrigger(subTag,lt);
				continue;
			}
			if(item == "OR"){
				LogicTrigger* lt = new LogicTrigger(LogicType::OR);
				trigger->putTrigger(lt);
				parseTrigger(subTag,lt);
				continue;			
			}
			//then custom_tt
			if(item == "custom_trigger_tooltip"){
				CustomTooltipTrigger* ctt = new CustomTooltipTrigger();
				ctt->show_origin = false;
				ParadoxString* tt = subTag->get("tooltip",1)->getAsString();
				
				if(tt == nullptr) {
					log_warning(current_location(),"No tooltip provided for a custom_tt,this content will be ignored.");
					continue;
				}
				else{
					ctt->tooltip = tt->getStringContent();
					subTag->remove("tooltip",1);
				} 
				trigger->putTrigger(ctt);
				parseTrigger(subTag,ctt);
				continue;
			}
			//then hidden_trigger
			if(item == "hidden_trigger"){
				HiddenTrigger* ht = new HiddenTrigger();
				ht->hidden_current = false;
				trigger->putTrigger(ht);
				parseTrigger(subTag,ht);
				
				continue;
			}
			//then change_scope
			Scope* scope = createScopeFromString(item);
			if(scope != nullptr){
				ChangeScopeTrigger* cst = new ChangeScopeTrigger(scope);
				trigger->putTrigger(cst);
				parseTrigger(subTag,cst);
				continue;
			} 
			//at last trigger with clause.
			//first is NumberRequiredTrigger
			if(numberRequiredItems.find(item) != numberRequiredItems.end()){
				NumberRequiredTrigger* nrt = new NumberRequiredTrigger();	
				nrt->amount = 1;
				std::string cnt_tag = numberRequiredItems[item];
				TriggerItem* item2 = items[item];
				nrt->item = item2;
				ParadoxBase* base1 = subTag->get(cnt_tag,1);
				if(base1 == nullptr){
					delete nrt;
					continue;
				}
				ParadoxInteger* num = base1->getAsInteger();
				if(num == nullptr) {
					delete nrt;
					continue;
				}
				nrt->amount = num->getIntegerContent();
				subTag->remove(cnt_tag,1);
				trigger->putTrigger(nrt);
				parseTrigger(subTag,nrt);
				continue;
			}
			if(loadedSTs.find(item) != loadedSTs.end()){
				ScriptedTrigger* st = loadedSTs[item];
				if(st->isFixed()) continue;
				Trigger* ti = st->createInstance(subTag->tags);
				if(ti != nullptr){
					SpecialTrigger* spt = new SpecialTrigger(st,ti);
					for(auto[k,v] : subTag->tags){
						spt->args[getLocalizationKeyPtr(k)] = v;
					}
					trigger->putTrigger(spt);
				}
				else {
					log_error(current_location(),"Cannot make instance of scripted_trigger \"",item,"\".");
					for(auto [k,v] : subTag->tags){
						log_error(current_location(),"Parameters:");
						log_error(current_location(),k,":",v->toString());
					}
				}
				continue;
			}
			//then common clause triggers..
			//simple trigger(no overrides) first
			if(simpleTriggers.find(item) != simpleTriggers.end()) {
				TriggerItem* ti = items[item];
				bool error = false;
				CommonTrigger* ct = new CommonTrigger(ti);
				ct->base.reserve(ti->parameterType.size());
				for(auto it = subTag->tags.begin();it != subTag->tags.end();it++){
					//to be honest I do not want to handle sth like typo...
					//however,if the programme crash just because type 'value' to 'valve'
					//that would be annoyed...
					std::string trigger_name = stripTag(it->first);
					if(ti->parameterName.find(trigger_name) == ti->parameterName.end()) {
						//ignore this tag..
						error = true;
						delete ct;
						break;
					}
					int index = ti->parameterName[trigger_name];
					//parameter type mismatch..
					if(!isCastable(it->second,ti->parameterType[index])){
						error = true;
						delete ct;
						break; 
					}
					ct->base[index] = it->second;
				}
				if(!error){
					trigger->putTrigger(ct);
				}
				continue; 
			}
			//ignore those triggers which have not registered
			if(items.find(item) == items.end()) continue;
			//finally overrides claused trigger
			//f**king Paradox do not actually give a right localization text for some of those..
			//but for wikis,a appropriate localization is necessary.. 
			TriggerItem* ti = items[item];
			CommonTrigger* ct = new CommonTrigger(ti);
			OverrideHandler handler = overrideHandlers[item];
			
			ct->base.resize(ti->parameterType.size());
			//the original tag will never be used again so we just move directly~
			//besides the pointer it contains is well managed by another file
			//so we can modify it freely~ 
			std::map<std::string,ParadoxBase*> parameters = std::move(subTag->tags);
			bool success = handler(parameters);
			if(!success){
				delete ct;
				continue; 
			} 
			for(auto it = parameters.begin();it != parameters.end();it++){

				int index = ti->parameterName[it->first];
				ct->base[index] = it->second;
			} 
			trigger->putTrigger(ct);
			continue; 
		}
		else if(registeredTriggers.find(item) == registeredTriggers.end()) {

			//log_warning(current_location(),"unknown Trigger: ",item);
			continue;
		}
		if(loadedSTs.find(item) != loadedSTs.end()){
			if(!loadedSTs[item]->isFixed()) continue;
			else {
				
				ParadoxBoolean* pb = base->getAsBoolean(); 
				if(pb == nullptr) continue;
				std::map<std::string,ParadoxBase*> args;
				if(!pb->getValue()){
					args["__REVERSED__"] = nullptr;
				} 
				Trigger* ti = loadedSTs[item]->createInstance(args);
				SpecialTrigger* st = new SpecialTrigger(loadedSTs[item],ti);
				if(pb->getValue()) st->args[getLocalizationKeyPtr("__REVERSED__")] = nullptr;
				trigger->putTrigger(st);
			}
		}
		//for no overrides..
		if(simpleTriggers.find(item) != simpleTriggers.end()){
			
			TriggerItem* ti = items[item];
			ParadoxType type = ti->parameterType[0];
			ParadoxBase* base1 = castTo(base,type);
			if(base1 == nullptr) continue;
			CommonTrigger* ct = new CommonTrigger(ti);
			if(base1->getType() == ParadoxType::BOOLEAN) {
				ct->reversed = !base1->getAsBoolean()->getValue();
			}
			else {
				ct->pushObject(base1);
				
			}

			trigger->putTrigger(ct); 
			continue;
		}
		ParadoxType type = base->getType();
		if(type == ParadoxType::INTEGER){
			ParadoxInteger* pInteger = base->getAsInteger();
			bool flag = false;
			for(int i = 0;i < sizeof(INTEGER_MATCH_SEQUENCE) / sizeof(ParadoxType);i++) {
				std::string name("");
				name.append(item);
				name.append("@");
				name.append(std::to_string(static_cast<int>(INTEGER_MATCH_SEQUENCE[i])));
				if(!items.contains(name)) continue;
				TriggerItem* ti = items[name];
				ParadoxBase* arg1 = castTo(base,INTEGER_MATCH_SEQUENCE[i]);
				if(arg1 == nullptr) continue;
				CommonTrigger* ct = new CommonTrigger(ti);
				ct->pushObject(arg1);		
				trigger->putTrigger(ct);
				flag = true;
				break;
			}
			//when nothing matched
			if(!flag) log_error(current_location(),"ERROR: No Matching Trigger for \"",item," = ",pInteger->getIntegerContent() / 1000.0,"\"");
			
			
		}
		else if(type == ParadoxType::STRING){
			ParadoxString* pString = base->getAsString();
			bool flag = false;
			for(int i = 0;i < sizeof(STRING_MATCH_SEQUENCE) / sizeof(ParadoxType);i++){
				std::string name("");
				name.append(item);
				name.append("@");
				name.append(std::to_string(static_cast<int>(STRING_MATCH_SEQUENCE[i])));
				if(items.find(name) == items.end()) continue;		
				TriggerItem* ti = items[name];
				ParadoxBase* arg1 = castTo(pString,STRING_MATCH_SEQUENCE[i]);
				if(arg1 == nullptr) continue;
				CommonTrigger* ct = new CommonTrigger(ti);
				ct->pushObject(arg1);		
				trigger->putTrigger(ct);
				flag = true;
				break;
			}
			//when nothing matched
			if(!flag) log_error(current_location(),"#ERROR: No Matching Trigger for \"" , item , " = " , pString->getStringContent() , "\"");
		}
		else{
			std::string name("");
			name.append(item);
			name.append("@");
			name.append(std::to_string(static_cast<int>(type)));
			if(items.find(name) == items.end()) continue;
			TriggerItem* ti = items[name];
			CommonTrigger* ct = new CommonTrigger(ti);
			if(type == ParadoxType::BOOLEAN){
				ct->reversed = !base->getAsBoolean()->getValue();
			}
			else ct->pushObject(base);
			trigger->putTrigger(ct); 
		}
	}
}

ComplexTrigger* createBaseTrigger(){
	auto ct = new ChangeScopeTrigger(nullptr);
	return ct;
}

bool parseConditionalTrigger(ParadoxTag* tag,ConditionalTrigger* ct){
	ParadoxTag* lim = tag->get("limit",1)->getAsTag();
	ct->condition = createBaseTrigger();
	if(lim->tags.empty()) return false;
	parseTrigger(lim,ct->condition);
	parseTrigger(tag,ct); 
	return true;
}

