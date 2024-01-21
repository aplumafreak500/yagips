<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// Unknown what any of these do.
$rsp_data = [
	"priv_proto" => "",
	"teenager_proto" => "",
	"third_proto" => "",
	"user_proto" => "",
	"full_priv_proto" => "",
	"create_time" => "0",
	"id" => 0,
	"major" => 13,
	"minimum" => 0
];

if (is_numeric($_GET["app_id"])) {
	if (isset($_GET["app_id"])) {
		$rsp_data["app_id"] = intval($_GET["app_id"], 10);
	}
	else {
		$rsp_data["app_id"] = 4;
	}
}
else {
	$rsp_data["app_id"] = 4;
}

if (is_numeric($_GET["major"])) {
	if (isset($_GET["major"])) {
		$major = $_GET["major"];
	}
	else {
		$major = 0;
	}
}
else {
	$major = 0;
}

if (isset($_GET["language"])) {
	$rsp_data["language"] = $_GET["language"];
}
else {
	$rsp_data["language"] = "en";
}

if ($major != 13) {
	$data = [
	 	"modified" => true,
		"protocol" => $rsp_data
	];
}
else {
	$data = [
	 	"modified" => false,
		"protocol" => null
	];
}

header("Content-Type: application/json");
print(json_encode([
	"retcode" => 0,
	"message" => "ok",
	"data" => $data
], 0));

?>
