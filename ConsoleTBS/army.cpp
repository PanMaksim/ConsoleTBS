
#include "army.h"

#include <vector>
#include <algorithm>
#include <execution>
#include <memory>

#include "file_database.h"
#include "random.h"
#include "creature.h"

#include <chrono>

const int army_size_max_{ 14 }; // changed from 18 to 14 for laptop

Army::Army() {
	army_ = std::make_shared<std::vector<Creature>>();
	army_->reserve(army_size_max_);

	static int army_id_counter_{};
	army_id_ = army_id_counter_++;
}

int Army::get_army_id() const { return army_id_; }
size_t Army::get_army_size() const { return army_->size(); }
//std::weak_ptr<std::vector<Creature>> Army::get_army_weak_ptr() { return army_; }
std::shared_ptr<std::vector<Creature>> Army::get_army_shared_ptr() { return army_; }

void Army::clear() { army_->clear(); }

std::vector<Creature>::iterator Army::begin() { return army_->begin(); }
std::vector<Creature>::iterator Army::end() { return army_->end(); }

Army generate_random_army() {
	Army army{};

	load_creature_main_database(FileDatabaseId::kCreatureNameDatabase);
	load_creature_main_database(FileDatabaseId::kCreatureTemplateDatabase);

	int army_id{ army.get_army_id() };

	std::generate_n(std::back_inserter(*army.army_), army_size_max_, []() { return creature_database_get_templates(static_cast<CreatureTemplate>((
		get_random_number(static_cast<int>(CreatureTemplate::kHumanSpearman), static_cast<int>(CreatureTemplate::kCreatureTemplateMax) - 1))));});

	std::for_each(std::execution::par_unseq, army.army_->begin(), army.army_->end(), [=](Creature& creature) {creature.join_army(army_id);});

	unload_creature_main_database(FileDatabaseId::kCreatureNameDatabase);
	unload_creature_main_database(FileDatabaseId::kCreatureTemplateDatabase);

	return army;
}

void Army::kill_creature(size_t dead_creature_id){
	auto dead_creature_iter = std::find_if(std::execution::par_unseq, army_->begin(), army_->end(), [dead_creature_id](const Creature& creature) { return creature.get_creature_id() == dead_creature_id; }); // there are unique creature_id for every creature, so there can be par_unsec execution
	if (dead_creature_iter != army_->end()) {
		army_->erase(dead_creature_iter);
	}
}