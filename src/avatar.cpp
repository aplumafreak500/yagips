/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <time.h>
#include "util.h"
#include "player.h"
#include "dbgate.h"
#include "data.h"
#include "data/avatar_data.h"
#include "avatar.h"
#include "define.pb.h"

Avatar::Avatar() {
	bornTime = time(NULL);
	level = 1;
	exp = 0;
	friendship = 1;
	friendship_exp = 0;
	constellation = 0;
	ascension = 0;
	wings = 14001;
	talent_level[0] = 1;
	talent_level[1] = 1;
	talent_level[2] = 1;
	energy = 0;
}

Avatar::~Avatar() {}

Avatar::Avatar(unsigned int _id) {
	bornTime = time(NULL);
	const AvatarData* tbl = globalGameData->avatar_data;
	data = (*tbl)[_id];
	id = _id;
	skillDepotId = data->skill_depot_id;
	level = 1;
	exp = 0;
	friendship = 1;
	friendship_exp = 0;
	constellation = 0;
	ascension = 0;
	wings = 14001;
	talent_level[0] = 1;
	talent_level[1] = 1;
	talent_level[2] = 1;
	energy = 0;
	// TODO Weapon
	// TODO Costume
}

Avatar::Avatar(const proto::AvatarInfo& pb) {
	const AvatarData* tbl = globalGameData->avatar_data;
	id = pb.avatar_id();
	data = (*tbl)[id];
	guid = pb.guid();
	bornTime = pb.born_time();
	skillDepotId = pb.skill_depot_id();
	wings = pb.wearing_flycloak_id();
	friendship = pb.fetter_info().exp_level();
	if (friendship >= 10) {
		// TODO Pull from AvatarFettersLevelData instead
		friendship_exp = 6325;
	}
	else {
		friendship_exp = pb.fetter_info().exp_number();
	}
	// TODO Auxiliary friendship data
	// TODO Has obtained namecard
	// TODO Costume
	// TODO Artifacts/weapon
	// TODO Skill map
	// TODO Talent ids
	// TODO Skill level map (talent levels)
	// TODO Fight props
	// TODO Avatar type
	// TODO Props (including current/max hp, level, exp, ascension, and satiation
	// TODO Core proud skill level (?)
	// TODO Inherit proud skill list (?)
	// TODO Proud skill extra level map (?)
}

Avatar::operator proto::AvatarInfo() const {
	proto::AvatarFetterInfo* fi = new proto::AvatarFetterInfo;
	fi->set_exp_level(friendship);
	if (friendship < 10) {
		fi->set_exp_number(friendship_exp);
	}
	// TODO Auxiliary friendship data
	// TODO Has obtained namecard
	proto::AvatarInfo pb;
	pb.set_avatar_id(id);
	pb.set_guid(guid);
	pb.set_born_time(bornTime);
	pb.set_skill_depot_id(skillDepotId);
	pb.set_allocated_fetter_info(fi);
	pb.set_wearing_flycloak_id(wings);
	// TODO Costume
	// TODO Check current hp first
	pb.set_life_state(1);
	// TODO Artifacts/weapon
	// TODO Skill map
	// TODO Talent ids
	// TODO Skill level map (talent levels)
	// TODO Fight props
	// TODO Avatar type
	// TODO Props (including current/max hp, level, exp, ascension, and satiation
	// TODO Core proud skill level (?)
	// TODO Inherit proud skill list (?)
	// TODO Proud skill extra level map (?)
	return pb;
}

int Avatar::loadFromDb() {
	return loadFromDb(guid);
}

int Avatar::loadFromDb(unsigned long long _guid) {
	Avatar* _new = globalDbGate->getAvatarByGuid(_guid);
	if (_new == NULL) return -1;
	*this = *_new;
	delete _new;
	return 0;
}

int Avatar::saveToDb() const {
	return globalDbGate->saveAvatar(*this);
}

unsigned int Avatar::getId() const {
	return id;
}

void Avatar::setId(unsigned int i) {
	setId(i, 0);
}

void Avatar::setId(unsigned int i, unsigned int recalc) {
	id = i;
	if (recalc) {
		const AvatarData* tbl = globalGameData->avatar_data;
		data = (*tbl)[i];
		skillDepotId = data->skill_depot_id;
		// TODO redo stats/props
	}
}

const AvatarDataEnt* Avatar::getData() const {
	return data;
}

void Avatar::setData(unsigned int i) {
	const AvatarData* tbl = globalGameData->avatar_data;
	data = (*tbl)[i];
}

void Avatar::setData(const AvatarDataEnt* d) {
	data = d;
}

unsigned long long Avatar::getGuid() const {
	return guid;
}

unsigned int Avatar::getUid() const {
	return guid >> 32;
}

const Player* Avatar::getOwner() const {
	if (!(guid >> 32)) return NULL;
	return globalDbGate->getPlayerByUid(guid >> 32);
}

void Avatar::setGuid() { /* TODO
	if (!guid >> 32) return;
	Player* owner = globalDbGate->getPlayerByUid(guid >> 32);
	if (owner == NULL) return;
	guid = owner->getNewGuid(); */
}

void Avatar::setGuid(unsigned long long g) {
	guid = g;
}

void Avatar::setUid(unsigned int u) {
	guid &= 0xffffffff;
	guid |= ((unsigned long long) u) << 32;
}

void Avatar::setOwner(const Player& p) {
	setUid(p.getUid());
}

unsigned short Avatar::getWeaponId() const {
	return weapon->id;
}

const Item& Avatar::getWeapon() const {
	return *weapon;
}

Item* Avatar::getWeapon() {
	return weapon;
}

int Avatar::setWeapon(Item* w) {
	if (w->type != ITEM_WEAPON) return -1;
	// TODO Check if equipped on someone else
	weapon = w;
	return 0;
}

std::vector<unsigned short> Avatar::getArtifactIds() const {
	std::vector<unsigned short> ret;
	unsigned int i;
	for (i = 0; i < 5; i++) {
		if (artifacts[i] == NULL) ret.push_back(0);
		else ret.push_back(artifacts[i]->id);
	}
	return ret;
}

std::vector<const Item*> Avatar::getArtifacts() const {
	std::vector<const Item*> ret;
	unsigned int i;
	for (i = 0; i < 5; i++) {
		ret.push_back(artifacts[i]);
	}
	return ret;
}

std::vector<Item*> Avatar::getArtifacts() {
	std::vector<Item*> ret;
	unsigned int i;
	for (i = 0; i < 5; i++) {
		ret.push_back(artifacts[i]);
	}
	return ret;
}

unsigned short Avatar::getArtifactId(unsigned int i) const {
	if (artifacts[i] == NULL) return 0;
	return artifacts[i]->id;
}

Item* Avatar::getArtifact(unsigned int i) {
	return artifacts[i];
}

int Avatar::setArtifacts(Item* item[5]) {
	unsigned int i;
	for (i = 0; i < 5; i++) {
		if (item[i]->type != ITEM_ARTIFACT) return -1;
		// TODO Check if equipped on someone else
	}
	for (i = 0; i < 5; i++) {
		artifacts[i] = item[i];
	}
	return 0;
}

int Avatar::setArtifact(Item* item) {
	if (item->type != ITEM_ARTIFACT) return -1;
	unsigned int idx = ((item->id % 100) / 10) - 1;
	if (idx >= 5) return -1;
	// TODO Check if equipped on someone else
	artifacts[idx] = item;
	return 0;
}

int Avatar::setArtifact(Item* item, unsigned int idx) {
	if (idx >= 5) return -1;
	if (item->type != ITEM_ARTIFACT) return -1;
	// TODO Check if equipped on someone else
	artifacts[idx] = item;
	return 0;
}

unsigned int Avatar::getLevel() const {
	return level;
}

unsigned int Avatar::getLevel(unsigned int recalc) {
	if (recalc) {
		// TODO
	}
	return level;
}

void Avatar::setLevel(unsigned int l) {
	if (l < 1) l = 1;
	else if (l > 90) l = 90;
	level = l;
}

void Avatar::setLevel(unsigned int l, unsigned int setExp) {
	if (l < 1) l = 1;
	else if (l > 90) l = 90;
	level = l;
	if (setExp) {
		// TODO set exp to level
	}
}

unsigned int Avatar::getExp() const {
	return exp;
}

void Avatar::setExp(unsigned int e) {
	// TODO Check max exp for lv 90
	exp = e;
}

void Avatar::setExp(unsigned int e, unsigned int recalc) {
	// TODO Check max exp for lv 90
	exp = e;
	if (recalc) {
		// TODO
	}
}

unsigned int Avatar::addExp(int expToAdd) {
	exp += expToAdd;
	if ((int) exp < 0) exp = 0;
	// TODO Check max exp for lv 90
	// TODO Recalculate level
	return exp;
}

unsigned int Avatar::getAscension() const {
	return ascension;
}

void Avatar::setAscension(unsigned int a) {
	if (a > 6) a = 6;
	ascension = a;
}

int Avatar::ascend() {
	if (ascension >= 6) return -1;
	ascension++;
	return ascension;
}

unsigned int Avatar::getCurHp() const {
	return curHp;
}

void Avatar::setCurHp(unsigned int h) {
	// TODO Cap to max HP
	curHp = h;
}

unsigned int Avatar::addCurHp(int h) {
	curHp += h;
	if ((int) curHp < 0) curHp = 0;
	// TODO Cap to max HP
	return curHp;
}

unsigned int Avatar::getCostume() const {
	return costume;
}

int Avatar::setCostume(unsigned int c) {
	// TODO Clamp to valid costumes for avatar id
	costume = c;
	return 0;
}

unsigned int Avatar::getWings() const {
	return wings;
}

int Avatar::setWings(unsigned int w) {
	// TODO Clamp to valid ids for wings
	wings = w;
	return 0;
}

unsigned int Avatar::getFriendship() const {
	return friendship;
}

unsigned int Avatar::getFriendship(unsigned int recalc) {
	if (recalc) {
		// TODO
	}
	return friendship;
}

void Avatar::setFriendship(unsigned int f) {
	if (f < 1) f = 1;
	else if (f > 10) f = 10;
	friendship = f;
}

void Avatar::setFriendship(unsigned int f, unsigned int recalc) {
	if (f < 1) f = 1;
	else if (f > 10) f = 10;
	friendship = f;
	if (recalc) {
		// TODO set exp to level
	}
}

unsigned int Avatar::getFriendshipExp() const {
	return friendship_exp;
}

void Avatar::setFriendshipExp(unsigned int e) {
	// TODO Check max exp for level 10
	friendship_exp = e;
}
void Avatar::setFriendshipExp(unsigned int e, unsigned int recalc) {
	// TODO Check max exp for level 10
	friendship_exp = e;
	if (recalc) {
		// TODO recalculate level
	}
}

unsigned int Avatar::addFriendshipExp(int expToAdd) {
	friendship_exp += expToAdd;
	if ((int) friendship_exp < 0) exp = 0;
	// TODO Check max exp for lv 10
	// TODO Recalculate level
	return friendship_exp;
}

const unsigned int* Avatar::getTalentLevels() const {
	return talent_level;
}

unsigned int Avatar::getTalentLevel(unsigned int idx) const {
	if (idx >= 3) return 0;
	return talent_level[idx];
}

void Avatar::setTalentLevels(unsigned int t[3]) {
	unsigned int i;
	for (i = 0; i < 3; i++) {
		if (t[i] < 1) t[i] = 1;
		else if (t[i] > 15) t[i] = 15;
		talent_level[i] = t[i];
	}
}

int Avatar::setTalentLevel(unsigned int l, unsigned int i) {
	if (i >= 3) return -1;
	if (l < 1) l = 1;
	else if (l > 15) l = 15;
	talent_level[i] = l;
	return 0;
}

int Avatar::addTalentLevel(unsigned int i) {
	if (i >= 3) return -1;
	if (talent_level[i] + 1 >= 15) return -1;
	talent_level[i]++;
	return talent_level[i];
}

unsigned int Avatar::getConstellation() const {
	return constellation;
}

void Avatar::setConstellation(unsigned int c) {
	if (c > 6) c = 6;
	constellation = c;
}

int Avatar::addConstellation() {
	if (constellation >= 6) return -1;
	constellation++;
	return constellation;
}

unsigned long long Avatar::getBornTime() const {
	return bornTime;
}

void Avatar::setBornTime(unsigned long long t) {
	bornTime = t;
}

unsigned long long Avatar::newBornTime() {
	bornTime = time(NULL);
	return bornTime;
}

unsigned int Avatar::getSkillDepotId() const {
	return skillDepotId;
}

unsigned int Avatar::setSkillDepotId() {
	skillDepotId = data->skill_depot_id;
	return skillDepotId;
}

// Needs a return type due to above overload.
unsigned int Avatar::setSkillDepotId(unsigned int i) {
	skillDepotId = i;
	return skillDepotId;
}

unsigned int Avatar::getFullness() const {
	return full;
}

void Avatar::setFullness(unsigned int f) {
	full = f;
}

unsigned int Avatar::addFullness(int f) {
	full += f;
	// TODO cap to 10000?
	if ((int) full < 0) full = 0;
	return full;
}

unsigned int Avatar::getFullnessCd() const {
	return full_cd;
}

void Avatar::setFullnessCd(unsigned int f) {
	full_cd = f;
}

unsigned int Avatar::addFullnessCd(int f) {
	full_cd += f;
	// TODO cap to 30000?
	if ((int) full_cd < 0) full_cd = 0;
	return full_cd;
}

unsigned int Avatar::getEnergy() const {
	return energy;
}

void Avatar::setEnergy(unsigned int e) {
	energy = e;
}

unsigned int Avatar::addEnergy(int e) {
	energy += e;
	if ((int) energy < 0) energy = 0;
	return energy;
}
