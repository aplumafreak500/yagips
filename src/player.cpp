/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

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
	// TODO: Props
	for (int i = 0; i < p.open_states_size(); i++) {
		openstates.push_back(p.open_states(i));
	}
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
	// TODO Props
	for (auto i = openstates.cbegin(); i != openstates.cend(); i++) {
		ret.add_open_states(*i);
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
	// TODO Prop map
	// TODO Hardcoded until proper region id handling is implemented
	pdn.set_region_id(1);
	if (pdn.SerializeToString(&pkt_data)) {
		Packet pdn_p(108);
		pdn_p.setData(pkt_data);
		s.sendPacket(pdn_p);
	}
	s.setState(Session::ACTIVE);
}
