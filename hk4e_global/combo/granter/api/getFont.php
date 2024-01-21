<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

header("Content-Type: application/json");
// TODO Get server address instead of hardcoding the urls
print(json_encode([
	"retcode" => 0,
	"message" => "ok",
	"data" => [
		"fonts" => [
			[
				"app_id" => 0,
				"font_id" => "0",
				"md5" => "4398dec1a0ffa3d3ce99ef1424107550",
				"name" => "zh-cn.ttf",
				"url" => "https://sdk.hoyoverse.com/zn-cn.ttf"
			],
			[
				"app_id" => 0,
				"font_id" => "0",
				"md5" => "2c148f36573625fc03c82579abd26fb1",
				"name" => "ja.ttf",
				"url" => "https://sdk.hoyoverse.com/ja.ttf"
			]
		]
	]
], 0));

?>
