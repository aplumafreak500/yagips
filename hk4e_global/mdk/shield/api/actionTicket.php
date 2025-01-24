<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

header("Content-Type: application/json");
// TODO Verify the returned data with upstream server
print(json_encode([
	"retcode" => 0,
	"message" => "ok",
	"data" => [
		// TODO Randomize this
		"ticket" => "AAAAAAAAAAA="
	]
], JSON_NUMERIC_CHECK));
?>
