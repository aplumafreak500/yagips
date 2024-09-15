/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef AVATAR_H
#define AVATAR_H
#include <string>
class Avatar;
#include "item.h"
#include "player.h"
#include "data/avatar_data.h"
#include "define.pb.h"

class Avatar {
public:
	Avatar();
	~Avatar();
	Avatar(unsigned int);
	Avatar(const proto::AvatarInfo&);
	operator proto::AvatarInfo() const;
	int loadFromDb();
	int loadFromDb(unsigned long long);
	int saveToDb() const;
	unsigned int getId() const;
	void setId(unsigned int);
	void setId(unsigned int, unsigned int);
	const AvatarDataEnt* getData() const;
	void setData(unsigned int);
	void setData(const AvatarDataEnt*);
	unsigned long long getGuid() const;
	unsigned int getUid() const;
	const Player* getOwner() const;
	void setGuid();
	void setGuid(unsigned long long);
	void setUid(unsigned int);
	void setOwner(const Player&);
	unsigned short getWeaponId() const;
	const Item& getWeapon() const;
	Item* getWeapon();
	int setWeapon(Item*);
	std::vector<unsigned short> getArtifactIds() const;
	std::vector<const Item*> getArtifacts() const;
	std::vector<Item*> getArtifacts();
	unsigned short getArtifactId(unsigned int) const;
	const Item& getArtifact(unsigned int) const;
	Item* getArtifact(unsigned int);
	int setArtifacts(Item*[5]);
	int setArtifact(Item*);
	int setArtifact(Item*, unsigned int);
	unsigned int getLevel() const;
	unsigned int getLevel(unsigned int);
	void setLevel(unsigned int);
	void setLevel(unsigned int, unsigned int);
	unsigned int getExp() const;
	void setExp(unsigned int);
	void setExp(unsigned int, unsigned int);
	unsigned int addExp(int);
	unsigned int getAscension() const;
	void setAscension(unsigned int);
	int ascend();
	unsigned int getCurHp() const;
	void setCurHp(unsigned int);
	unsigned int addCurHp(int);
	unsigned int getCostume() const;
	int setCostume(unsigned int);
	unsigned int getWings() const;
	int setWings(unsigned int);
	unsigned int getFriendship() const;
	unsigned int getFriendship(unsigned int);
	void setFriendship(unsigned int);
	void setFriendship(unsigned int, unsigned int);
	unsigned int getFriendshipExp() const;
	void setFriendshipExp(unsigned int);
	void setFriendshipExp(unsigned int, unsigned int);
	unsigned int addFriendshipExp(int);
	const unsigned int* getTalentLevels() const;
	unsigned int getTalentLevel(unsigned int) const;
	void setTalentLevels(unsigned int[3]);
	int setTalentLevel(unsigned int, unsigned int);
	int addTalentLevel(unsigned int);
	unsigned int getConstellation() const;
	void setConstellation(unsigned int);
	int addConstellation();
	unsigned long long getBornTime() const;
	void setBornTime(unsigned long long);
	unsigned long long newBornTime();
	unsigned int getSkillDepotId() const;
	unsigned int setSkillDepotId();
	unsigned int setSkillDepotId(unsigned int);
	unsigned int getFullness() const;
	void setFullness(unsigned int);
	unsigned int addFullness(int);
	unsigned int getFullnessCd() const;
	void setFullnessCd(unsigned int);
	unsigned int addFullnessCd(int);
	unsigned int getEnergy() const;
	void setEnergy(unsigned int);
	unsigned int addEnergy(int);
private:
	unsigned int id;
	unsigned long long guid;
	Item* weapon;
	Item* artifacts[5];
	unsigned int level;
	unsigned int exp;
	unsigned int ascension;
	unsigned int curHp;
	unsigned int costume;
	unsigned int wings;
	unsigned int friendship;
	unsigned int friendship_exp;
	unsigned int talent_level[3]; // normal, skill, and burst
	unsigned int constellation;
	unsigned long long bornTime;
	unsigned int skillDepotId;
	unsigned int full;
	unsigned int full_cd;
	unsigned int energy; // TODO make type a float?
	const AvatarDataEnt* data;
	// TODO Skill map
	// TODO Talent ids
	// TODO Fight props
	// TODO Avatar type
	// TODO Props
	// TODO Core proud skill level (?)
	// TODO Inherit proud skill list (?)
	// TODO Proud skill extra level map (?)
	// TODO anything else?
};
#endif
