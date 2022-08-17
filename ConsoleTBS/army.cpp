
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

using namespace creature;

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

creature::stat::ComplexID Army::generate_creature_complex_id() {
	if (++creature_id_counter_ == int_max_value) {
		// reassign creature_id_'s from start (free unused id's)
		creature_id_counter_ = 0;
		std::for_each(army_->begin(), army_->end(), [&](std::shared_ptr<Creature> creature) { 
			creature->receive_new_creature_id(creature_id_counter_); });
		// not ending, because return value used to create new creature after reassigning id's
	}
	return{ army_id_, creature_id_counter_ };
}

Army generate_random_army() {
	
	using namespace file_database;

	Army army{};

	creature::load_database_into_memory(ID::kCreatureNameDatabase);
	creature::load_database_into_memory(ID::kCreatureTemplateDatabase);

	std::generate_n(std::back_inserter(*army.army_), army_size_max_, [&army]() { return std::make_shared<Creature>(creature::get_ptr_to_creature_template_from_database(static_cast<creature::CreatureTemplateID>((
		random::get_random_number(static_cast<int>(creature::CreatureTemplateID::kHumanSpearman), static_cast<int>(creature::CreatureTemplateID::kCreatureTemplateMax) - 1)))), army.generate_creature_complex_id());});

	creature::unload_database_from_memory(ID::kCreatureNameDatabase);
	creature::unload_database_from_memory(ID::kCreatureTemplateDatabase);

	return army;
}

void Army::kill_creature(size_t dead_creature_id){
	auto dead_creature_iter = std::find_if(std::execution::par_unseq, army_->begin(), army_->end(), [dead_creature_id](std::shared_ptr<Creature> creature) { return creature->get_creature_id() == dead_creature_id; }); // there are unique creature_id_ for every creature, so there can be par_unsec execution
	if (dead_creature_iter != army_->end()) {
		army_->erase(dead_creature_iter);
	}
}