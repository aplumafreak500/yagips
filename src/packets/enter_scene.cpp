/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string>
#include "player.h"
#include "packet.h"
#include "session.h"
#include "packet_head.pb.h"
#include "scene.pb.h"

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
