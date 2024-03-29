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
#include "misc.pb.h"

int handlePingReq(Session& session, std::string& header, std::string& data) {
	proto::PacketHead pkt_head;
	proto::PingReq req;
	proto::PingRsp rsp;
	Packet rsp_pkt(6);
	if (!pkt_head.ParseFromString(header)) {
		fprintf(stderr, "Error parsing packet header\n");
		return -1;
	}
	if (!req.ParseFromString(data)) {
		fprintf(stderr, "Error parsing packet data\n");
		return -1;
	}
	session.updateLastPingTime();
	// TODO what to do with `client_time`?
	rsp.set_seq(pkt_head.client_sequence_id());
	if (!rsp.SerializeToString(&data)) {
		fprintf(stderr, "Error building packet data\n");
		return -1;
	}
	rsp_pkt.setHeader(header);
	rsp_pkt.setData(data);
	return session.sendPacket(rsp_pkt);
}
