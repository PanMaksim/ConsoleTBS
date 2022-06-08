#include "creature_actions.h"

#include "files_initialization.h"
#include "creature.h"
#include "creature_stats.h"
#include "rolls.h"

//const std::string creature_database_ability_result_naming[static_cast<int>(AbilityResult::kAbilityResultMax)]{
//	"Got Dead",
//	"Critical Fail"
//	"Fail",
//	"No Result",
//	"Success",
//	"Critical Success"
//	"Killed Target",
//};
//
//const std::string* creature_database_get_ability_result_naming(AbilityResult ability_result) {
//	return &creature_database_ability_result_naming[static_cast<int>(ability_result)];
//}

void log_creature_action_result(const CreatureActionResult* action_result, const Creature* action_dealer, const Creature* action_target) {
	game_ptr->add_string_to_ui_log("Ability Result: " + *creature_database_get_roll_result_naming(action_result->ability_result));

	if (action_result->ability_result >= RollResult::kSmallSuccess) {
		game_ptr->add_string_to_ui_log(*action_target->get_name() + " received " + std::to_string(action_result->damage_dealed) + " damage.");
	}
	else if (action_result->ability_result <= RollResult::kSmallFail) {
		game_ptr->add_string_to_ui_log(*action_dealer->get_name() + " received " + std::to_string(action_result->damage_received) + " damage.");
	}
}

CreatureActionResult creature_ability_default_attack(Creature* attacker, Creature* defender) {
	RollResult roll_result{ compare_rolls(attacker, CreatureStatId::kATK_ML, defender, CreatureStatId::kDEF_ML) };

	int damage_dealed{ },
		damage_received{ }; // received from counterattack

	if (roll_result == RollResult::kNoResult) {
		return CreatureActionResult{ RollResult::kNoResult, 0, 0 };
	}
	else if (roll_result >= RollResult::kSmallSuccess) {
		damage_dealed = defender->calculate_received_damage(attacker, creature_database_get_roll_result_multiplier(roll_result));
	}
	else if (roll_result <= RollResult::kSmallFail){
		damage_received = attacker->calculate_received_damage(defender, creature_database_get_roll_result_multiplier(
				static_cast<RollResult>(static_cast<int>(RollResult::kCriticalSuccess) - static_cast<int>(roll_result))));
	}

	// those checks not in same if check with calculate damage because of possible received damage even when attacked without receiving counterattack
	// check to prevent healing when damage value debuffed to be lower than 0
	if (damage_dealed > 0) {
		defender->change_certain_stat_current_value(CreatureStatId::kHP, (-1) * damage_dealed);
	}
	if (damage_received > 0) {
		attacker->change_certain_stat_current_value(CreatureStatId::kHP, (-1) * damage_received);
	}

	return {roll_result, damage_dealed, damage_received};
}