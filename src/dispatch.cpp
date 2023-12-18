/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/random.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include "gameserver.h"
#include "util.h"
#include "crypt.h"
#include "dispatch.h"
#include "dispatch.pb.h"
#include "keys.h"
#include "http.h"

// TODO pass in a struct containing the HTTP GET parameters
std::string getQueryRegionListHttpRsp() {
	proto::QueryRegionListHttpRsp ret;
	proto::RegionSimpleInfo* region = ret.add_region_list();
	std::string ret_enc;
	// TODO Get these from config
	// TODO Unknown what these do
	const char* config = "{\"sdkenv\":\"2\",\"checkdevice\":false,\"loadPatch\":false,\"showexception\":false,\"regionConfig\":\"pm|fk|add\",\"downloadMode\":0,\"codeSwitch\":[0]}";
	size_t config_sz = strlen(config);
	char configBuf[1024];
	size_t i;
	for (i = 0; i < config_sz; i++) {
		configBuf[i] = config[i] ^ dispatchKey[i % 4096];
	}
	/* Text ID of the region */
	region->set_name("os_usa");
	/* Name of the region (displayed in client) */
	region->set_title("yagips test");
	/* Server type (TODO unknown what this does) */
	region->set_type("DEV_PUBLIC");
	/* Query current region URL */
	region->set_dispatch_url("http://ps.yuuki.me/query_cur_region"); // TODO Temporary value!
	/* Dispatch seed (used to derive xor key) */
	std::string seed((const char*) dispatchSeed, 2076);
	ret.set_client_secret_key(seed);
	/* TODO Unknown what this does. */
	ret.set_enable_login_pc(1);
	/* Encrypted json config object. */
	std::string cconfig(configBuf, config_sz);
	ret.set_client_custom_config_encrypted(cconfig);
	if (!ret.SerializeToString(&ret_enc)) {
		return "";
	}
	return ret_enc;
}

// TODO pass in a struct containing the HTTP GET parameters
std::string getQueryCurrRegionHttpRsp(std::string& sign, int doEnc) {
	int doSign = 1;
	proto::QueryCurrRegionHttpRsp ret;
	std::string ret_enc;
	// TODO Most, if not all, of this data can be pulled from the config and/or client.
	/* Unknown Fields TODO
		* client_secret_key - ECB seed struct. At least on Yuuki, this is actually different from the one given in the region list. Unknown what this is actually used for, but it's likely that, once derived, it's the same as the secretKey we already have.
		* region_custom_config_encrypted - unknown JSON object (I think) encrypted with either the region list client_secret_key or the one from this message (idk which). Also unknown how/if it differs from the one below, or with the one from query_cur_region
		* client_region_custom_config_encrypted - unknown JSON object (I think) encrypted with either the region list client_secret_key or the one from this message (idk which). Also unknown how/if it differs from the one above, or with the one from query_cur_region
	*/
	/* Status code. 0 means success, anything else means an error. TODO see if there are differences in client behavior whenever retcode is positive or negative */
	ret.set_retcode(0);
	/* Return message. I'm pretty sure this is used to report things like bans or server problems, and that, if it's set, it displays when retcode != 0. TODO Verify */
	ret.set_msg("ok");
	/* Start Region Info */
	proto::RegionInfo* region = new proto::RegionInfo;
	/* Unknown Fields TODO
		* pay_callback_url - definitely related to IAP, but unknown how it's used
		* area_type - Unknown. maybe CN vs OS?
		* resource_url_bak - Backup base URL for resources? (On Yuuki this is set to just the branch name...?)
		* data_url_bak - most likely same as resource_url_bak, but for data/silence
		* next_resource_url - Preload resource base URL(?)
		* secret_key - ECB seed struct. On Yuuki, this is the same as the dispatch seed. Unknown if this is true on vanilla. Also unknown if it's even used.
		* game_biz - Unknown.
	*/
	/* Gameserver IP. TODO Unknown if the game accepts IPv6 addresses. */
	region->set_gateserver_ip("192.168.49.1");
	/* Gameserver domain name. (Use instead of gateserver_ip if DNS needs to be used for the gameserver; helpful for eg. DDNS or load balancing) */
	// region->set_use_gateserver_domain_name(1);
	// region->set_gateserver_domain_name("ps.yuuki.me");
	/* Gameserver port */
	region->set_gateserver_port(22102);
	/* Note: The gameserver can work without any more fields set. Tho, behavior might be weird when accessing web stuff, because the client would be attempting to open an empty string as a URL. (Also, in-game resource downloads won't work.) */
	/* Where the "Feedback" button in the Paimon menu leads to. */
	region->set_feedback_url("http://ps.yuuki.me/feedback");
	/* Where the "Notices" button in the Paimon menu leads to. TODO Verify */
	region->set_bulletin_url("http://ps.yuuki.me/news");
	/* Where the "Version Highlights" button in the Paimon menu leads to. TODO Verify */
	region->set_handbook_url("http://ps.yuuki.me/book");
	/* Where the "Community" button in the Paimon menu leads to. */
	region->set_official_community_url("http://discord.yuuki.me");
	/* Where the "User Center" menu in the settings menu leads to. */
	region->set_user_center_url("http://ps.yuuki.me/user");
	/* Where the "Privacy Policy" button in the settings menu leads to. TODO Verify */
	region->set_privacy_policy_url("http://ps.yuuki.me/privacy");
	/* TODO Unknown what exactly this does. I do seem to remember another button in the settings menu... */
	region->set_account_bind_url("http://ps.yuuki.me/account");
	/* TODO Unknown what exactly this does. */
	region->set_cdkey_url("http://ps.yuuki.me/api/key/hk4e/");
	/* Base URL for client resources */
	region->set_resource_url("http://ps.yuuki.me/data_game/genshin/client_game_res/3.2_live");
	/* Base URL for client design data (+ silence) */
	region->set_data_url("http://ps.yuuki.me/data_game/genshin/client_design_data/3.2_live");
	/* Data revision */
	region->set_client_data_version(11793813);
	/* Data hash suffix. Unknown how this is generated (at least on vanilla). */
	region->set_client_version_suffix("1bb7eba264");
	/* JSON object containing name, MD5 and size of data file list */
	// TODO Set these to their proper values
	//region->set_client_data_md5("{\"remoteName\":\"data_versions\",\"md5\":\"\", \"fileSize\":0}");
	/* Silence revision */
	region->set_client_silence_data_version(11212885);
	/* Silence hash suffix. Unknown how this is generated (at least on vanilla). */
	region->set_client_silence_version_suffix("766b0a2560");
	/* JSON object containing name, MD5 and size of silence file list */
	// TODO Set these to their proper values
	//region->set_client_silence_data_md5("{\"remoteName\":\"data_versions\",\"md5\":\"\",\"fileSize\":0}");
	/* Start Resource Config */
	proto::ResVersionConfig* res = new proto::ResVersionConfig;
	/* Resource revision */
	res->set_version(11611027);
	/* Resource hash suffix. Unknown how this is generated (at least on vanilla). */
	res->set_version_suffix("8bf0cf5f3d");
	/* JSON object containing name, MD5, and size of resource file lists, one line per file. On vanilla, the list of files are: res_versions_{external,medium,streaming} and base_revision. Preloads also list audio_diff_versions. */
	// TODO Set these to their proper values
	/*res->set_md5(
		"{\"remoteName\":\"res_versions_external\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"res_versions_medium\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"res_versions_streaming\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"base_revision\",\"md5\":\"\",\"fileSize\":0}\n"
		// TODO release_res wut? (base rev? original unmodified files?)
		"{\"remoteName\":\"release_res_versions_external\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"release_res_versions_medium\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"release_res_versions_streaming\",\"md5\":\"\",\"fileSize\":0}"
	);*/
	/* Branch name */
	res->set_branch("3.2_live");
	/* TODO Unknown what exactly this does. */
	// res->set_release_total_size("0");
	/* TODO Unknown what exactly this does. */
	// res->set_relogin(0);
	/* End Resource Config */
	region->set_allocated_res_version_config(res);
	/* Start Next Resource Config */
	// proto::ResVersionConfig* resNext = new proto::ResVersionConfig;
	/* Resource revision */
	// TODO 3.3's proper value
	// resNext->set_version(0);
	/* Resource hash suffix. Unknown how this is generated (at least on vanilla). */
	// TODO 3.3's proper value
	// resNext->set_version_suffix("0000000000");
	/* JSON object containing name, MD5, and size of resource file lists, one line per file. On vanilla, the list of files are: res_versions_{external,medium,streaming} and base_revision. Preloads also list audio_diff_versions. */
	// TODO 3.3's proper values
	/*resNext->set_md5(
		"{\"remoteName\":\"res_versions_external\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"res_versions_medium\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"res_versions_streaming\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"base_revision\",\"md5\":\"\",\"fileSize\":0}\n"
		"{\"remoteName\":\"audio_diff_versions_32-33\",\"md5\":\"\",\"fileSize\":0}"
	);*/
	/* Branch name */
	// resNext->set_branch("3.3_live");
	/* TODO Unknown what exactly this does. */
	// resNext->set_release_total_size("0");
	/* TODO Unknown what exactly this does. */
	// resNext->set_relogin(0);
	/* Script version string. Normally not set in current resource config. TODO Unknown what exactly this does. But it may have to do with where the client places the preload data. */
	// resNext->set_next_script_version("3.3.0");
	/* End Next Resource Config */
	// region->set_allocated_next_res_version_config(resNext);
	/* End Region Info */
	ret.set_allocated_region_info(region);
	/* Start Server Stop Data */
	// proto::StopServerInfo* stop = new proto::StopServerInfo;
	/* Start of maintenance as Unix timestamp */
	// stop->set_stop_begin_time(0);
	/* End of maintenance as Unix timestamp */
	// stop->set_stop_end_time();
	/* a URL; iirc this is where clients go when a "more information" button is clicked TODO verify */
	// stop->set_url("http://discord.yuuki.me");
	/* Return message. On vanilla this just tells people to go to HoyoLAB (TODO verify), whereas on Grasscutter it also reports what client version(s) the server supports. */
	// stop->set_content_msg("");
	/* End Server Stop Data */
	// ret.set_allocated_stop_server(stop);
	/* Start Force Update */
	// proto::ForceUpdateInfo* upd = new proto::ForceUpdateInfo;
	/* I'm pretty sure the client gets sent this if they need to update after a new major version releases. TODO Verify */
	// upd->set_force_update_url("http://ps.yuuki.me/game/genshin-impact");
	/* End Force Update */
	// ret.set_allocated_stop_server(upd);
	if (!ret.SerializeToString(&ret_enc)) {
		sign = "";
		return "";
	}
	size_t sz = ret_enc.size() - 1;
	size_t sign_sz = 4096;
	unsigned char sign_c[sign_sz];
	int j;
	if (doSign) {
		j = HyvCryptRsaSign((unsigned char*) ret_enc.c_str(), sz, sign_c, sign_sz);
		if (j < 0) {
			sign = "";
		}
		else {
			sign.assign((const char*) sign_c, (unsigned int) j);
		}
	}
	if (!doEnc) {
		return ret_enc;
	}
	// TODO HACK - get libasan to stop complaining about a read buffer overflow
	ret_enc.resize(sz * 2, '\0');
	// Default to twice the size of the buffer to account for some crazy small keys.
	unsigned char* tmpbuf = (unsigned char*) malloc(sz * 2);
	if (tmpbuf == NULL) {
		return ret_enc; // fall back to unencrypted buffer
	}
	// TODO Get key id from client
	j = HyvCryptRsaEnc((unsigned char*) ret_enc.c_str(), sz, tmpbuf, sz * 2, 4);
	if (j < 0) {
		free(tmpbuf);
		return ret_enc;
	}
	ret_enc.assign((const char*) tmpbuf, j);
	free(tmpbuf);
	return ret_enc;
}

extern "C" {
	volatile int DispatchServerSignal = 0;

	enum {
		SDK_QUERY_REGION_LIST = 0,	// Get list of regions
		SDK_QUERY_CURR_REGION,		// Get region data
		SDK_GET_AUTH_TOKEN,			// Generate new auth token
		SDK_CHECK_AUTH_TOKEN,		// Verify auth token
		SDK_CHECK_COMBO_TOKEN,		// Verify auth token and send back combo token
		SDK_GACHA_CUR,				// Current banners
		SDK_GACHA_HIST,				// Gacha history
		SDK_REDEEM,					// Redeem gift codes and IAPs, also out-of-game rewards (hurichurl's treasure trove, daily check in, etc.)
		SDK_GM_CMD,					// Web-based GM command (Ayaka bot commands are handled as part of the in-game chat)
		SDK_STATS,					// Server-wide statistics
		SDK_GET_PLAYER_DATA,		// player data including in-game info (traveler's diary/battle chronicle) and also account details (useful for external dispatch servers)
		SDK_SET_PLAYER_DATA,		// set password/oath tokens or reset the account, also a low-level interface to directly set in-game data without going through gm
		SDK_ADMIN,					// Shut down or restart the server, manage the banlist, change abyss/gacha/event schedules
		SDK_ENDPOINT_CNT
	};

	const char* const sdkEndpoints[SDK_ENDPOINT_CNT] = {
		[SDK_QUERY_REGION_LIST] = "/query_region_list",
		[SDK_QUERY_CURR_REGION] = "/query_cur_region",
		[SDK_GET_AUTH_TOKEN] = "/session/new", // "short" for /hk4e_{cn,global}/mdk/shield/api/login
		[SDK_CHECK_AUTH_TOKEN] = "/session/verify", // "short" for /hk4e_{cn,global}/mdk/shield/api/verify
		[SDK_CHECK_COMBO_TOKEN] = "/session/combo", // "short" for /hk4e_{cn,global}/combo/granter/login/v2/login
		[SDK_GACHA_CUR] = "/gacha/details",
		[SDK_GACHA_HIST] = "/gacha/history",
		[SDK_REDEEM] = "/redeem",
		[SDK_GM_CMD] = "/gm",
		[SDK_STATS] = "/stat",
		[SDK_GET_PLAYER_DATA] = "/player/",
		[SDK_SET_PLAYER_DATA] = "/set-player/",
		[SDK_ADMIN] = "/admin",
	};

	// Backend for the dispatch server
	void* DispatchServerMain(__attribute__((unused)) void* __u) {
		const char* ip = "::";
		unsigned short port = 22100;
		int fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		int cfd = -1;
		int opt = 0;
		if (fd < 0) {
			fprintf(stderr, "Failed to create dispatch server socket, errno = %d (%s)\n", errno, strerror(errno));
			GameserverSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(int)) < 0) {
			fprintf(stderr, "Warning: Unable to unset option IPV6_V6ONLY (errno %d - %s).\nDispatch server might only be able to accept IPv6 clients.\n", errno, strerror(errno));
		}
		struct in_addr addr4 = {INADDR_ANY};
		struct in6_addr addr6 = IN6ADDR_ANY_INIT;
		sock_t srv, client;
		if (inet_pton(AF_INET6, ip, &addr6) == 0) {
			// v6 addr parse error. Try it with v4.
			if (inet_pton(AF_INET, ip, &addr4) != 1) {
				fprintf(stderr, "Failed to parse the bind address %s", ip);
				GameserverSignal = 1; // TODO enum constant
				return (void*) -1;
			}
			// Special logic for certain specific v4 addresses.
			// Check for an address in loopback range.
			if (ntohl(addr4.s_addr) >> 24 == 127) {
				// Set the last byte of addr6 to 1, indicating IPv6 loopback.
				addr6.s6_addr[15] = 1;
			}
			// Check if not INADDR_ANY
			else if (ntohl(addr4.s_addr) != INADDR_ANY) {
				// Create a v4-mapped v6 address. TODO does it actually work this way?
				memcpy(&addr6.s6_addr[12], &addr4, sizeof(struct in_addr));
				addr6.s6_addr[10] = 0xff;
				addr6.s6_addr[11] = 0xff;
			}
		}
		srv.sin6_family = AF_INET6;
		srv.sin6_port = htons(port);
		srv.sin6_addr = addr6;
		if (bind(fd, (struct sockaddr*) &srv, sizeof(sock_t)) < 0) {
			fprintf(stderr, "Failed to bind dispatch server socket to %s:%d, errno = %d (%s)\n", ip, port, errno, strerror(errno));
			close(fd);
			GameserverSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		if (listen(fd, 8) < 0) {
			fprintf(stderr, "Failed to listen for connections on dispatch server socket, errno = %d (%s)\n", errno, strerror(errno));
			close(fd);
			GameserverSignal = 1; // TODO enum constant
			return (void*) -1;
		}
		fprintf(stderr, "Successfully bound dispatch backend server to %s:%d\n", ip, port);
		const size_t buf_len = 16 * 1024;
		const unsigned int _hdr_cnt = 128;
		size_t actual_len;
		size_t prev_len, meth_len, path_len;
		size_t hdr_cnt;
		ssize_t rd_len;
		int pret;
		unsigned int status = 200;
		static char pkt_buf[buf_len];
		const char* meth;
		const char* path;
		const char* body;
		struct phr_header headers[_hdr_cnt];
		int mver;
		unsigned int ssz = sizeof(sock_t);
		char cip[64];
		unsigned short cport;
		while (!DispatchServerSignal) {
			pret = 0;
			actual_len = 0;
			cfd = accept(fd, (struct sockaddr*) &client, &ssz);
			if (cfd < 0) {
				fprintf(stderr, "Failed to accept a new connection, errno = %d (%s)\n", errno, strerror(errno));
			}
			inet_ntop(AF_INET6, client.sin6_addr.s6_addr, cip, 64);
			cport = ntohs(client.sin6_port);
			fprintf(stderr, "Accepted new connection from %s:%d\n", cip, cport);
			while (!DispatchServerSignal) {
				if (cfd < 0) break; // Quick safety check
				rd_len = recv(cfd, pkt_buf + actual_len, buf_len - actual_len, 0);
				if (rd_len == 0) {
					fprintf(stderr, "Client closed the connection before a response could be sent.\n");
					close(cfd);
					cfd = -1;
					break;
				}
				if (rd_len < 0) {
					fprintf(stderr, "Failed to read from the client, errno = %d (%s)\n", errno, strerror(errno));
					continue;
				}
				prev_len = actual_len;
				actual_len += rd_len;
				hdr_cnt = _hdr_cnt;
				pret = phr_parse_request(pkt_buf, buf_len, &meth, &meth_len, &path, &path_len, &mver, headers, &hdr_cnt, prev_len);
				if (pret >= -1) break; // -1 means parse error, this is tested for outside the read loop in order to send a 400 response to the client
				if (pret < -2) { // -2 means request hasn't finished yet
					fprintf(stderr, "HTTP parser returned unexpected error %d, **this should never happen!**\n", -pret);
					status = 500;
					goto write_rsp;
				}
				if (actual_len >= buf_len) {
					status = 413;
					goto write_rsp;
				}
			}
			if (cfd >= 0) {
				if (pret < 0) {
					status = 400;
					goto write_rsp;
				}
				body = pkt_buf + pret;
				if (strncmp(meth, "OPTIONS", 7) == 0) {
					status = 1204;
					goto write_rsp;
				}
				if (!(
					strncmp(meth, "GET", 3) == 0 ||
					strncmp(meth, "POST", 4) == 0 ||
					strncmp(meth, "HEAD", 4) == 0
				)) {
					status = 405;
					goto write_rsp;
				}
				// the extra space creates a distinction between the root directory and another path
				// the question mark, on the other hand, accounts for the possibility of GET parameters
				// there's gotta be a more graceful way to handle this
				if (strncmp(path, "/ ", 2) == 0 || strncmp(path, "/?", 2) == 0) {
					status = 200;
					goto write_rsp;
				}
				if (strncmp(path, "/generate_204", 13) == 0 || strncmp(path, "/gen_204", 8) == 0) {
					status = 204;
					goto write_rsp;
				}
				unsigned int i;
				for (i = 0; i < SDK_ENDPOINT_CNT; i++) {
					if (sdkEndpoints[i] == NULL) continue;
					if (strncmp(path, sdkEndpoints[i], strlen(sdkEndpoints[i])) == 0) {
						status = i;
						goto write_rsp;
					}
				}
				// everything else
				if (strncmp(path, "/", 1) == 0) {
					status = 404;
					goto write_rsp;
				}
				status = 400;
write_rsp:
				const char* rsp = "";
				const char* rsp_body = "";
				std::string rsp_str;
				std::string sign;
				size_t rsp_len = 0;
				const char* mime = "text/plain";
				const char* extra_hdrs = "";
				char token[16];
				const char* chk_token;
				switch (status) {
				// Generic responses
				case 404:
				default:
					rsp = "Not Found";
					rsp_str = "{\"retcode\":-10,\"message\":\"No such endpoint\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 204:
					rsp = "No Content";
					mime = "text/plain";
					rsp_len = strlen(rsp_body);
					break;
				case 1204: // options method
					status = 204;
					rsp = "No Content";
					mime = "text/plain";
					rsp_len = strlen(rsp_body);
					extra_hdrs = "Allow: GET, HEAD, POST, OPTIONS\r\n";
					break;
				case 405:
					rsp = "Method Not Allowed";
					rsp_str = "{\"retcode\":-1,\"message\":\"HTTP method not allowed\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					extra_hdrs = "Allow: GET, HEAD, POST, OPTIONS\r\n";
					break;
				case 500:
					rsp = "Internal Server Error";
					rsp_str = "{\"retcode\":-1,\"message\":\"Internal server error\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 413:
					rsp = "Content Too Large";
					rsp_str = "{\"retcode\":-1,\"message\":\"Request too big\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case 200:
					rsp = "OK";
					rsp_str = "hi from yagips dispatch server\n";
					mime = "text/plain";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				// Endpoints - TODO implement them all
				case SDK_QUERY_REGION_LIST:
					// Notes: PHP frontend converts GET (or POST) parameters to JSON before sending it to us in a POST. If we get a GET request anyways (or an empty POST), fall back to some reasonable default parameters.
					// TODO: Parse the parameters and then pass them to this function
					rsp_str = b64enc(getQueryRegionListHttpRsp());
					mime = "text/plain";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case SDK_QUERY_CURR_REGION:
					// Notes: PHP frontend converts GET (or POST) parameters to JSON before sending it to us in a POST. If we get a GET request anyways (or an empty POST), fall back to some reasonable default parameters.
					rsp_str = "{\"content\":\"" + b64enc(getQueryCurrRegionHttpRsp(sign, 1)) + "\"";
					// TODO What to do if there's no signature?
					if (!sign.empty()) rsp_str += ",\"sign\":\"" + b64enc(sign) + "\"";
					rsp_str += "}";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					mime = "application/json";
					break;
				// TODO Wait to fully implement these until we have a database driver to use.
				case SDK_GET_AUTH_TOKEN:
				case SDK_CHECK_AUTH_TOKEN:
				case SDK_CHECK_COMBO_TOKEN:
					rsp_str = "{\"retcode\":-101,\"message\":\"Endpoint not implemented yet, please be patient\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				case SDK_GACHA_HIST:
				case SDK_GET_PLAYER_DATA:
				// These additionally require out-of-game client authentication
				case SDK_SET_PLAYER_DATA:
				case SDK_REDEEM:
				case SDK_GM_CMD:
				case SDK_ADMIN:
				// TODO Wait to fully implement this until we can load config files
				case SDK_GACHA_CUR:
				// TODO Wait to fully implement this until we can gather and report statistics
				case SDK_STATS:
					rsp_str = "{\"retcode\":-1,\"message\":\"Endpoint not implemented yet, please be patient\"}";
					mime = "application/json";
					rsp_body = rsp_str.c_str();
					rsp_len = rsp_str.size();
					break;
				}
				if (status < SDK_ENDPOINT_CNT) {
					status = 200;
					rsp = "OK";
				}
				snprintf(pkt_buf, buf_len,
					"HTTP/1.%d %03d %s\r\n"
					"Content-Length: %ld\r\n"
					"Content-Type: %s\r\n"
					"Connection: close\r\n"
					// TODO Pull these from config.h
					"Server: yagips/0.0a\r\n%s"
					"\r\n%s",
				mver, status, rsp, rsp_len, mime, extra_hdrs, strncmp(meth, "HEAD", 4) == 0 ? "" : rsp_body);
				actual_len = 0;
				ssize_t wr_len = 0;
				prev_len = strnlen(pkt_buf, buf_len);
				while (prev_len > 0) {
					wr_len = send(cfd, pkt_buf + actual_len, prev_len, 0);
					if (wr_len < 0) {
						if (errno == EPIPE) {
							fprintf(stderr, "Client closed the connection before a response could be sent.\n");
						}
						else {
							fprintf(stderr, "Failed to send a response to the client, errno = %d (%s)\n", errno, strerror(errno));
						}
						close(cfd);
						cfd = -1;
						continue;
					}
					prev_len -= wr_len;
					actual_len += wr_len;
				}
				close(cfd);
				cfd = -1;
				continue;
			}
		}
		close(fd);
		GameserverSignal = 1; // TODO enum constant
		return (void*) 0;
	}
}
