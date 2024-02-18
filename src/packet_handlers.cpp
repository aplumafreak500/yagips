/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string>
#include "session.h"
#include "packet.h"
// TODO Use a file full of enum constants that can be auto generated. This is due to the fact that opcode IDs (usually) change from one client version to the next.

extern int handlePingReq(Session&, std::string&, std::string&);
extern int handleGetPlayerTokenReq(Session&, std::string&, std::string&);
extern int handlePlayerLoginReq(Session&, std::string&, std::string&);
extern int handleEnterSceneReadyReq(Session&, std::string&, std::string&);

int processPacket(Session& session, Packet& packet) {
	unsigned int opcode = packet.getOpcode();
	std::string header = packet.getHeader();
	std::string data = packet.getData();
	switch (opcode) {
	default:
		fprintf(stderr, "Don't know how to handle opcode %d\n", opcode);
		return -1;
	case 5:
		return handlePingReq(session, header, data);
	case 101:
		return handleGetPlayerTokenReq(session, header, data);
	case 103:
		return handlePlayerLoginReq(session, header, data);
	case 282:
		return handleEnterSceneReadyReq(session, header, data);
	}
}
