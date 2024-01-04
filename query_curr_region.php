<?php
/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

function parseHeaders($headers) {
	$head = array();
	foreach($headers as $k=>$v) {
		$t = explode(':', $v, 2);
		if(isset($t[1])) {
			$head[trim($t[0])] = trim($t[1]);
		}
		else {
			$head[] = $v;
			if(preg_match("#HTTP/[0-9\.]+\s+([0-9]+)#", $v, $out)) {
				$head['reponse_code'] = intval($out[1]);
			}
		}
	}
	return $head;
}

$ctx = stream_context_create([
	"http" => [
		"method" => "POST",
		"header" => [
			"Content-Type: application/json",
			"Connection: close"
		],
		"content" => json_encode($_GET, JSON_NUMERIC_CHECK),
		"ignore_errors" => true
	]
]);
// TODO Make this configurable
$str = @fopen("http://127.0.0.1:22100/query_cur_region", "rb", false, $ctx);
if ($str === false) {
	// Print a generic preencoded response.
	// TODO actual JSON response
	header("HTTP/1.1 503 Service Unavailable");
	header("Content-Type: text/plain");
	print("CP///////////wE=");
	exit(0);
}
$rsp = @stream_get_contents($str);
if ($rsp === false) {
	// Print a generic preencoded response.
	// TODO actual JSON response
	header("HTTP/1.1 502 Bad Gateway");
	header("Content-Type: text/plain");
	print("CP///////////wE=");
	exit(0);
}
// forward status code, Content-Type header, and body
$rsp_arr = stream_get_meta_data($str);
fclose($str);
if (isset($rsp_arr["wrapper_data"])) {
	$rsp_hdr = parseHeaders($rsp_arr["wrapper_data"]);
	if (isset($rsp_hdr[0])) {
		header($rsp_hdr[0]);
	}
	if (isset($rsp_hdr["Content-Type"])) {
		header("Content-Type: ".$rsp_hdr["Content-Type"]);
	}
	else {
		header("Content-Type: text/plain");
	}
}
else {
	header("Content-Type: text/plain");
}
print($rsp);
?>
