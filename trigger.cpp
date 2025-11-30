#include<vector>
#include "paradox_type.h"
#include "trigger.h"
#include "pattern.h"
#include "scope.h"
#include<map>
#include<iostream>
#include<set>

typedef bool(*OverrideHandler)(std::map<std::string,ParadoxBase*>&);

std::map<std::string,std::string> numberRequiredItems;
std::set<std::string> simpleTriggers;
std::set<std::string> registeredTriggers;
std::map<std::string,OverrideHandler> overrideHandlers;
std::map<std::string,TriggerItem*> items;

bool parseConditionalTrigger(ParadoxTag*,ConditionalTrigger*);

//No arg means only yes/no or even only allowing yes.
//those trigger will be registered as 1 BOOL type args.
//and no args will be used in localization text
void registerNoArgTrigger(std::string name,std::string pattern,std::string reversePattern){
	TriggerItem* item = new TriggerItem();
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(ParadoxType::BOOLEAN);
	items[name] = item;
	simpleTriggers.insert(name);
	registeredTriggers.insert(name);
}
//1 arg type.
//arg 0 will be used in patterns.
void registerSimpleTrigger(std::string name,std::string pattern,std::string reversePattern,ParadoxType type){
	TriggerItem* item = new TriggerItem();
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(type);
	item->usedParameter.push_back(0);
	items[name] = item;
	registeredTriggers.insert(name);
}

void registerSimpleClauseTrigger(std::string name,TriggerItem* triggerItem){
	simpleTriggers.insert(name);
	items[name] = triggerItem;
	registeredTriggers.insert(name);
}
void registerNumberRequiredTrigger(std::string name,std::string amountKey,std::string pattern,std::string reversePattern){
	TriggerItem* item = new TriggerItem();
	item->reversePattern = reversePattern;
	item->parameterType.push_back(ParadoxType::INTEGER);
	item->usedParameter.push_back(0);
	simpleTriggers.insert(name);
	numberRequiredItems[name] = amountKey;
	items[name] = item;
	registeredTriggers.insert(name);
}
void registerBooleanTrigger(std::string name,std::string pattern,std::string reversePattern){
	std::string actual_name(name);
	actual_name.append("@");
	actual_name.append(std::to_string(static_cast<int>(ParadoxType::BOOLEAN)));
	TriggerItem* item = new TriggerItem();
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(ParadoxType::BOOLEAN);
	items[actual_name] = item;
	registeredTriggers.insert(name);

}
void registerSingleArgTrigger(std::string name,std::string pattern,std::string reversePattern,ParadoxType type){
	if(type == ParadoxType::BOOLEAN){
		registerBooleanTrigger(name,pattern,reversePattern);
		return;
	}
	std::string actual_name(name);
	actual_name.append("@");
	actual_name.append(std::to_string(static_cast<int>(type)));
	TriggerItem* item = new TriggerItem();
	item->pattern = pattern;
	item->reversePattern = reversePattern;
	item->parameterType.push_back(type);
	item->usedParameter.push_back(0);
	items[actual_name] = item;
	registeredTriggers.insert(name);
}
void registerClausedTrigger(std::string name,TriggerItem* item,OverrideHandler handler){
	items[name] = item;
	overrideHandlers[name] = handler;
	registeredTriggers.insert(name);
}

void registerItems(){
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
	registerSimpleClauseTrigger("ai_attitude",new TriggerItem(
		{"%s对该国的态度为%s","%s为该国的态度不为%s"},
		{"who","attitude"},
		{ParadoxType::SCOPE,ParadoxType::STRING},
		{0,1}
	));
	registerSimpleTrigger("army_professionalism","陆军职业度%p%%","陆军职业度少于%p%%",ParadoxType::INTEGER);
	registerSingleArgTrigger("army_size","军队规模至少为%dK","军队规模小于%dK",ParadoxType::INTEGER);
	registerSingleArgTrigger("army_size","拥有至少和%s规模相同的军队","军队规模小于%s",ParadoxType::SCOPE);
	registerSimpleTrigger("army_size_percentage","军队规模至少为上限的%p%%","军队规模小于上限的%p%%",ParadoxType::INTEGER);
	registerSimpleClauseTrigger("army_strength",new TriggerItem(
		{"陆军实力至少为%s的%p%%","陆军实力少于%s的%p%%"},
		{"who","value"},
		{ParadoxType::SCOPE,ParadoxType::INTEGER},
		{0,1}
	));
	registerSingleArgTrigger("army_tradition","陆军传统至少为%d","陆军传统少于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("army_tradition","陆军传统不低于%s","陆军传统低于%s",ParadoxType::SCOPE);
	registerSingleArgTrigger("artillery_fraction","炮兵比例至少为%p%%","炮兵比例小于%p%%",ParadoxType::INTEGER);
	registerSingleArgTrigger("artillery_in_province","有至少%d队炮兵", "炮兵的数量小于%d队",ParadoxType::INTEGER);
	registerSingleArgTrigger("artillery_in_province","有来自%s的炮兵", "没有来自%s的炮兵",ParadoxType::SCOPE);
	registerSingleArgTrigger("authority","权威值至少为%d", "权威值小于%d",ParadoxType::INTEGER);
	registerSingleArgTrigger("authority","拥有至少与%s相同的权威值", "权威值小于%s",ParadoxType::SCOPE);
	registerSimpleTrigger("average_autonomy","平均自治度至少为%d%%","平均自治度低于%d%%",ParadoxType::INTEGER);
	
	registerSimpleTrigger("innovativeness","创新度至少为%d","创新度小于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("treasury","拥有至少%d[[File:crown.png]]","拥有少于%d[[File:crown]]",ParadoxType::INTEGER);
	registerNumberRequiredTrigger("num_of_owned_provinces_with","value","至少%d个拥有的省份满足下列条件:","少于%d个拥有的省份满足下列条件:");
	registerSimpleTrigger("base_manpower","基础人力至少为%d","基础人力少于%d",ParadoxType::INTEGER);
	registerSimpleTrigger("has_country_flag","国家标签'%s'已被设置","国家标签'%s'未被设置",ParadoxType::STRING);
	
	
	std::cout << "REGISTERD TRIGGER COUNT:" <<registeredTriggers.size() << std::endl;
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
				bool success = false;
				if(parameterType[index] == ParadoxType::INTEGER){
					success = p.setNextInteger(base1->getAsInteger()->getIntegerContent());
				}
				else if(parameterType[index] == ParadoxType::STRING){
					success = p.setNextString(base1->getAsString()->getStringContent());
				}
				else if(parameterType[index] == ParadoxType::DATE){
					success = p.setNextString(base1->getAsDate()->getDateContent().toString());			
				}
				else if(parameterType[index] == ParadoxType::SCOPE){
					std::string str = base1->getAsString()->getStringContent();
					Scope* scope = createScopeFromString(str);
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

TriggerItem::TriggerItem(std::pair<std::string,std::string>&& patterns,std::vector<std::string>&& parameterName,std::vector<ParadoxType>&& parameterType,std::vector<int>&& usedParameter){
	this->pattern = patterns.first;
	this->reversePattern = patterns.second;
	this->parameterType = parameterType;
	this->usedParameter = usedParameter;
	for(int i = 0;i < parameterName.size();i++){
		this->parameterName[parameterName[i]] = i;
	}
}

void preInit(Trigger* trigger,std::string& str){
	for(int i = 0;i < trigger->depth;i++){
		str.append("*");
	}
}
ComplexTrigger* Trigger::getAsComplexTrigger(){
	if(this->getType() == TriggerType::COMMON) return nullptr;
	return dynamic_cast<ComplexTrigger*>(this);
}

LogicTrigger* Trigger::getAsLogicTrigger(){
	if(this->getType() != TriggerType::LOGIC) return nullptr;
	return dynamic_cast<LogicTrigger*>(this);
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

ChangeScopeTrigger::ChangeScopeTrigger(Scope* scope){
	this->changedScope = scope;
	this->depth = 0;
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

std::string CommonTrigger::toString(bool reversed){
	std::string str("");
	preInit(this,str);
	str.append(this->item->toString(this->base,Xor(reversed,this->reversed)));
	this->depth = 0;
	return str;
}

std::string ChangeScopeTrigger::toString(bool reversed){
	std::string str("");
	if(this->subTriggers.empty()) return str;
	if(this->changedScope != nullptr){
		preInit(this,str);
		bool should_add_bracket = this->changedScope->getType() != ScopeType::SPECIAL;
		if(should_add_bracket) str.append("(");
		if(!should_add_bracket && !condition.empty()) str.append("满足特定条件的");
		if(use_type){
			str.append(trigger_type ? "所有" : "任意");
		}
		str.append(this->changedScope->toString());
		if(should_add_bracket) str.append(")");
		str.append(":\n");
	}
	if(!condition.empty()){
		preInit(this,str);
		str.append("条件:");
		for(int i = 0;i < this->condition.size();i++){
			str.append(this->condition[i]->toString(false));
			if(this->condition[i]->getType() == TriggerType::COMMON) str.append("\n");

		}
	}
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	return str;
}

std::string LogicTrigger::toString(bool reversed){
	std::string str("");
	int size = this->subTriggers.size();
	if(size == 0) return str;
	if(!this->ignored) preInit(this,str);
	LogicType actual_type = this->type;
	if(reversed){
		if(actual_type == LogicType::AND) actual_type = LogicType::OR;
		else if(actual_type == LogicType::OR) actual_type = LogicType::AND;
	}
	switch(actual_type){
		case LogicType::AND:
			if(size == 1){
				if(!ignored){
					this->ignored = true;
					ignoreCurrentDepth(this);
				}
				return this->subTriggers[0]->toString(reversed);
			}
			else{
				if(!this->omitted) str.append("下列条件需全部满足:\n");
				for(int i = 0;i < size;i++){
					LogicTrigger* subtrigger = this->subTriggers[i]->getAsLogicTrigger();
					bool shouldOmit = subtrigger != nullptr && subtrigger->type != LogicType::OR;
					if(shouldOmit){
						subtrigger->omitted = true;
						subtrigger->ignored = true; 
						ignoreCurrentDepth(subtrigger);
					} 
					str.append(this->subTriggers[i]->toString(reversed));
					if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
				}
			}
			break;
		case LogicType::OR:
			if(size == 1){
				if(!ignored){
					this->ignored = true;
					ignoreCurrentDepth(this);
				}
				return this->subTriggers[0]->toString(reversed);
			}
			else{
				if(!this->omitted) str.append("下列条件至少满足一个:\n");
				for(int i = 0;i < size;i++){
					LogicTrigger* subtrigger = this->subTriggers[i]->getAsLogicTrigger();
					bool shouldOmit = subtrigger != nullptr && subtrigger->type == LogicType::OR;
					if(shouldOmit){
						subtrigger->omitted = true;
						subtrigger->ignored = true; 
						ignoreCurrentDepth(subtrigger);
					} 
					str.append(this->subTriggers[i]->toString(reversed));
					if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
				}
			}
			break;
		case LogicType::NOT:
			if(size == 1){
				if(!ignored){
					this->ignored = true;
					ignoreCurrentDepth(this);
				}
				return this->subTriggers[0]->toString(!reversed);
			}
			else {
				if(!this->omitted) str.append("下列条件需全部满足:\n");
				for(int i = 0;i < size;i++){
					LogicTrigger* subtrigger = this->subTriggers[i]->getAsLogicTrigger();
					bool shouldOmit = subtrigger != nullptr && subtrigger->type == LogicType::OR;
					if(shouldOmit){
						subtrigger->omitted = true;
						subtrigger->ignored = true; 
						ignoreCurrentDepth(subtrigger);
					} 
					str.append(this->subTriggers[i]->toString(!reversed));
					if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
				}
			}
			break;
	}
	return str;
}

std::string NumberRequiredTrigger::toString(bool reversed){
	std::string str("");
	if(this->subTriggers.empty()) return str;
	preInit(this,str);
	ParadoxInteger* tmp = new ParadoxInteger(this->amount);
	std::vector<ParadoxBase*> base;
	base.push_back(tmp);
	str.append(this->item->toString(base,reversed));
	str.append("\n");
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	delete tmp; 
	return str;
}

std::string ConditionalTrigger::toString(bool reversed){
	std::string str("");
	if(this->subTriggers.empty()) return str;
	preInit(this,str);
	str.append("当以下条件满足时:\n");
	for(int i = 0;i < this->condition.size();i++){
		str.append(this->condition[i]->toString(false));
		if(this->condition[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	preInit(this,str);
	str.append("存在下列额外要求:\n");
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	//str.append("\n");
	return str;
}

std::string HiddenTrigger::toString(bool reversed){
	std::string str("");
	if(this->subTriggers.empty()) return str;
	if(this->hidden_current){
		if(!ignored){
			this->ignored = true;
			ignoreCurrentDepth(this);
		}
	}
	else {
		preInit(this,str);
		str.append("(隐藏条件):\n");
	}
	for(int i = 0;i < this->subTriggers.size();i++){
		str.append(this->subTriggers[i]->toString(reversed));
		if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
	}
	return str;
}

std::string CustomTooltipTrigger::toString(bool reversed){
	std::string str("");
	if(this->subTriggers.empty()) return str;
	if(!this->show_origin){
		preInit(this,str);
		str.append(this->tooltip);
		str.append("\n");
	}
	else{
		if(!ignored){
			this->ignored = true;
			ignoreCurrentDepth(this);
		}
		for(int i = 0;i < this->subTriggers.size();i++){
			str.append(this->subTriggers[i]->toString(reversed));
			if(this->subTriggers[i]->getType() == TriggerType::COMMON) str.append("\n");
		}
	}
	return str;
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
				
				ParadoxString* tt = subTag->get("tooltip",1)->getAsString();
				if(tt == nullptr) {
					//error case should be passed.
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
			ct->base.reserve(ti->parameterType.size());
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
		else if(registeredTriggers.find(item) == registeredTriggers.end()) continue;

		//for no overrides..
		if(simpleTriggers.find(item) != simpleTriggers.end()){
			TriggerItem* ti = items[item];
			ParadoxType type = ti->parameterType[0];
			if(!isCastable(base,type)) continue;
			CommonTrigger* ct = new CommonTrigger(ti);
			ct->pushObject(base);
			trigger->putTrigger(ct); 
			continue;
		}
		//for overrides
		
		int value = static_cast<int>(base->getType());
		if(isCastable(base,ParadoxType::BOOLEAN)){
			value = static_cast<int>(ParadoxType::BOOLEAN);
		}
		std::string name("");
		name.append(item);
		name.append("@");
		name.append(std::to_string(value));
		//if the parameter not fit...
		if(items.find(name) == items.end()) {
			//check is it a scope
			if(value == static_cast<int>(ParadoxType::INTEGER) || value == static_cast<int>(ParadoxType::STRING)){
				name = "";
				name.append(item);
				name.append("@");
				value = static_cast<int>(ParadoxType::SCOPE);
				name.append(std::to_string(value));
				if(items.find(name) == items.end()) continue;
			}
			else continue;
		}
		else if(base->getType() == ParadoxType::STRING){
			std::string scope_name(item);
			scope_name.append("@");
			scope_name.append(std::to_string(static_cast<int>(ParadoxType::SCOPE)));
			if(items.find(scope_name) != items.end() && isCastable(base,ParadoxType::SCOPE)){
				name = scope_name;
			}
		}
		
		TriggerItem* ti = items[name];
		ParadoxType type = ti->parameterType[0];

		//now it is not necessary...
		//if(!isCastable(base,type)) continue;

		CommonTrigger* ct = new CommonTrigger(ti);
		if(type == ParadoxType::BOOLEAN){
			ct->reversed = castToBool(base->getAsString());
		}
		else ct->pushObject(base);
		
		trigger->putTrigger(ct); 
	}
}

ComplexTrigger* createBaseTrigger(){
	return new ChangeScopeTrigger(nullptr);
}

std::vector<Trigger*> parseTriggerList(ParadoxTag* tag,int root_depth){
	ChangeScopeTrigger cst(nullptr);
	cst.depth = root_depth;
	parseTrigger(tag,&cst);
	std::vector<Trigger*> vec = std::move(cst.subTriggers);
	return vec;
}
bool parseConditionalTrigger(ParadoxTag* tag,ConditionalTrigger* ct){
	ParadoxTag* lim = tag->get("limit",1)->getAsTag();
	if(lim->tags.empty()) return false;
	ct->condition = parseTriggerList(lim,ct->depth);
	parseTrigger(tag,ct); 
	return true;
}

