#pragma once
#include <iostream>
#include <vector>

#include "creature.h"

enum class BattleStartStatus {
	kBattleStartStatusMin,
	kAttaking = kBattleStartStatusMin,
	kDefending,
	kBattleStartStatusMax
};

class Army {
public:
	static constexpr int army_size_max_{ 14 }; // changed from 18 to 14 for laptop

public:

	Army() {
		army_.reserve(army_size_max_);

		static int army_id_counter_{};
		army_id_ = army_id_counter_++;
	}
	~Army() = default;

	int get_army_id() const { return army_id_; }
	size_t get_army_size() const { return army_.size(); }
	std::vector<Creature>* get_army_ptr() { return &army_; }

	void kill_creature(size_t creature_id);

	void generate_random_army();
	void clear();
private:

private:
	int army_id_;
	std::vector<Creature> army_;
};