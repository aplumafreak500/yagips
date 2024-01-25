/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "account.h"
#include "dbgate.h"
#include "player.h"
#include "session.h"
#include "kcpsession.h"
#include "packet.h"
#include "vector.h"
#include "avatar.pb.h"
#include "scene.pb.h"
#include "define.pb.h"

Player::Player() {}
Player::~Player() {}

const Account* Player::getAccount() const {
	return account;
}

void Player::setAccount(const Account* a) {
	account = a;
}

unsigned long Player::getUid() const {
	return uid;
}

void Player::setUid(unsigned long u) {
	uid = u;
}

int Player::saveToDb() const {
	return globalDbGate->savePlayer(*this);
}

void Player::onLogin(Session& session) {
	KcpSession* kcp = session.getKcpSession();
	if (kcp == NULL) return;
	Player* player = session.getPlayer();
	if (player == NULL) return;
	std::string pkt_data;
	// TODO Hardcoded until proper handling for avatar/team data is implemented
	proto::AvatarInfo* av;
	proto::AvatarDataNotify adn;
	unsigned long long guid = (player->getUid() << 32) | 0xd0d0c0;
	av = adn.add_avatar_list();
	av->set_avatar_id(10000029); // Klee can help!
	av->set_guid(guid);
	// TODO Skill depot, props, and (default) weapon
	// TODO add to team
	adn.set_choose_avatar_guid(guid);
	if (adn.SerializeToString(&pkt_data)) {
		Packet adn_p(1716);
		adn_p.buildHeader(session.nextSeq());
		adn_p.setData(pkt_data);
		session.sendPacket(adn_p);
	}
	// TODO Hardcoded until proper handling for scene data is implemented
	proto::Vector* startPos = new proto::Vector();
	struct timespec c;
	unsigned long long curms;
	clock_gettime(CLOCK_REALTIME, &c);
	curms = (c.tv_sec * 1000) + (c.tv_nsec / 1000000);
	startPos->set_x(2747.6);
	startPos->set_y(194.7);
	startPos->set_z(-1719.4);
	proto::PlayerEnterSceneNotify esn;
	esn.set_scene_id(3);
	esn.set_allocated_pos(startPos);
	esn.set_scene_begin_time(curms);
	esn.set_target_uid(uid);
	esn.set_enter_scene_token(1);
	esn.set_type(proto::ENTER_SELF);
	if (esn.SerializeToString(&pkt_data)) {
		Packet esn_p(201);
		esn_p.setData(pkt_data);
		session.sendPacket(esn_p);
	}
}
