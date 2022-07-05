
#include "army.h"

#include <vector>
#include <algorithm>

#include "random.h"
#include "creature.h"

void Army::clear() {
	army_.clear();
}

void Army::generate_random_army() { // for testing
	if (army_.size() != 0) {
		this->clear();
	}

	std::generate_n(std::back_inserter(army_), army_size_max_, []() { return creature_database_get_templates(static_cast<CreatureTemplate>((
		get_random_number(static_cast<int>(CreatureTemplate::kCreatureTemplateMin), static_cast<int>(CreatureTemplate::kCreatureTemplateMax) - 1))));});

	std::for_each(army_.begin(), army_.end(), [=](Creature& creature) {creature.join_army(army_id_);});
}

void Army::kill_creature(size_t dead_creature_id){
	auto dead_creature_iter = std::find_if(army_.begin(), army_.end(), [dead_creature_id](const Creature& creature) { return creature.get_creature_id() == dead_creature_id; });
	if (dead_creature_iter != army_.end()) {
		army_.erase(dead_creature_iter);
	}
}