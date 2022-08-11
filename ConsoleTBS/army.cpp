
#include "army.h"

#include <vector>
#include <algorithm>
#include <execution>
#include <memory>
#include <limits>

#include "file_database.h"
#include "random.h"
#include "creature.h"

#include <chrono>

const int army_size_max_{ 14 }; // changed from 18 to 14 for laptop
int int_max_value{ std::numeric_limits<int>::max() };

Army::Army() {
	army_ = std::make_shared<std::vector<std::shared_ptr<Creature>>>();
	army_->reserve(army_size_max_);

	static int army_id_counter_{};
	army_id_ = army_id_counter_++;
}

short Army::get_army_id() const { return army_id_; }
int Army::get_army_size() const { return static_cast<int>(army_->size()); }
//std::weak_ptr<std::vector<std::shared_ptr<Creature>>> Army::get_army_weak_ptr() { return army_; }
std::shared_ptr<std::vector<std::shared_ptr<Creature>>> Army::get_army_shared_ptr() { return army_; }

void Army::clear() { army_->clear(); }

std::vector<std::shared_ptr<Creature>>::iterator Army::begin() { return army_->begin(); }
std::vector<std::shared_ptr<Creature>>::iterator Army::end() { return army_->end(); }

CreatureComplexID Army::generate_creature_complex_id() {
	if (creature_id_counter_ >= int_max_value) {
		// reassign creature_id_'s from start (free unused id's and resort)
	}
	return{ army_id_, creature_id_counter_ };
}

Army generate_random_army() {
	Army army{};

	load_creature_main_database(FileDatabaseId::kCreatureNameDatabase);
	load_creature_main_database(FileDatabaseId::kCreatureTemplateDatabase);

	std::generate_n(std::back_inserter(*army.army_), army_size_max_, [&army]() { return std::make_shared<Creature>(creature_database_get_templates(static_cast<CreatureTemplate>((
		get_random_number(static_cast<int>(CreatureTemplate::kHumanSpearman), static_cast<int>(CreatureTemplate::kCreatureTemplateMax) - 1)))), army.generate_creature_complex_id());});

	unload_creature_main_database(FileDatabaseId::kCreatureNameDatabase);
	unload_creature_main_database(FileDatabaseId::kCreatureTemplateDatabase);

	return army;
}

void Army::kill_creature(size_t dead_creature_id){
	auto dead_creature_iter = std::find_if(std::execution::par_unseq, army_->begin(), army_->end(), [dead_creature_id](std::shared_ptr<Creature> creature) { return creature->get_creature_id() == dead_creature_id; }); // there are unique creature_id_ for every creature, so there can be par_unsec execution
	if (dead_creature_iter != army_->end()) {
		army_->erase(dead_creature_iter);
	}
}