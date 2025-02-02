/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <time.h>
#include "account.h"
#include "dbgate.h"
#include "player.h"
#include "gameserver.h"
#include "session.h"
#include "kcpsession.h"
#include "packet.h"
#include "vector.h"
#include "util.h"
#include "avatar.h"
#include "data.h"
#include "data/openstate_data.h"
#include "enum/openstate.h"
#include "prop.h"
#include "player.pb.h"
#include "avatar.pb.h"
#include "scene.pb.h"
#include "define.pb.h"
#include "storage.pb.h"

Player::Player() {
	account = NULL;
	session = NULL;
	tpToken = 1;
	nextGuid = 1;
	ar = 1;
	ar_exp = 0;
	worldLevel = 0;
	// TODO Hardcoded until proper handling for scene data is implemented
	pos.x = 2747.562;
	pos.y = 194.633;
	pos.z = -1719.386;
	scene_id = 3;
	props[PROP_PLAYER_LEVEL] = ar;
	props[PROP_PLAYER_EXP] = ar_exp;
	props[PROP_PLAYER_WORLD_LEVEL] = worldLevel;
	props[PROP_PLAYER_HCOIN] = 0;
	props[PROP_PLAYER_SCOIN] = 0;
	props[PROP_PLAYER_MCOIN] = 0;
	props[PROP_PLAYER_RESIN] = 160;
	props[PROP_PLAYER_LEGENDARY_KEY] = 0;
	props[PROP_PLAYER_LEGENDARY_DAILY_TASK_NUM] = 0;
	// TODO any other props that need explicit initalizing?
	updateOpenstates();
}

Player::Player(const storage::PlayerInfo& p) {
	uid = p.uid();
	account = globalDbGate->getAccountByAid(p.aid());
	long long ctime = time(NULL);
	session = NULL;
	tpToken = 1;
	if (ctime > p.session_expire_ts()) {
		session = getSessionById(p.session_id());
	}
	scene_id = p.scene_id();
	worldLevel = p.world_level();
	ar = p.adventure_rank();
	ar_exp = p.adventure_exp();
	name = p.name();
	signature = p.signature();
	namecard = p.namecard();
	pfp = p.pfp();
	pos = p.position();
	nextGuid = p.next_guid();
	for (auto i = p.props().cbegin(); i != p.props().cend(); i++) {
		props[i->first] = i->second;
	}
	props[PROP_PLAYER_LEVEL] = ar;
	props[PROP_PLAYER_EXP] = ar_exp;
	props[PROP_PLAYER_WORLD_LEVEL] = worldLevel;
	for (int j = 0; j < p.open_states_size(); j++) {
		openstates.push_back(p.open_states(j));
	}
	updateOpenstates();
}

Player::~Player() {
	fprintf(stderr, "Warning: Call saveToDb before destructor\n");
	saveToDb();
}

Player::operator storage::PlayerInfo() const {
	storage::PlayerInfo ret;
	ret.set_uid(uid);
	long long ctime;
	if (account != NULL) {
		ret.set_aid(account->getAccountId());
	}
	if (session != NULL) {
		const KcpSession* kcp = session->getKcpSession();
		if (kcp != NULL) {
			ret.set_session_id(kcp->getSessionId());
			ret.set_session_seed(session->getSessionSeed());
			ctime = time(NULL) + (3 * 60 * 60);
			ret.set_session_expire_ts(ctime);
		}
	}
	ret.set_next_guid(nextGuid);
	ret.set_name(name);
	ret.set_signature(signature);
	ret.set_pfp(pfp);
	ret.set_namecard(namecard);
	ret.set_adventure_rank(ar);
	ret.set_adventure_exp(ar_exp);
	ret.set_world_level(worldLevel);
	proto::Vector* _pos = new proto::Vector();
	*_pos = pos;
	ret.set_allocated_position(_pos);
	ret.set_scene_id(scene_id);
	auto* m = ret.mutable_props();
	for (auto i = props.cbegin(); i != props.cend(); i++) {
		switch(i->first) {
		default:
			(*m)[i->first] = i->second;
			break;
		case PROP_PLAYER_LEVEL:
			(*m)[i->first] = ar;
			break;
		case PROP_PLAYER_EXP:
			(*m)[i->first] = ar_exp;
			break;
		case PROP_PLAYER_WORLD_LEVEL:
			(*m)[i->first] = worldLevel;
			break;
		}
	}
	for (auto j = openstates.cbegin(); j != openstates.cend(); j++) {
		ret.add_open_states(*j);
	}
	return ret;
}

int Player::loadFromDb() {
	return loadFromDb(uid);
}

int Player::loadFromDb(unsigned int _uid) {
	Player* _new = globalDbGate->getPlayerByUid(_uid);
	if (_new == NULL) return -1;
	*this = *_new;
	delete _new;
	return 0;
}

int Player::saveToDb() const {
	return globalDbGate->savePlayer(*this);
}

int Player::loadInventoryAndAvatars() {
	storage::InventoryEntry* ent = NULL;
	for (unsigned long long i = 0; i < nextGuid; i++) {
		ent = globalDbGate->getInventoryEntry(((unsigned long long) uid << 32) | i);
		if (ent == NULL) continue;
		if (ent->has_avatar()) addAvatar(ent->avatar());
		// TODO else if (ent->has_item()) ;
	}
	return 0;
}

const Account* Player::getAccount() const {
	return account;
}

void Player::setAccount(const Account* a) {
	account = a;
}

unsigned int Player::getUid() const {
	return uid;
}

void Player::setUid(unsigned int u) {
	uid = u;
}

unsigned int Player::getTpToken() const {
	return tpToken;
}

void Player::setTpToken(unsigned int t) {
	tpToken = t;
}

const Avatar* Player::getAvatarById(unsigned int id) const {
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if (id == (*i).getId()) return &(*i);
	}
	return NULL;
}

Avatar* Player::getAvatarById(unsigned int id) {
	for (auto i = avatars.begin(); i != avatars.end(); i++) {
		if (id == (*i).getId()) return &(*i);
	}
	return NULL;
}

const Avatar* Player::getAvatarByGuid(unsigned long long guid) const {
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if (guid == (*i).getGuid()) return &(*i);
	}
	return NULL;
}

Avatar* Player::getAvatarByGuid(unsigned long long guid) {
	for (auto i = avatars.begin(); i != avatars.end(); i++) {
		if (guid == (*i).getGuid()) return &(*i);
	}
	return NULL;
}

int Player::setAvatar(unsigned long long guid, Avatar& a) {
	a.setGuid(guid);
	for (auto i = avatars.begin(); i != avatars.end(); i++) {
		if (guid == (*i).getGuid()) {
			*i = a;
			return 0;
		}
	}
	avatars.push_back(a);
	return 0;
}

int Player::setAvatar(unsigned long long guid, const Avatar& a) {
	if (a.getGuid() != guid) return -1;
	for (auto i = avatars.begin(); i != avatars.end(); i++) {
		if (guid == (*i).getGuid()) {
			*i = a;
			return 0;
		}
	}
	avatars.push_back(a);
	return 0;
}

int Player::updateAvatar(const Avatar& a) {
	for (auto i = avatars.begin(); i != avatars.end(); i++) {
		if (a.getGuid() == (*i).getGuid()) {
			*i = a;
			return 0;
		}
	}
	return -1;
}

int Player::addAvatar(Avatar* a) {
	if (a == NULL) return -1;
	if (!(a->getGuid() & 0xffffffff)) {
		a->setGuid(nextGuid);
		nextGuid++;
	}
	if (!a->getUid()) a->setUid(uid);
	avatars.push_back(*a);
	return 0;
}

int Player::addAvatar(const Avatar* a) {
	if (a == NULL) return -1;
	avatars.push_back(*a);
	return 0;
}

int Player::addAvatar(Avatar& a) {
	if (!(a.getGuid() & 0xffffffff)) {
		a.setGuid(nextGuid);
		nextGuid++;
	}
	if (!a.getUid()) a.setUid(uid);
	avatars.push_back(a);
	return 0;
}

int Player::addAvatar(const Avatar& a) {
	avatars.push_back(a);
	return 0;
}

int Player::addAvatar(Avatar** a) {
	if (a == NULL) return -1;
	Avatar n;
	n.setGuid(nextGuid);
	nextGuid++;
	n.setUid(uid);
	avatars.push_back(n);
	auto i = avatars.end();
	assert((*i).getGuid() == n.getGuid());
	*a = &(*i);
	return 0;
}

int Player::addAvatar(unsigned int id, Avatar** a) {
	return addAvatar(id, a, 0);
}

int Player::addAvatar(unsigned int id, Avatar** a, unsigned int is_trial) {
	if (a == NULL) return -1;
	Avatar n(id);
	// TODO Allow trial avatars to be saved to db?
	if (!is_trial) {
		n.setGuid(nextGuid);
		nextGuid++;
		n.setUid(uid);
	}
	// TODO Else set trial avatar data
	avatars.push_back(n);
	auto i = avatars.end();
	assert((*i).getGuid() == n.getGuid());
	*a = &(*i);
	return 0;
}

int Player::swapToAvatar(unsigned int id) {
	return swapToAvatar(getAvatarById(id));
}

int Player::swapToAvatar(unsigned long long guid) {
	return swapToAvatar(getAvatarByGuid(guid));
}

int Player::swapToAvatar(const Avatar* a) {
	if (a == NULL) return -1;
	// TODO Implement switching cooldown
	curAvatar = a;
	// TODO Send packet to client indicating status
	return 0;
}

const AvatarTeam* Player::getAvatarTeam(unsigned int i) const {
	if (i >= teams.size()) return NULL;
	for (auto j = teams.cbegin(); j != teams.cend(); j++) {
		if (i == 0) return &(*j);
		i--;
	}
	return NULL;
}

AvatarTeam* Player::getAvatarTeam(unsigned int i) {
	if (i >= teams.size()) return NULL;
	for (auto j = teams.begin(); j != teams.end(); j++) {
		if (i == 0) return &(*j);
		i--;
	}
	return NULL;
}

const AvatarTeam* Player::getCurrAvatarTeam() const {
	return getAvatarTeam(curTeamIndex);
}

AvatarTeam* Player::getCurrAvatarTeam() {
	return getAvatarTeam(curTeamIndex);
}

int Player::swapToTeam(unsigned int i) {
	if (i >= teams.size()) return -1;
	int ret = curTeamIndex;
	curTeamIndex = i;
	return ret;
}

int Player::swapToTeam(const AvatarTeam* team) {
	unsigned int i = 0;
	int ret = curTeamIndex;
	for (auto j = teams.cbegin(); j != teams.cend(); j++) {
		if (team == &(*j)) {
			curTeamIndex = i;
			return ret;
		}
		i++;
	}
	return -1;
}

int Player::addAvatarTeam() {
	return addAvatarTeam("New Team");
}

int Player::addAvatarTeam(const std::string& name) {
	AvatarTeam team;
	team.setName(name);
	teams.push_back(std::move(team));
	return 0;
}

int Player::addAvatarTeam(const AvatarTeam* team) {
	if (team == NULL) return -1;
	teams.push_back(*team);
	return 0;
}

int Player::addAvatarTeam(const std::string name, const std::list<const Avatar*> avatars) {
	AvatarTeam team;
	team.setName(name);
	team.setAvatars(avatars);
	teams.push_back(std::move(team));
	return 0;
}

int Player::setAvatarTeam(const AvatarTeam* team, unsigned int idx) {
	unsigned int i = 0;
	for (auto j = teams.begin(); j != teams.end(); j++) {
		if (i == idx) {
			*j = *team;
			return 0;
		}
		i++;
	}
	return -1;
}

int Player::setAvatarTeam(const std::string name, const std::list<const Avatar*> avatars, unsigned int idx) {
	unsigned int i = 0;
	for (auto j = teams.begin(); j != teams.end(); j++) {
		if (i == idx) {
			AvatarTeam team;
			team.setName(name);
			team.setAvatars(avatars);
			*j = std::move(team);
			return 0;
		}
		i++;
	}
	return -1;
}

int Player::delAvatarTeam(unsigned int idx) {
	unsigned int i = 0;
	for (auto j = teams.cbegin(); j != teams.cend(); j++) {
		if (i == idx) {
			teams.erase(j);
			return 0;
		}
		i++;
	}
	return -1;
}

int Player::addAvatarToTeam(const Avatar* av) {
	return addAvatarToTeam(av, getCurrAvatarTeam());
}

int Player::addAvatarToTeam(const Avatar* av, AvatarTeam* team) {
	if (team == NULL || av == NULL) return -1;
	return team->addAvatar(av);
}

int Player::addAvatarToTeam(unsigned int idx) {
	return addAvatarToTeam(idx, getCurrAvatarTeam());
}

int Player::addAvatarToTeam(unsigned int idx, AvatarTeam* team) {
	if (team == NULL) return -1;
	return team->addAvatar(getAvatarById(idx));
}

int Player::addAvatarToTeam(unsigned long long guid) {
	return addAvatarToTeam(guid, getCurrAvatarTeam());
}

int Player::addAvatarToTeam(unsigned long long guid, AvatarTeam* team) {
	if (team == NULL) return -1;
	return team->addAvatar(getAvatarByGuid(guid));
}

int Player::removeAvatarFromTeam(const Avatar* av) {
	return removeAvatarFromTeam(av, getCurrAvatarTeam());
}

int Player::removeAvatarFromTeam(const Avatar* av, AvatarTeam* team) {
	if (av != NULL && team != NULL) team->delAvatar(av);
	return 0;
}

int Player::removeAvatarFromTeam(unsigned int idx) {
	return removeAvatarFromTeam(idx, getCurrAvatarTeam());
}

int Player::removeAvatarFromTeam(unsigned int idx, AvatarTeam* team) {
	if (team != NULL) team->delAvatar(getAvatarById(idx));
	return 0;
}

int Player::removeAvatarFromTeam(unsigned long long guid) {
	return removeAvatarFromTeam(guid, getCurrAvatarTeam());
}

int Player::removeAvatarFromTeam(unsigned long long guid, AvatarTeam* team) {
	if (team != NULL) team->delAvatar(getAvatarByGuid(guid));
	return 0;
}

unsigned int Player::getOpenstate(unsigned int state) const {
	for (auto i = openstates.cbegin(); i != openstates.end(); i++) {
		if (state == *i) return 1;
	}
	return 0;
}

void Player::setOpenstate(unsigned int state) {
	setOpenstate(state, 0);
}

void Player::setOpenstate(unsigned int state, int sendNotify) {
	for (auto i = openstates.cbegin(); i != openstates.end(); i++) {
		if (state == *i) return;
	}
	openstates.push_back(state);
	if (sendNotify) {
		if (session != NULL && session->getState() >= Session::LOGIN_WAIT) {
			proto::OpenStateChangeNotify p;
			std::string pkt_data;
			auto os_list = p.mutable_open_state_map();
			(*os_list)[state] = 1;
			if (p.SerializeToString(&pkt_data)) {
				Packet pkt(125);
				pkt.setData(pkt_data);
				session->sendPacket(pkt);
			}
		}
	}
}

void Player::clearOpenstate(unsigned int state) {
	clearOpenstate(state, 0);
}

void Player::clearOpenstate(unsigned int state, int sendNotify) {
	for (auto i = openstates.cbegin(); i != openstates.end(); i++) {
		if (state == *i) {
			openstates.erase(i);
		}
	}
	if (sendNotify) {
		if (session != NULL && session->getState() >= Session::LOGIN_WAIT) {
			proto::OpenStateChangeNotify p;
			std::string pkt_data;
			auto os_list = p.mutable_open_state_map();
			(*os_list)[state] = 0;
			if (p.SerializeToString(&pkt_data)) {
				Packet pkt(125);
				pkt.setData(pkt_data);
				session->sendPacket(pkt);
			}
		}
	}
}

void Player::updateOpenstates() {
	updateOpenstates(0);
}

void Player::updateOpenstates(int sendNotify) {
	if (globalGameData == NULL) return;
	const OpenStateData* data = globalGameData->openstate_data;
	if (data == NULL) return;
	const OpenStateDataEnt* state;
	for (unsigned int i = 0; i < data->size(); i++) {
		state = (*data)[i];
		if (state == NULL) continue;
		if (state->is_default) {
			setOpenstate(state->id, 0);
			continue;
		}
		for (unsigned int j = 0; j < 2; j++) {
			switch (state->openCond[j].type) {
			default:
				break;
			case OPEN_STATE_COND_PLAYER_LEVEL:
				if (ar >= state->openCond[j].param[0]) {
					setOpenstate(state->id, 0);
				}
				break;
			// TODO All other openstate conditions
			}
		}
	}
	if (sendNotify) {
		if (session != NULL && session->getState() >= Session::LOGIN_WAIT) {
			proto::OpenStateUpdateNotify p;
			std::string pkt_data;
			auto os_list = p.mutable_open_state_map();
			for (auto i = openstates.cbegin(); i != openstates.cend(); i++) {
				(*os_list)[*i] = 1;
			}
			if (p.SerializeToString(&pkt_data)) {
				Packet pkt(124);
				pkt.setData(pkt_data);
				session->sendPacket(pkt);
			}
		}
	}
}

long long Player::getProp(unsigned int p) const {
	switch(p) {
	default:
		try {
			return props.at(p);
		}
		catch(const std::out_of_range& e) {
			return 0;
		}
	case PROP_PLAYER_LEVEL:
		return ar;
	case PROP_PLAYER_EXP:
		return ar_exp;
	case PROP_PLAYER_WORLD_LEVEL:
		return worldLevel;
	}
}

void Player::setProp(unsigned int p, long long v) {
	switch(p) {
	case PROP_PLAYER_LEVEL:
		ar = v;
		break;
	case PROP_PLAYER_EXP:
		ar_exp = v;
		break;
	case PROP_PLAYER_WORLD_LEVEL:
		worldLevel = v;
		break;
	}
	props[p] = v;
}

void Player::onLogin(Session& s) {
	KcpSession* kcp = s.getKcpSession();
	if (kcp == NULL) return;
	Player* player = s.getPlayer();
	if (player == NULL) return;
	std::string pkt_data;
	// TODO Hardcoded until proper handling for avatar/team data is implemented
	proto::AvatarInfo* avp;
	proto::AvatarDataNotify adn;
	proto::AvatarTeam at;
	unsigned long long guid = ((unsigned long long) player->getUid() << 32) | 0xd0d0c0;
	Avatar av(10000029); // Klee can help!
	av.setGuid(guid);
	avp = adn.add_avatar_list();
	*avp = av;
	at.add_avatar_guid_list(guid);
	at.set_team_name("yagips test team");
	auto m = adn.mutable_avatar_team_map();
	(*m)[1] = at;
	adn.set_choose_avatar_guid(228); // TODO what actually needs to go here?
	adn.set_cur_avatar_team_id(1);
	if (adn.SerializeToString(&pkt_data)) {
		Packet adn_p(1716);
		adn_p.buildHeader(s.nextSeq());
		adn_p.setData(pkt_data);
		s.sendPacket(adn_p);
	}
	proto::Vector* _pos = new proto::Vector();
	*_pos = pos;
	proto::PlayerEnterSceneNotify esn;
	esn.set_scene_id(scene_id);
	esn.set_allocated_pos(_pos);
	esn.set_scene_begin_time(curTimeMs());
	esn.set_target_uid(uid);
	esn.set_enter_scene_token(tpToken);
	esn.set_is_first_login_enter_scene(1);
	esn.set_world_level(worldLevel);
	esn.set_enter_reason(1);
	esn.set_type(proto::ENTER_SELF);
	if (esn.SerializeToString(&pkt_data)) {
		Packet esn_p(201);
		esn_p.setData(pkt_data);
		s.sendPacket(esn_p);
	}
	updateOpenstates(1);
	proto::PlayerDataNotify pdn;
	pdn.set_nick_name(name);
	pdn.set_server_time(curTimeMs());
	pdn.set_is_first_login_today(1); // TODO Add a check to see if this is actually true
	auto* prop_map = pdn.mutable_prop_map();
	for (auto p = props.cbegin(); p != props.cend(); p++) {
		(*prop_map)[p->first].set_type(p->first);
		(*prop_map)[p->first].set_val(p->second);
		(*prop_map)[p->first].set_ival(p->second);
	}
	// TODO Hardcoded until proper region id handling is implemented
	pdn.set_region_id(1);
	if (pdn.SerializeToString(&pkt_data)) {
		Packet pdn_p(108);
		pdn_p.buildHeader(2);
		pdn_p.setData(pkt_data);
		s.sendPacket(pdn_p);
	}
	s.setState(Session::ACTIVE);
}
