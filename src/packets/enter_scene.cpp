/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string>
#include "player.h"
#include "avatar.h"
#include "item.h"
#include "packet.h"
#include "session.h"
#include "packet_head.pb.h"
#include "scene.pb.h"
#include "retcode.pb.h"

int handleEnterSceneReadyReq(Session& session, std::string&, std::string& data) {
	proto::EnterSceneReadyRsp rsp;
	proto::EnterScenePeerNotify notif;
	Packet rsp_pkt(proto::EnterSceneReadyRsp_CmdId_CMD_ID);
	Packet notif_pkt(proto::EnterScenePeerNotify_CmdId_CMD_ID);
	Player* player = session.getPlayer();
	if (player == NULL) {
		fprintf(stderr, "No player associated with current session\n");
		// TODO should we send a response packet?
		return -1;
	}
	notif.set_enter_scene_token(player->getTpToken());
	// TODO Hardcoded until proper handling for scenes and worlds are implemented
	notif.set_dest_scene_id(3);
	notif.set_peer_id(1);
	notif.set_host_peer_id(1);
	if (!notif.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data (EnterScenePeerNotify)\n");
		// TODO should we send a response packet?
		return -1;
	}
	notif_pkt.setData(data);
	if (session.sendPacket(notif_pkt) < 0) {
		fprintf(stderr, "Error sending packet data (EnterScenePeerNotify)\n");
		// TODO should we send a response packet?
		return -1;
	}
	rsp.set_enter_scene_token(player->getTpToken());
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.buildHeader(11);
	rsp_pkt.setData(data);
	return session.sendPacket(rsp_pkt);
}

int handleSceneInitFinishReq(Session& session, std::string&, std::string& data) {
	proto::SceneInitFinishReq req;
	proto::SceneInitFinishRsp rsp;
	proto::PlayerEnterSceneInfoNotify esin;
	proto::SceneTeamUpdateNotify stun;
	proto::AvatarEnterSceneInfo* avatarInfo;
	proto::TeamEnterSceneInfo* teamInfo;
	proto::MPLevelEntityInfo* mpInfo;
	proto::SceneTeamAvatar* sceneTeamAvatar;
	proto::SceneAvatarInfo* sceneAvatarInfo;
	proto::SceneAvatarInfo* sceneAvatarInfo2;
	proto::SceneWeaponInfo* sceneWeaponInfo;
	proto::SceneEntityInfo* sceneEntityInfo;
	proto::AvatarInfo* avatarInfo2;
	Packet rsp_pkt(proto::SceneInitFinishRsp_CmdId_CMD_ID);
	Packet esin_pkt(proto::PlayerEnterSceneInfoNotify_CmdId_CMD_ID);
	Packet stun_pkt(proto::SceneTeamUpdateNotify_CmdId_CMD_ID);
	Player* player = session.getPlayer();
	Avatar* avatar;
	if (player == NULL) {
		fprintf(stderr, "No player associated with current session\n");
		// TODO should we send a response packet?
		return -1;
	}
	if (!req.ParseFromString(data)) {
		fprintf(stderr, "Error parsing packet data\n");
		// TODO should we send a response packet?
		return -1;
	}
	unsigned int tpToken = player->getTpToken();
	if (tpToken != req.enter_scene_token()) {
		rsp.set_retcode(proto::RET_ENTER_SCENE_TOKEN_INVALID);
		goto build;
	}
	rsp.set_enter_scene_token(tpToken);
	// Add player to scene TODO
	// TODO Numerous other Nofify packets are sent here. Which ones are the most critical?
	esin.set_enter_scene_token(tpToken);
	// TODO Hardcoded until proper handling for scenes and entities are implemented
	esin.set_cur_avatar_entity_id((1 << 24) | 1);
	teamInfo = esin.mutable_team_enter_info();
	teamInfo->set_team_entity_id((9 << 24) | 1);
	mpInfo = esin.mutable_mp_level_entity_info();
	mpInfo->set_entity_id((11 << 24) | 1);
	mpInfo->set_authority_peer_id(1);
	avatarInfo = esin.add_avatar_enter_info();
	avatarInfo->set_avatar_entity_id((1 << 24) | 1);
	avatarInfo->set_avatar_guid(((unsigned long long) player->getUid() << 32) | 0xd0d0c0);
	avatarInfo->set_weapon_entity_id((6 << 24) | 1);
	avatarInfo->set_weapon_guid(((unsigned long long) player->getUid() << 32) | 0x1d0d0c0);
	if (!esin.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data (PlayerEnterSceneInfoNotify)\n");
		// TODO should we send a response packet?
		return -1;
	}
	esin_pkt.setData(data);
	if (session.sendPacket(esin_pkt) < 0) {
		fprintf(stderr, "Error sending packet data (PlayerEnterSceneInfoNotify)\n");
		// TODO should we send a response packet?
		return -1;
	}
	// TODO Loop through curTeam instead
	avatar = player->getCurAvatar();
	if (avatar != NULL) {
		sceneTeamAvatar = stun.add_scene_team_avatar_list();
		sceneAvatarInfo = sceneTeamAvatar->mutable_scene_avatar_info();
		sceneEntityInfo = sceneTeamAvatar->mutable_scene_entity_info();
		avatarInfo2 = sceneTeamAvatar->mutable_avatar_info();
		*avatarInfo2 = *avatar;
		// TODO Move to an operator within an Entity class
		sceneEntityInfo->set_life_state(1);
		sceneEntityInfo->set_entity_id((1 << 24) | 1);
		sceneEntityInfo->set_entity_type(proto::ProtEntityType::PROT_ENTITY_AVATAR);
		sceneAvatarInfo->set_uid(player->getUid());
		sceneAvatarInfo->set_avatar_id(avatar->getId());
		sceneAvatarInfo->set_guid(avatar->getGuid());
		sceneAvatarInfo->set_peer_id(1);
		sceneAvatarInfo->add_equip_id_list(avatar->getWeapon()->guid);
		sceneWeaponInfo = sceneAvatarInfo->mutable_weapon();
		sceneWeaponInfo->set_entity_id((6 << 24) | 1);
		sceneWeaponInfo->set_item_id(avatar->getWeapon()->id);
		sceneWeaponInfo->set_guid(avatar->getWeapon()->guid);
		sceneWeaponInfo->set_level(avatar->getWeapon()->data.weapon.level);
		sceneWeaponInfo->set_promote_level(avatar->getWeapon()->data.weapon.ascension);
		// TODO Affixes
		// TODO other fields
		sceneAvatarInfo2 = sceneEntityInfo->mutable_avatar();
		*sceneAvatarInfo2 = *sceneAvatarInfo;
		sceneTeamAvatar->set_weapon_guid(avatar->getWeapon()->guid);
		sceneTeamAvatar->set_weapon_entity_id((6 << 24) | 1);
		sceneTeamAvatar->set_entity_id((1 << 24) | 1);
		sceneTeamAvatar->set_player_uid(player->getUid());
		sceneTeamAvatar->set_scene_id(3); // TODO get what it really is
		if (!stun.SerializeToString(&data)) {
			fprintf(stderr, "Error building packet data (SceneTeamUpdateNotify)\n");
			// TODO should we send a response packet?
			return -1;
		}
		stun_pkt.setData(data);
		if (session.sendPacket(stun_pkt) < 0) {
			fprintf(stderr, "Error sending packet data (SceneTeamUpdateNotify)\n");
			// TODO should we send a response packet?
			return -1;
		}
	}
build:
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.buildHeader(11);
	rsp_pkt.setData(data);
	return session.sendPacket(rsp_pkt);
}
