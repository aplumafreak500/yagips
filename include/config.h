/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef CONFIG_H
#define CONFIG_H
class RegionConfig;

class Config {
public:
	Config();
	~Config();
	const char* getGameserverBindIp();
	unsigned short getGameserverBindPort();
	const char* getDispatchBindIp();
	unsigned short getDispatchBindPort();
	unsigned int getMaxSessions();
	const char* getDbPath();
	const char* getDataPath();
private:
	const char* gameserver_bind_ip;
	unsigned short gameserver_bind_port;
	const char* dispatch_bind_ip;
	unsigned short dispatch_bind_port;
	unsigned int max_sessions;
	const char* db_path;
	const char* data_path;
};

class RegionConfig {
public:
	
private:
	
};

#endif
