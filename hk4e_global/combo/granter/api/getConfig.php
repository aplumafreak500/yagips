<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

header("Content-Type: application/json");
print(json_encode([
	"retcode" => 0,
	"message" => "ok",
	"data" => [
		// enables a qr sanner in the client, presumably for authentication purposes. unknown exaxtly what's expected to be encoded in said qr codes.
		"qr_enabled" => false,
		// related to announcements
		"enable_announce_pic_popup" => true,
		// TODO Get server IP instead of hardcoding the url
		"announce_url" => "https://sdk.hoyoverse.com/hk4e/announcement/index.html?sdk_presentation_style=fullscreen&announcement_version=1.44&sdk_screen_transparent=true&game_biz=hk4e_global&auth_appid=announcement&game=hk4e#/",
		// official server: `原神海外` ("Yuanshen Overseas"/Genshin global)
		"app_name" => "yagips sdkserver",
		"log_level" => "INFO", // controls client->server log level? unknown what other values are accepted
		// unknown exactly what these do
		"protocol" => true,
		"disable_ysdk_guard" => false
	]
], JSON_NUMERIC_CHECK));

?>
