/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string>
#include "packet.h"
#include "session.h"
#include "packet_head.pb.h"
#include "scene.pb.h"

int handleEnterSceneReadyReq(Session& session, std::string& header, std::string& data) {
	proto::EnterSceneReadyRsp rsp;
	Packet rsp_pkt(283);
	rsp.set_enter_scene_token(1); // TODO Pull from player class instead of hardcoding it
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.buildHeader(11);
	rsp_pkt.setData(data);
	return session.sendPacket(rsp_pkt);
}
