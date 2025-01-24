<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

if (strcmp($_SERVER["REQUEST_METHOD"], "POST") !== 0) {
	header("HTTP/1.1 400 Bad Request");
	header("Content-Type: application/json");
	print(json_encode([
		"retcode" => -1,
		"message" => "Not a POST request"
	], JSON_NUMERIC_CHECK));
	exit(0);
}
$post_str = @file_get_contents("php://input");
if ($post_str === false) {
	header("HTTP/1.1 500 Internal Server Error");
	header("Content-Type: application/json");
	print(json_encode([
		"retcode" => -1,
		"message" => "Can't get POST response data"
	], JSON_NUMERIC_CHECK));
	exit(0);
}
$post = @json_decode($post_str, true);
if ($post === null) {
	header("HTTP/1.1 400 Bad Request");
	header("Content-Type: application/json");
	print(json_encode([
		"retcode" => -1,
		"message" => "Failed to parse JSON POST data"
	], JSON_NUMERIC_CHECK));
	exit(0);
}

header("Content-Type: application/json");
// TODO Verify the returned data with upstream server
// TODO Check for the actual presence of the data. Right now we just send the same stuff back to the client, but they need checked either way
print(json_encode([
	"retcode" => 0,
	"message" => "ok",
	"data" => [
		// TODO Official server masks username and email (not realname!?) for privacy reasons
		"realname" => $post["realname"],
		"identity_card" => $post["identity_card"],
		// "uid" => 1,
		// "name" => "yagips",
		// "email" => "test@yagips.lumafi.io",
	]
], JSON_NUMERIC_CHECK));
?>
