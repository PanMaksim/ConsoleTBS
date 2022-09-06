
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
using namespace army;

const int army::army_size_max_{ 12 }; // 18 for PC and 14 for laptop

army::Army::Army() {
	army_ = std::make_shared<std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>>(compare_shared_ptrs_to_creature);

	static int army_id_counter_{};
	army_id_ = army_id_counter_++;
}

short army::Army::get_army_id() const { return army_id_; }
int army::Army::get_army_size() const { return static_cast<int>(army_->size()); }
//std::weak_ptr<std::vector<std::shared_ptr<Creature>>> Army::get_army_weak_ptr() { return army_; }
std::shared_ptr<std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>> Army::get_army_shared_ptr() { return army_; }

void army::Army::clear() { army_->clear(); }

std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>::iterator army::Army::begin() { return army_->begin(); }
std::set<std::shared_ptr<creature::Creature>, decltype(creature::compare_shared_ptrs_to_creature)>::iterator army::Army::end() { return army_->end(); }

creature::stat::ComplexID army::Army::generate_creature_complex_id() {
	if (++creature_id_counter_ == std::numeric_limits<int>::max()) {
		// reassign creature_id_'s from start (free unused id's)
		creature_id_counter_ = 0;
		std::for_each(army_->begin(), army_->end(), [&](std::shared_ptr<Creature> creature) { 
			creature->receive_new_creature_id(creature_id_counter_); });
		// not ending, because return value used to create new creature after reassigning id's
	}
	return{ army_id_, creature_id_counter_ };
}

army::Army army::generate_random_army() {
	using namespace file_database;

	Army army{};

	creature::load_database_into_memory(ID::kCreatureNameDatabase);
	creature::load_database_into_memory(ID::kCreatureTemplateDatabase);

	std::generate_n(std::inserter(*army.army_, army.army_->begin()), army_size_max_, [&army]() { return std::make_shared<Creature>(creature::get_ptr_to_creature_template_from_database(static_cast<creature::CreatureTemplateID>((
		random::get_random_number(static_cast<int>(creature::CreatureTemplateID::kHumanSpearman), static_cast<int>(creature::CreatureTemplateID::kCreatureTemplateMax) - 1)))), army.generate_creature_complex_id());});

	creature::unload_database_from_memory(ID::kCreatureNameDatabase);
	creature::unload_database_from_memory(ID::kCreatureTemplateDatabase);

	return army;
}

void army::Army::kill_creature(size_t dead_creature_id){
	auto dead_creature_iter = std::find_if(std::execution::par_unseq, army_->begin(), army_->end(), [dead_creature_id](std::shared_ptr<Creature> creature) { return creature->get_creature_id() == dead_creature_id; }); // there are unique creature_id_ for every creature, so there can be par_unsec execution
	if (dead_creature_iter != army_->end()) {
		army_->erase(dead_creature_iter);
	}
}