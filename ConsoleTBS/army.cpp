
#include "army.h"

#include <vector>

#include "random.h"
#include "creature.h"


void Army::clear() {
	army_.clear();
}

// test

void Army::generate_random_army() { // for testing
	if (army_.size() != 0) {
		army_.clear();
	}
	army_.reserve(army_size_max_);

	for (int army_size_current{}; army_size_current != army_size_max_; ++army_size_current) {
		army_.push_back(creature_database_get_templates(static_cast<CreatureTemplate>((
			get_random_number(static_cast<int>(CreatureTemplate::kCreatureTemplateMin), static_cast<int>(CreatureTemplate::kCreatureTemplateMax) - 1)))));
		army_.back().join_army(army_id_);
	}
}