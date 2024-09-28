/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef PERMISSION_H
#define PERMISSION_H
enum class Permission : unsigned int {
	/*
		IMPORTANT: To All Contributers
		Each permission is given an ID. Permissions are stored by their IDs into to a variable-sized list and then stored to a persistent database. Any changes to the existing IDs will break backwards compatibility with older versions of the server and may also create security problems by granting accounts permissions they did not have in prior versions of the server. Thus, all new permissions need to be appended to the end of this list. It is considered best practice to give new permissions IDs that are much higher (at least a couple hundered or thousand) than the last one already on the list when creating a new permission to use in development, and then get a proper ID assigned upon merging into the main tree and/or a release branch.
	*/
	// Basic permissions
	Admin = 0,
	ReadPlayerData = 1,
	WritePlayerData = 2,
	ReadOtherPlayerData = 3,
	WriteOtherPlayerData = 4,
	RunCommands = 5,
	RunCommamdsSelf = 6,
	LoginInGame = 7,
	LoginWeb = 8,
	CreateAccount = 9,
	CreateOtherAccount = 10,
	DeleteAccount = 11,
	DeleteOtherAccount = 12,
	// Moderation permissions
	Ban = 13,
	Unban = 14,
	Kick = 15,
	// In-game chat related permissions
	Mute = 16,
	Unmute = 17,
	Deafen = 18,
	Undeafen = 19,
	KickCoop = 20,
	MuteSelf = 21,
	DeafenSelf = 22
};
#endif
