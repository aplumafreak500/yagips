<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// correct for version 3.2
$clients = [
	0 => "Editor", // root, essentially
	1 => "IOS",
	2 => "Android",
	3 => "PC",
	4 => "PS", // ps4
	5 => "Server", // ipc?
	6 => "CloudAndroid",
	7 => "CloudIOS",
	8 => "PS5",
	9 => "CloudWeb",
	10 => "CloudTV",
	11 => "CloudMacOS",
	12 => "CloudPC",
	13 => "CloudThirdPartyMobile",
	14 => "CloudThirdPartyPC",
];
$rsp_data = [
	"disable_mmt" => true, // disables geetest/captcha
	"disable_regist" => true, // disables register option?
	"enable_email_captcha" => false,
	"enable_ps_bind_account" => false, //  related to linking psn accounts?
	"identity" => "I_IDENTITY", // unknown exactly what this does
	"scene" => "S_TEMPLE", // CN clients only, controls whether mobile phone numbers can be used for auth
	"thirdparty" => [], // third party auth services. "fb" = facebook "tw" = x/twitter "gl" = hoyolab? what else is accepted?
	"guest" => false,
	"server_guest" => false, // enables guest accounts, TODO which one actually controls it, if not both?
	// official server: `原神海外` ("Yuanshen Overseas"/Genshin global)
	"name" => "yagips sdkserver"
];
if (is_numeric($_GET["client"])) {
	if ($_GET["client"] > 14) {
		$rsp_data["client"] = $_GET["client"];
	}
	else {
		$rsp_data["client"] = $clients[$_GET["client"]];
	}
}
else {
	$rsp_data["client"] = $_GET["client"];
}
if (isset($_GET["game_key"])) {
	$rsp_data["game_key"] = $_GET["game_key"];
}
else {
	$rsp_data["game_key"] = "hk4e_global";
}
header("Content-Type: application/json");
print(json_encode([
	"retcode" => 0,
	"message" => "ok",
	"data" => $rsp_data
], 0));

?>
