#include "modifier.h"
#include "paradox_type.h"
#include <iostream>
#include <cmath>
/*
REPLACE REGEXS...
+%:
//(.+) (.+) \+%
registerModifier\("$1",ModifierType::PERCENTAGE,"$2"\);
//(.+) (.+) -%
registerModifier\("$1",ModifierType::MINUS_PERCENTAGE,"$2"\);
//(.+) (.+) \+
registerModifier\("$1",ModifierType::NORMAL,"$2"\);
//(.+) (.+) -
registerModifier\("$1",ModifierType::MINUS,"$2"\);
//(.+) (.+) =
registerModifier\("$1",ModifierType::NEUTRAL,"$2"\);
//(.+) (.+) =%
registerModifier\("$1",ModifierType::NEUTRAL_PERCENTAGE,"$2"\);

*/
std::map<std::string,ModifierObject> modifierObjects; 

void registerModifier(std::string modifierName,ModifierType type,std::string localizedName){
	ModifierObject obj;
	obj.localizedName = localizedName;
	obj.type = type;
	modifierObjects[modifierName] = obj;
}

void generatePercentage(int value,char* str){
	int pValue = value / 10;
	int qValue = std::abs(value % 10);
	if(qValue == 0){
		if(pValue >= 0) sprintf(str,"+%d%% ",pValue);
		else sprintf(str,"%d%% ",pValue);
	}
	else {
		if(pValue > 0 || (pValue == 0 && qValue >= 0)) sprintf(str,"+%d.%d%% ",pValue,qValue);
		else sprintf(str,"%d.%d%% ",pValue,qValue);
	}
} 
void generateValue(int value,char* str){
	int pValue = value / 1000;
	int qValue = std::abs(value % 1000);
	if(qValue == 0){
		if(pValue >= 0) sprintf(str,"+%d ",pValue);
		else sprintf(str,"%d ",pValue);
	}
	else {
		while(qValue % 10 == 0) qValue /= 10;
		if(pValue > 0 || (pValue == 0 && qValue >= 0)) sprintf(str,"+%d.%d ",pValue,qValue);
		else sprintf(str,"%d.%d ",pValue,qValue);
	}
} 
void generatePercentage2(int value,char* str){
	int pValue = value / 1000;
	int qValue = std::abs(value % 1000);
	if(qValue == 0){
		if(pValue >= 0) sprintf(str,"+%d%% ",pValue);
		else sprintf(str,"%d%% ",pValue);
	}
	else {
		while(qValue % 10 == 0) qValue /= 10;
		if(pValue > 0 || (pValue == 0 && qValue >= 0)) sprintf(str,"+%d.%d%% ",pValue,qValue);
		else sprintf(str,"%d.%d%% ",pValue,qValue);
	}
} 

void loadInternalModifier(){
	registerModifier("can_not_build_colonies",ModifierType::MINUS_MARK,"无法建立殖民地");
	registerModifier("can_not_build_buildings",ModifierType::MINUS_MARK,"无法建设建筑");	
	registerModifier("can_not_build_missionaries",ModifierType::MINUS_MARK,"无法派遣传教士");	
	registerModifier("can_not_declare_war",ModifierType::MINUS_MARK,"无法宣战");	
	registerModifier("can_not_send_merchants",ModifierType::MINUS_MARK,"无法派遣商人团");
	registerModifier("capped_by_forcelimit",ModifierType::MINUS_MARK,"无法超过军队上限");
	registerModifier("can_claim_states",ModifierType::MARK,"可以宣称整个州");
	registerModifier("free_concentrate_development",ModifierType::MARK,"能够无损集中发展度");
	registerModifier("build_cost",ModifierType::MINUS_PERCENTAGE,"建造花费");
	registerModifier("local_build_cost",ModifierType::MINUS_PERCENTAGE,"本地建造花费");
	registerModifier("build_time",ModifierType::MINUS_PERCENTAGE,"建造时间");
	registerModifier("local_build_time",ModifierType::MINUS_PERCENTAGE,"本地建造时间");
	registerModifier("local_unrest",ModifierType::MINUS,"本地叛乱度");
	registerModifier("global_unrest",ModifierType::MINUS,"全国叛乱度");
	registerModifier("development_cost",ModifierType::MINUS_PERCENTAGE,"发展成本");
	registerModifier("development_cost_modifier",ModifierType::MINUS_PERCENTAGE,"发展成本修正");
	registerModifier("local_development_cost",ModifierType::MINUS_PERCENTAGE,"本地发展成本");
	registerModifier("local_development_cost_modifier",ModifierType::MINUS_PERCENTAGE,"本地发展成本修正");
	registerModifier("trade_efficiency",ModifierType::PERCENTAGE,"贸易效率");
	registerModifier("province_trade_power_value",ModifierType::NORMAL,"省份贸易力量");
	registerModifier("province_trade_power_modifier",ModifierType::PERCENTAGE,"省份贸易力量修正");
	registerModifier("global_prov_trade_power_modifier",ModifierType::PERCENTAGE,"全局省份贸易力量修正");
	registerModifier("production_efficiency",ModifierType::PERCENTAGE,"生产效率");
	registerModifier("local_production_efficiency",ModifierType::PERCENTAGE,"本地生产效率");
	registerModifier("trade_goods_size",ModifierType::NORMAL,"商品产出");
	registerModifier("trade_goods_size_modifier",ModifierType::PERCENTAGE,"商品产出修正");
	registerModifier("raze_power_gain",ModifierType::PERCENTAGE,"焚掠获得点数");
	registerModifier("monarch_power_tribute",ModifierType::PERCENTAGE,"来自朝贡国的君主点数朝贡");
	registerModifier("tributary_conversion_cost_modifier",ModifierType::MINUS_PERCENTAGE,"朝贡国转向附庸国花费修正");
	registerModifier("expand_infrastructure_cost_modifier",ModifierType::MINUS_PERCENTAGE,"扩建基础设施花费修正");
	registerModifier("max_absolutism_effect",ModifierType::MINUS_PERCENTAGE,"最大专制度效果");	
	registerModifier("centralize_state_cost",ModifierType::MINUS_PERCENTAGE,"州权力集中化花费");
	registerModifier("local_centralize_state_cost",ModifierType::MINUS_PERCENTAGE,"本地州权力集中化");
	registerModifier("land_morale_constant",ModifierType::NORMAL,"陆军士气");
	registerModifier("naval_morale_constant",ModifierType::NORMAL,"海军士气");
	registerModifier("max_general_shock",ModifierType::NORMAL,"最大将领冲击");
	registerModifier("max_general_fire",ModifierType::NORMAL,"最大将领活力");
	registerModifier("max_general_maneuver",ModifierType::NORMAL,"最大将领机动");
	registerModifier("max_general_siege",ModifierType::NORMAL,"最大将领围城");
	registerModifier("max_admiral_shock",ModifierType::NORMAL,"最大海军将领冲击");
	registerModifier("max_admiral_fire",ModifierType::NORMAL,"最大海军将领火力");
	registerModifier("max_admiral_maneuver",ModifierType::NORMAL,"最大海军将领机动");
	registerModifier("max_admiral_siege",ModifierType::NORMAL,"最大海军将领围城");
	registerModifier("coast_raid_range",ModifierType::NORMAL,"劫掠海岸范围");
	registerModifier("development_cost_in_primary_culture",ModifierType::MINUS_PERCENTAGE,"主流文化发展成本");
	registerModifier("reduced_trade_penalty_on_non_main_tradenode",ModifierType::MINUS_PERCENTAGE,"非贸易本埠节点贸易惩罚");
	registerModifier("colony_cost_modifier",ModifierType::MINUS_PERCENTAGE,"殖民花费修正");
	registerModifier("local_colony_cost_modifier",ModifierType::MINUS_PERCENTAGE,"本地殖民花费修正");
	registerModifier("spy_action_cost_modifier",ModifierType::MINUS_PERCENTAGE,"间谍行动花费修正");
	registerModifier("placed_merchant_power_modifier",ModifierType::PERCENTAGE,"已配置商人团竞争力修正");
	registerModifier("reduced_liberty_desire_on_other_continent",ModifierType::MINUS_PERCENTAGE,"降低其他大洲属国独立倾向");
	registerModifier("overextension_impact_modifier",ModifierType::MINUS_PERCENTAGE,"过度扩张影响修正");
	registerModifier("artillery_level_modifier",ModifierType::NORMAL,"炮兵等级对围城的贡献");
	registerModifier("local_tolerance_of_heretics",ModifierType::NORMAL,"本地异端容忍度");
	registerModifier("local_tolerance_of_heathens",ModifierType::NORMAL,"本地异教容忍度");
	registerModifier("tax_income",ModifierType::NORMAL,"年度税收");
	registerModifier("global_tax_income",ModifierType::NORMAL,"年度税收");
	registerModifier("local_tax_modifier",ModifierType::PERCENTAGE,"本地税收修正");
	registerModifier("global_tax_modifier",ModifierType::PERCENTAGE,"国家税收修正");
	registerModifier("stability_cost_modifier",ModifierType::MINUS_PERCENTAGE,"稳定度花费修正");
	registerModifier("inflation_reduction",ModifierType::PERCENTAGE2,"每年通货膨胀减量");
	registerModifier("inflation_reduction_local",ModifierType::PERCENTAGE2,"每年通货膨胀减量");
	registerModifier("interest",ModifierType::MINUS_PERCENTAGE2,"年利息");
	registerModifier("colonists",ModifierType::NORMAL,"殖民队");
	registerModifier("missionaries",ModifierType::NORMAL,"传教士");
	registerModifier("merchants",ModifierType::NORMAL,"商人团");
	registerModifier("diplomats",ModifierType::NORMAL,"外交官");
	registerModifier("global_trade_power",ModifierType::PERCENTAGE,"全局贸易竞争力");
	registerModifier("global_foreign_trade_power",ModifierType::PERCENTAGE,"国外贸易竞争力");
	registerModifier("global_own_trade_power",ModifierType::PERCENTAGE,"国内贸易竞争力");
	registerModifier("colonist_placement_chance",ModifierType::PERCENTAGE,"定居几率");
	registerModifier("local_colonist_placement_chance",ModifierType::PERCENTAGE,"本地殖民几率");
	registerModifier("global_missionary_strength",ModifierType::PERCENTAGE,"传教强度");
	registerModifier("local_missionary_strength",ModifierType::PERCENTAGE,"本地传教强度");
	registerModifier("land_morale",ModifierType::PERCENTAGE,"陆军士气");
	registerModifier("naval_morale",ModifierType::PERCENTAGE,"海军士气");
	registerModifier("local_manpower",ModifierType::NORMAL,"本地人力");
	registerModifier("global_manpower",ModifierType::NORMAL,"国家人力");
	registerModifier("local_manpower_modifier",ModifierType::PERCENTAGE,"本地人力修正");
	registerModifier("global_manpower_modifier",ModifierType::PERCENTAGE,"国家人力修正");
	registerModifier("manpower_recovery_speed",ModifierType::PERCENTAGE,"人力恢复速度");
	registerModifier("morale_damage_received",ModifierType::MINUS_PERCENTAGE,"所受士气打击");
	registerModifier("morale_damage",ModifierType::PERCENTAGE,"士气打击");
	registerModifier("military_tactics",ModifierType::NORMAL,"军事战术");
	registerModifier("local_sailors",ModifierType::NORMAL,"本地水手");
	registerModifier("global_sailors",ModifierType::NORMAL,"国家水手");
	registerModifier("local_sailors_modifier",ModifierType::PERCENTAGE,"本地水手修正");
	registerModifier("global_sailors_modifier",ModifierType::PERCENTAGE,"国家水手修正");
	registerModifier("sailors_recovery_speed",ModifierType::PERCENTAGE,"水手恢复速度");
	registerModifier("land_forcelimit",ModifierType::NORMAL,"陆军规模上限");
	registerModifier("naval_forcelimit",ModifierType::NORMAL,"海军规模上限");
	registerModifier("overlord_naval_forcelimit",ModifierType::NORMAL,"宗主国海军规模上限");
	registerModifier("overlord_naval_forcelimit_modifier",ModifierType::PERCENTAGE,"宗主国海军规模上限修正");
	registerModifier("land_forcelimit_modifier",ModifierType::PERCENTAGE,"陆军规模上限修正");
	registerModifier("naval_forcelimit_modifier",ModifierType::PERCENTAGE,"海军规模上限修正");
	registerModifier("land_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"陆军维护费修正");
	registerModifier("naval_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"海军维护费修正");
	registerModifier("merc_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"雇佣兵维护费");
	registerModifier("fort_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"要塞维护费");
	registerModifier("local_fort_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"本地要塞维护费");
	registerModifier("mercenary_cost",ModifierType::MINUS_PERCENTAGE,"雇佣兵花费");
	registerModifier("infantry_cost",ModifierType::MINUS_PERCENTAGE,"步兵组建费用");
	registerModifier("cavalry_cost",ModifierType::MINUS_PERCENTAGE,"骑兵组建费用");
	registerModifier("artillery_cost",ModifierType::MINUS_PERCENTAGE,"炮兵组建费用");
	registerModifier("heavy_ship_cost",ModifierType::MINUS_PERCENTAGE,"重型战舰建造费用");
	registerModifier("light_ship_cost",ModifierType::MINUS_PERCENTAGE,"轻型战舰建造费用");
	registerModifier("galley_cost",ModifierType::MINUS_PERCENTAGE,"桨帆战舰建造费用");
	registerModifier("transport_cost",ModifierType::MINUS_PERCENTAGE,"运输船建造费用");
	registerModifier("infantry_power",ModifierType::PERCENTAGE,"步兵作战能力");
	registerModifier("cavalry_power",ModifierType::PERCENTAGE,"骑兵作战能力");
	registerModifier("artillery_power",ModifierType::PERCENTAGE,"炮兵作战能力");
	registerModifier("heavy_ship_power",ModifierType::PERCENTAGE,"重型战舰作战能力");
	registerModifier("light_ship_power",ModifierType::PERCENTAGE,"轻型战舰作战能力");
	registerModifier("galley_power",ModifierType::PERCENTAGE,"桨帆战舰作战能力");
	registerModifier("transport_power",ModifierType::PERCENTAGE,"运输船只作战能力");
	registerModifier("attrition",ModifierType::PERCENTAGE2,"本地损耗");
	registerModifier("hostile_attrition",ModifierType::PERCENTAGE2,"全局敌军损耗");
	registerModifier("artillery_barrage_cost",ModifierType::MINUS_PERCENTAGE,"炮兵轰击花费");
	registerModifier("transport_attrition",ModifierType::MINUS_PERCENTAGE,"装载进船只时的损耗");
	registerModifier("land_attrition",ModifierType::MINUS_PERCENTAGE,"陆军损耗");
	registerModifier("naval_attrition",ModifierType::MINUS_PERCENTAGE,"海军损耗");
	registerModifier("max_attrition",ModifierType::NEUTRAL_PERCENTAGE2,"最大损耗");
	registerModifier("max_hostile_attrition",ModifierType::PERCENTAGE2,"最大敌军损耗");
	registerModifier("supply_limit",ModifierType::NORMAL,"补给上限");
	registerModifier("war_exhaustion",ModifierType::MINUS,"每月厌战度");
	registerModifier("war_exhaustion_cost",ModifierType::MINUS,"降低厌战度的花费");
	registerModifier("local_hostile_attrition",ModifierType::PERCENTAGE2,"本地敌军损耗");
	registerModifier("army_tradition",ModifierType::NORMAL,"每年陆军传统");
	registerModifier("navy_tradition",ModifierType::NORMAL,"每年海军传统");
	registerModifier("army_tradition_decay",ModifierType::MINUS_PERCENTAGE,"年度陆军传统衰减");
	registerModifier("navy_tradition_decay",ModifierType::MINUS_PERCENTAGE,"年度海军传统衰减");
	registerModifier("leader_land_fire",ModifierType::NORMAL,"陆军将领火力");
	registerModifier("leader_land_shock",ModifierType::NORMAL,"陆军将领冲击");
	registerModifier("leader_naval_fire",ModifierType::NORMAL,"海军将领火力");
	registerModifier("leader_naval_shock",ModifierType::NORMAL,"海军将领冲击");
	registerModifier("leader_siege",ModifierType::NORMAL,"陆军将领围城");
	registerModifier("leader_land_manuever",ModifierType::NORMAL,"陆军将领机动");
	registerModifier("leader_naval_manuever",ModifierType::NORMAL,"海军将领机动");
	registerModifier("state_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"直属州维护费修正");
	registerModifier("local_state_maintenance_modifier",ModifierType::MINUS_PERCENTAGE,"本地直属州维护费修正");
	registerModifier("global_spy_defence",ModifierType::PERCENTAGE,"外国间谍察觉");
	registerModifier("spy_offence",ModifierType::PERCENTAGE,"间谍网建设");
	registerModifier("trade_value",ModifierType::NORMAL,"贸易价值");
	registerModifier("fort_level",ModifierType::NORMAL,"要塞等级");
	registerModifier("blockade_efficiency",ModifierType::PERCENTAGE,"封锁效率");
	registerModifier("ship_recruit_speed",ModifierType::MINUS_PERCENTAGE,"本地造船时间");
	registerModifier("regiment_recruit_speed",ModifierType::MINUS_PERCENTAGE,"本地募兵时间");
	registerModifier("global_ship_recruit_speed",ModifierType::MINUS_PERCENTAGE,"造船时间");
	registerModifier("global_regiment_recruit_speed",ModifierType::MINUS_PERCENTAGE,"募兵时间");
	registerModifier("prestige",ModifierType::NORMAL,"年度威望");
	registerModifier("prestige_decay",ModifierType::MINUS_PERCENTAGE,"年度威望衰减");
	registerModifier("prestige_from_land",ModifierType::PERCENTAGE,"陆战带来的威望");
	registerModifier("prestige_from_naval",ModifierType::PERCENTAGE,"海战带来的威望");
	registerModifier("trade_value_modifier",ModifierType::PERCENTAGE,"贸易价值修正");
	registerModifier("garrison_growth",ModifierType::PERCENTAGE,"要塞驻军增长");
	registerModifier("global_garrison_growth",ModifierType::PERCENTAGE,"国家要塞驻军增长");
	registerModifier("advisor_cost",ModifierType::MINUS_PERCENTAGE,"顾问花费");
	registerModifier("advisor_pool",ModifierType::NORMAL,"可能的顾问");
	registerModifier("female_advisor_chance",ModifierType::PERCENTAGE,"女性顾问概率");
	registerModifier("technology_cost",ModifierType::MINUS_PERCENTAGE,"科研费用");
	registerModifier("discipline",ModifierType::PERCENTAGE,"训练度");
	registerModifier("reinforce_speed",ModifierType::PERCENTAGE,"补充速度");
	registerModifier("range",ModifierType::PERCENTAGE,"殖民距离");
	registerModifier("global_colonial_growth",ModifierType::NORMAL,"全局移民增加");
	registerModifier("local_colonial_growth",ModifierType::NORMAL,"本地移民增加");
	registerModifier("tolerance_own",ModifierType::NORMAL,"正统信仰容忍");
	registerModifier("tolerance_heretic",ModifierType::NORMAL,"异端容忍度");
	registerModifier("tolerance_heathen",ModifierType::NORMAL,"异教容忍度");
	registerModifier("defensiveness",ModifierType::PERCENTAGE,"防御效率");
	registerModifier("local_defensiveness",ModifierType::PERCENTAGE,"本地防御效率");
	registerModifier("global_ship_cost",ModifierType::MINUS_PERCENTAGE,"船只花费");
	registerModifier("global_ship_repair",ModifierType::MINUS_PERCENTAGE,"全局船只修理");
	registerModifier("global_regiment_cost",ModifierType::MINUS_PERCENTAGE,"部队花费");
	registerModifier("global_tariffs",ModifierType::PERCENTAGE2,"全局关税");
	registerModifier("diplomatic_reputation",ModifierType::NORMAL,"外交声誉");
	registerModifier("papal_influence",ModifierType::PERCENTAGE,"教廷影响力");
	registerModifier("devotion",ModifierType::NORMAL,"年度奉献度");
	registerModifier("legitimacy",ModifierType::NORMAL,"每年正统性");
	registerModifier("horde_unity",ModifierType::NORMAL,"每年部落团结");
	registerModifier("republican_tradition",ModifierType::NORMAL,"每年共和传统");
	registerModifier("monthly_splendor",ModifierType::NORMAL,"每月辉煌点数");
	registerModifier("country_admin_power",ModifierType::NORMAL,"每月行政点数");
	registerModifier("country_military_power",ModifierType::NORMAL,"每月军事点数");
	registerModifier("country_diplomatic_power",ModifierType::NORMAL,"每月外交点数");
	registerModifier("idea_cost",ModifierType::MINUS_PERCENTAGE,"理念花费");
	registerModifier("shock_damage",ModifierType::PERCENTAGE,"冲击伤害");
	registerModifier("fire_damage",ModifierType::PERCENTAGE,"火力伤害");
	registerModifier("diplomatic_annexation_cost",ModifierType::MINUS_PERCENTAGE,"外交合并花费");
	registerModifier("diplomatic_upkeep",ModifierType::NORMAL,"外交关系");
	registerModifier("ae_impact",ModifierType::MINUS_PERCENTAGE,"侵略扩张影响");
	registerModifier("improve_relation_modifier",ModifierType::PERCENTAGE,"改善关系");
	registerModifier("movement_speed",ModifierType::PERCENTAGE,"移动速度");
	registerModifier("siege_ability",ModifierType::PERCENTAGE,"围城能力");
	registerModifier("monthly_reform_progress_modifier",ModifierType::PERCENTAGE,"月度改革进度修正");
	registerModifier("global_monthly_devastation",ModifierType::MINUS,"全局每月荒废度");
	registerModifier("global_prosperity_growth",ModifierType::NORMAL,"全局繁荣度增长");
	registerModifier("mercenary_discipline",ModifierType::PERCENTAGE,"雇佣兵训练度");
	registerModifier("mercenary_manpower",ModifierType::PERCENTAGE,"雇佣兵人力");
	registerModifier("promote_culture_cost",ModifierType::MINUS_PERCENTAGE,"认同文化花费");	
	registerModifier("liberty_desire_from_subject_development",ModifierType::MINUS_PERCENTAGE,"属国发展度带来的独立倾向");	
	registerModifier("manpower_in_accepted_culture_provinces",ModifierType::PERCENTAGE,"相容文化省份的人力");
	registerModifier("manpower_in_own_culture_provinces",ModifierType::PERCENTAGE,"主流文化省份的人力");
	registerModifier("global_attacker_dice_roll_bonus",ModifierType::NORMAL,"全局进攻方骰子数值奖励");
	registerModifier("local_attacker_dice_roll_bonus",ModifierType::NORMAL,"本地进攻方骰子数值奖励");
	registerModifier("global_defender_dice_roll_bonus",ModifierType::NORMAL,"全局防御方骰子数值奖励");
    registerModifier("local_defender_dice_roll_bonus",ModifierType::NORMAL,"本地防御方骰子数值奖励");
    registerModifier("own_coast_naval_combat_bonus",ModifierType::NORMAL,"己方临海省份海域海战奖励");
    registerModifier("local_own_coast_naval_combat_bonus",ModifierType::NORMAL,"当地省份海岸己方海战奖励");
    registerModifier("own_territory_dice_roll_bonus",ModifierType::NORMAL,"己方领地骰子数值奖励");
	registerModifier("trade_steering",ModifierType::PERCENTAGE,"贸易引导");	registerModifier("free_policy",ModifierType::NORMAL,"免费政策");
	registerModifier("free_adm_policy",ModifierType::NORMAL,"免费行政政策");
    registerModifier("free_dip_policy",ModifierType::NORMAL,"免费外交政策");
    registerModifier("free_mil_policy",ModifierType::NORMAL,"免费军事政策");
	registerModifier("province_warscore_cost",ModifierType::MINUS_PERCENTAGE,"省份战争分数花费");	registerModifier("yearly_corruption",ModifierType::MINUS,"每年腐败度");
	registerModifier("artillery_levels_available_vs_fort",ModifierType::NORMAL,"炮兵围攻加成上限");
	registerModifier("reform_progress_growth",ModifierType::PERCENTAGE,"改革进度增长");
	registerModifier("warscore_cost_vs_other_religion",ModifierType::MINUS_PERCENTAGE,"对其他宗教的战争分数花费");
	registerModifier("local_monthly_devastation",ModifierType::MINUS,"本地每月荒废度");
	registerModifier("local_prosperity_growth",ModifierType::NORMAL,"本地繁荣度增长");
	registerModifier("local_governing_cost",ModifierType::MINUS_PERCENTAGE,"省份治理成本");
	registerModifier("governing_cost",ModifierType::MINUS_PERCENTAGE,"治理成本");
    registerModifier("trade_company_governing_cost",ModifierType::MINUS_PERCENTAGE,"贸易公司治理成本");
    registerModifier("state_governing_cost",ModifierType::MINUS_PERCENTAGE,"直属州治理成本");
    registerModifier("territories_governing_cost",ModifierType::MINUS_PERCENTAGE,"自治领地治理成本");
    registerModifier("local_governing_cost_increase",ModifierType::MINUS,"本地治理成本增加");
    registerModifier("state_governing_cost_increase",ModifierType::MINUS,"直属州治理成本增加");
    registerModifier("statewide_governing_cost",ModifierType::MINUS_PERCENTAGE,"全州范围内治理成本");
	registerModifier("local_institution_spread",ModifierType::PERCENTAGE,"本地思潮传播");
    registerModifier("global_institution_spread",ModifierType::PERCENTAGE,"全局思潮传播");
	registerModifier("supply_limit_modifier",ModifierType::PERCENTAGE,"补给上限修正");
	registerModifier("global_supply_limit_modifier",ModifierType::PERCENTAGE,"国家补给上限修正");
	registerModifier("min_autonomy",ModifierType::MINUS_PERCENTAGE2,"最低自治度");
    registerModifier("global_autonomy",ModifierType::MINUS,"每月自治度变化");
    registerModifier("min_local_autonomy",ModifierType::MINUS_PERCENTAGE2,"最低本地自治度");
	registerModifier("local_autonomy",ModifierType::MINUS,"本地每月自治度变化");
    registerModifier("culture_conversion_cost",ModifierType::MINUS_PERCENTAGE,"转变文化花费");
    registerModifier("local_culture_conversion_cost",ModifierType::MINUS,"本地转变文化花费");
    registerModifier("culture_conversion_time",ModifierType::MINUS_PERCENTAGE,"文化转换时间");
    registerModifier("local_culture_conversion_time",ModifierType::MINUS_PERCENTAGE,"本地文化转换时间");
	registerModifier("local_friendly_movement_speed",ModifierType::PERCENTAGE,"友方军队移动速度");
	registerModifier("local_hostile_movement_speed",ModifierType::MINUS_PERCENTAGE,"敌方军队移动速度");
	registerModifier("local_regiment_cost",ModifierType::MINUS_PERCENTAGE,"本地部队花费");
	registerModifier("block_slave_raid",ModifierType::MARK,"无法劫掠海岸");
    registerModifier("may_perform_slave_raid",ModifierType::MARK,"可以劫掠海岸");
    registerModifier("may_perform_slave_raid_on_same_religion",ModifierType::MARK,"可以劫掠海岸（包括相同宗教国家）");
	
	registerModifier("relation_with_heretics",ModifierType::NORMAL,"对异端国家的好感度");
	registerModifier("global_religious_conversion_resistance",ModifierType::PERCENTAGE,"对宗教改革的抵抗能力");
	registerModifier("institution_spread_from_true_faith",ModifierType::PERCENTAGE,"正统信仰省份中的思潮传播");
	registerModifier("embracement_cost",ModifierType::MINUS_PERCENTAGE,"接纳思潮花费");
	registerModifier("curia_treasury_contribution",ModifierType::PERCENTAGE,"向教廷金库献金");
	registerModifier("unrest_catholic_provinces",ModifierType::MINUS,"揭秘教省份动乱");
	registerModifier("prestige_per_development_from_conversion",ModifierType::NORMAL,"每点发展度传教带来的威望");
	registerModifier("religious_unity",ModifierType::PERCENTAGE,"宗教统一");
	registerModifier("yearly_absolutism",ModifierType::NORMAL,"每年专制度");
	registerModifier("missionary_maintenance_cost",ModifierType::MINUS_PERCENTAGE,"传教维护费");
	registerModifier("manpower_in_true_faith_provinces",ModifierType::PERCENTAGE,"正统信仰省份提供的人力");
	registerModifier("curia_powers_cost",ModifierType::MINUS_PERCENTAGE,"教廷影响力花费");
	
	registerModifier("harsh_treatment_cost",ModifierType::MINUS_PERCENTAGE,"严酷镇压花费");
	registerModifier("all_power_cost",ModifierType::MINUS_PERCENTAGE,"所有点数花费");	
	registerModifier("loot_amount",ModifierType::PERCENTAGE,"掠夺速度");
	
	registerModifier("expand_administration_cost",ModifierType::MINUS_PERCENTAGE,"扩大行政机构花费");
	registerModifier("monarch_lifespan",ModifierType::PERCENTAGE,"统治者平均寿命");
	registerModifier("envoy_travel_time",ModifierType::MINUS_PERCENTAGE,"特使行程时间");
	registerModifier("accept_vassalization_reasons",ModifierType::NORMAL,"外交附庸接受因素");
	registerModifier("capture_ship_chance",ModifierType::PERCENTAGE,"俘获敌舰几率");
	
	registerModifier("yearly_harmony",ModifierType::NORMAL,"年度和谐度");
	registerModifier("global_trade_goods_size_modifier",ModifierType::PERCENTAGE,"全局商品产出修正");
	registerModifier("max_absolutism",ModifierType::NORMAL,"最大专制度");
	registerModifier("core_creation",ModifierType::MINUS_PERCENTAGE,"核心化花费");
	registerModifier("shock_damage_received",ModifierType::MINUS_PERCENTAGE,"受到的冲击伤害");
	registerModifier("reinforce_cost_modifier",ModifierType::MINUS_PERCENTAGE,"军队补员花费");
	registerModifier("min_autonomy_in_territories",ModifierType::MINUS_PERCENTAGE,"自治领地最低自治度");
	registerModifier("adm_tech_cost_modifier",ModifierType::MINUS_PERCENTAGE,"行政科技花费");
	
	registerModifier("local_ship_cost",ModifierType::MINUS_PERCENTAGE,"本地造船花费");
	registerModifier("local_ship_repair",ModifierType::PERCENTAGE,"本地船只维修");
	registerModifier("trade_range_modifier",ModifierType::PERCENTAGE,"贸易范围修正");
	registerModifier("global_heretic_missionary_strength",ModifierType::PERCENTAGE,"全局异端传教力量");
	registerModifier("global_heathen_missionary_strength",ModifierType::PERCENTAGE,"全局异教传教力量");
	registerModifier("enemy_core_creation",ModifierType::PERCENTAGE,"敌方国家建立核心成本");
	registerModifier("free_leader_pool",ModifierType::NORMAL,"免维护点数将领");
	registerModifier("heir chance",ModifierType::PERCENTAGE,"新继承人产生几率");
	registerModifier("embargo_efficiency",ModifierType::PERCENTAGE,"禁运效率");
	registerModifier("recover_army_morale_speed",ModifierType::PERCENTAGE,"陆军士气恢复速度");
	registerModifier("recover_navy_morale_speed",ModifierType::PERCENTAGE,"海军士气回复速度");
	registerModifier("unjustified_demands",ModifierType::PERCENTAGE,"不合理的要求花费");
	registerModifier("fabricate_claims_cost",ModifierType::MINUS_PERCENTAGE,"伪造宣称花费");
	
	registerModifier("claim_duration",ModifierType::PERCENTAGE,"宣称期限");
	registerModifier("regiment_manpower_usage",ModifierType::MINUS_PERCENTAGE,"部队人力使用");
	registerModifier("all_estate_influence_modifier",ModifierType::NEUTRAL_PERCENTAGE,"所有阶层影响修正");
	registerModifier("justify_trade_conflict_cost",ModifierType::MINUS_PERCENTAGE,"正当化贸易争端花费");
	registerModifier("rebel_support_efficiency",ModifierType::PERCENTAGE,"叛军支持效率");
	registerModifier("discovered_relations_impact",ModifierType::MINUS_PERCENTAGE,"间谍被发现对关系影响");
 	registerModifier("annexation_relations_impact",ModifierType::MINUS_PERCENTAGE,"吞并属国对关系影响");
	registerModifier("vassal_income",ModifierType::PERCENTAGE,"附庸国进贡收入");
	registerModifier("inflation_action_cost",ModifierType::MINUS_PERCENTAGE,"降低通胀花费");
	registerModifier("migration_cost",ModifierType::MINUS_PERCENTAGE,"迁徙成本");
	registerModifier("add_tribal_land_cost",ModifierType::MINUS_PERCENTAGE,"添加部落领地花费");
	registerModifier("settle_cost",ModifierType::MINUS_PERCENTAGE,"定居花费");
	registerModifier("monthly_fervor_increase",ModifierType::NORMAL,"月度热情增长");
	registerModifier("monthly_piety",ModifierType::NORMAL,"每月正法"); 
	registerModifier("monthly_piety_accelerator",ModifierType::NORMAL,"每月正法增长");
	registerModifier("monthly_karma",ModifierType::NORMAL,"每月科琳典范值");
	registerModifier("monthly_karma_accelerator",ModifierType::NORMAL,"每月科琳典范值加速");
	registerModifier("global_rebel_suppression_efficiency",ModifierType::PERCENTAGE,"全局镇压叛乱花费");
	registerModifier("caravan_power",ModifierType::PERCENTAGE,"商队力量");
	registerModifier("privateer_efficiency",ModifierType::PERCENTAGE,"私掠效率");
	registerModifier("global_trade_goods_size_modifier",ModifierType::PERCENTAGE,"商品产出修正");
	registerModifier("global_trade_goods_size",ModifierType::NORMAL,"全局商品产出");

	registerModifier("imperial_authority",ModifierType::PERCENTAGE,"帝国权威修正");
	registerModifier("imperial_authority_value",ModifierType::NORMAL,"帝国权威");
	registerModifier("imperial_mandate",ModifierType::PERCENTAGE,"猫命增长修正");
	registerModifier("national_focus_years",ModifierType::MINUS,"切换国家焦点所需年数");
	registerModifier("vassal_forcelimit_bonus",ModifierType::PERCENTAGE,"附庸提供的陆军上限");
	registerModifier("vassal_naval_forcelimit_bonus",ModifierType::PERCENTAGE,"附庸提供的海军上限");
	registerModifier("vassal_manpower_bonus",ModifierType::PERCENTAGE,"附庸提供的人力");
	registerModifier("vassal_sailors_bonus",ModifierType::PERCENTAGE,"附庸提供的水手");
	registerModifier("years_of_nationalism",ModifierType::MINUS,"分离主义年数");
	registerModifier("local_years_of_nationalism",ModifierType::MINUS,"本地分离主义年数");
	registerModifier("num_accepted_cultures",ModifierType::NORMAL,"可接受文化数量");
	registerModifier("chance_to_inherit",ModifierType::PERCENTAGE,"继承联合统治属国概率");
	registerModifier("ship_durability",ModifierType::NORMAL,"船只耐久度");
	registerModifier("liberty_desire",ModifierType::PERCENTAGE,"独立倾向");
	registerModifier("reduce_liberty_desire",ModifierType::MINUS_PERCENTAGE,"属国独立倾向");
	registerModifier("allowed_num_of_buildings",ModifierType::NORMAL,"可用建筑位");
	registerModifier("global_allowed_num_of_buildings",ModifierType::NORMAL,"全局可用建筑位");
	registerModifier("allowed_num_of_manufactories",ModifierType::NORMAL,"可建工场数量");
	registerModifier("global_allowed_num_of_manufactories",ModifierType::NORMAL,"全局可建工场数量");
	registerModifier("church_power_modifier",ModifierType::PERCENTAGE,"宗教力量修正");
	registerModifier("monthly_church_power",ModifierType::NORMAL,"每月宗教力量");
	registerModifier("garrison_size",ModifierType::PERCENTAGE,"驻军规模");
	registerModifier("local_garrison_size",ModifierType::PERCENTAGE,"本地驻军规模");
	registerModifier("native_uprising_chance",ModifierType::MINUS_PERCENTAGE,"原住民暴动几率");
	registerModifier("native_assimilation",ModifierType::PERCENTAGE,"原住民同化");
	registerModifier("may_recruit_female_generals",ModifierType::NEUTRAL_MARK,"可以招募女性将领");
	registerModifier("block_introduce_heir",ModifierType::MINUS_MARK,"阻止引荐新的继承人");
	registerModifier("can_transfer_vassal_wargoal",ModifierType::MARK,"允许在和平条款中使用\"转移属国\"");
	registerModifier("can_chain_claim",ModifierType::MARK,"允许宣称已宣称省份相邻省份");
	registerModifier("free_maintenance_on_expl_conq",ModifierType::MARK,"使用探险家或征服者不需要点数维护");
	registerModifier("colony_development_boost",ModifierType::NORMAL,"殖民地发展提升");
	registerModifier("attack_bonus_in_capital_terrain",ModifierType::NORMAL,"在地形与首都相同的省份战斗加成");
	registerModifier("can_bypass_forts",ModifierType::MARK,"可以无视要塞控制区");
	registerModifier("ignore_coring_distance",ModifierType::MARK,"无视核心化距离");
	registerModifier("force_march_free",ModifierType::MARK,"不花费点数强行军");
	registerModifier("possible_condottieri",ModifierType::NORMAL,"可用的遣外雇佣军团");
	registerModifier("global_ship_trade_power",ModifierType::PERCENTAGE,"舰船贸易竞争力");
	registerModifier("local_naval_engagement_modifier",ModifierType::PERCENTAGE,"本地海军接战修正");
	registerModifier("global_naval_engagement_modifier",ModifierType::PERCENTAGE,"全局海军接战修正");
	registerModifier("global_naval_engagement",ModifierType::NORMAL,"全局海军接战");
	registerModifier("cavalry_flanking",ModifierType::PERCENTAGE,"骑兵侧翼攻击能力");
	

	//For Estates Temp. 
	//Will be Removed when Estate Preload Reader Complete
	registerModifier("artificers_loyalty_modifier",ModifierType::PERCENTAGE,"奇械术士忠诚均衡点");
	registerModifier("church_loyalty_modifier",ModifierType::PERCENTAGE,"教士忠诚均衡点");

	//Anbennar Modifiers...
	registerModifier("artificers_research_time",ModifierType::MINUS_PERCENTAGE,"奇械发明研究时间"); 	
	registerModifier("artificers_invention_slots",ModifierType::NORMAL,"奇械发明空槽");
	registerModifier("artificers_capacity",ModifierType::NORMAL,"奇械术承载力");
	registerModifier("telescope_investment_efficency",ModifierType::PERCENTAGE,"天文中心投资效率");
	registerModifier("mages_mana_regen",ModifierType::NORMAL,"法力回复");
	registerModifier("mages_mana_regen_mult",ModifierType::PERCENTAGE,"法力回复修正");
	registerModifier("mages_mana_capacity",ModifierType::NORMAL,"法力容量");
	registerModifier("max_estate_spell_levels",ModifierType::NORMAL,"最大阶层法术等级");
	registerModifier("mages_monthly_experience",ModifierType::PERCENTAGE,"每月魔法学习经验");
	registerModifier("mages_ruler_experience_mod",ModifierType::PERCENTAGE,"统治者魔法学习经验加成");
	registerModifier("mages_estate_experience_mod",ModifierType::PERCENTAGE,"阶层魔法学习经验加成");
	registerModifier("abjuration_experience_mod",ModifierType::PERCENTAGE,"防护系魔法学习经验加成");
	registerModifier("conjuration_experience_mod",ModifierType::PERCENTAGE,"咒法系魔法学习经验加成");
	registerModifier("divination_experience_mod",ModifierType::PERCENTAGE,"预言系魔法学习经验加成");
	registerModifier("enchantment_experience_mod",ModifierType::PERCENTAGE,"惑控系魔法学习经验加成");
	registerModifier("evocation_experience_mod",ModifierType::PERCENTAGE,"召唤系魔法学习经验加成");
	registerModifier("illusion_experience_mod",ModifierType::PERCENTAGE,"幻术系魔法学习经验加成");
	registerModifier("necromancy_experience_mod",ModifierType::PERCENTAGE,"死灵系魔法学习经验加成");
	registerModifier("transmutation_experience_mod",ModifierType::PERCENTAGE,"变形系魔法学习经验加成");
	registerModifier("monarch_spell_levels",ModifierType::NORMAL,"统治者起始法术等级");


	
}


void ParseModifier(ParadoxTag* tag,std::vector<Modifier>& modifiers){
	for(int i = 0;i < tag->seq.size();i++){
		ParadoxTag* modifierTag = tag->getAsTag(i);
		if(modifierTag == nullptr) continue;
 		Modifier modifier;
		std::string item = stripTag(tag->seq[i]);
				
		modifier.name = item;
		ParserModifier(modifierTag,modifier);
		modifiers.push_back(modifier);
	}
}
void ParserModifier(ParadoxTag* tag,Modifier& modifier){
	for(int i = 0;i < tag->seq.size();i++){
		ModifierItem mod_item;
		std::string item = stripTag(tag->seq[i]);
		auto it = modifierObjects.find(item);
		if(it == modifierObjects.end()) continue;
		ModifierObject obj = it->second;
		bool isMark = obj.type == ModifierType::MARK || obj.type == ModifierType::MINUS_MARK;
		ParadoxBase* base = tag->get(i);
		mod_item.modifierName = item;
		if(base->getType() == ParadoxType::INTEGER && !isMark){
			mod_item.value = base->getAsInteger()->getIntegerContent();
			modifier.items.push_back(mod_item);
		}
		else if(base->getType() == ParadoxType::STRING && isMark){
			mod_item.value = base->getAsString()->getStringContent() == "no" ? 0 : 1;
			modifier.items.push_back(mod_item);
		}
	}
}
std::string Modifier::localize(){
	std::string localized = "";
	localized.append(this->name);
	localized.append(":\r\n");
	for(int i = 0;i < items.size();i++){
		ModifierObject obj = modifierObjects[items[i].modifierName];
		int type_id = (int)obj.type; 
		if(type_id >= 6 && type_id <= 8){
			localized.append(obj.localizedName);
			if(items[i].value == 0){
				localized.append(":否");	
			}
			else localized.append(":是");	
		}
		else if (type_id >= 3 && type_id <= 5){
			char buffer[16];
			generatePercentage(items[i].value,buffer);
			localized.append(buffer);
			localized.append(obj.localizedName);
		}
		else if(type_id >= 0 && type_id <= 2){
			char buffer[16];
			generateValue(items[i].value,buffer);
			localized.append(buffer);
			localized.append(obj.localizedName);
		}
		else{
			char buffer[16];
			generatePercentage2(items[i].value,buffer);
			localized.append(buffer);
			localized.append(obj.localizedName);
		}
		localized.append("\r\n");
	}
	return localized;
}

std::string Modifier::localizeHtml(){
	std::string localized = "<div class=\"effect_title\">";
	localized.append(this->name);
	localized.append(":</div>\r\n");
	for(int i = 0;i < items.size();i++){
		ModifierObject obj = modifierObjects[items[i].modifierName];
		localized.append("<div>");
		int type_id = (int)obj.type; 
		if(type_id >= 6 && type_id <= 8){
			localized.append(obj.localizedName);
			localized.append(":");
			std::string class_span = Xor(obj.type == MINUS_MARK,items[i].value== 0) ? "modifier_negative" : "modifier_positive";
			if(obj.type == NEUTRAL_MARK) class_span = "modifier_neutral"; 
			localized.append("<span class=\"");
			localized.append(class_span);
			if(items[i].value == 0){
				
				
				localized.append("\">否 </span>"); 
			}
			else localized.append("\">是 </span>");	
		}
		else if (type_id >= 3 && type_id <= 5){
			char buffer[16];
			std::string class_span = Xor(obj.type == MINUS_PERCENTAGE,items[i].value < 0) ? "modifier_negative" : "modifier_positive";
			if(obj.type == NEUTRAL_PERCENTAGE) class_span = "modifier_neutral";  
			localized.append("<span class=\"");
			localized.append(class_span);
			localized.append("\">");
			generatePercentage(items[i].value,buffer);
			
			localized.append(buffer);
			localized.append("</span>");
			localized.append(obj.localizedName);
		}
		else if (type_id >= 0 && type_id <= 2){
			char buffer[16];
			std::string class_span = Xor(obj.type == MINUS,items[i].value < 0) ? "modifier_negative" : "modifier_positive";
			if(obj.type == NEUTRAL) class_span = "modifier_neutral"; 
			localized.append("<span class=\"");
			localized.append(class_span);
			localized.append("\">");
			generateValue(items[i].value,buffer);
			
			localized.append(buffer);
			localized.append("</span>");
			localized.append(obj.localizedName);
		}
		else{
			char buffer[16];
			std::string class_span = Xor(obj.type == MINUS_PERCENTAGE2,items[i].value < 0) ? "modifier_negative" : "modifier_positive";
			if(obj.type == NEUTRAL_PERCENTAGE2) class_span = "modifier_neutral"; 
			localized.append("<span class=\"");
			localized.append(class_span);
			localized.append("\">");
			generatePercentage2(items[i].value,buffer);
			
			localized.append(buffer);
			localized.append("</span>");
			localized.append(obj.localizedName);
		}
		localized.append("</div>\r\n");
	}
	return localized;	
}
