/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "command.h"
#define IN_ACCOUNT_CMD
#include "commands/account.h"
#include <stdlib.h>
#include <string.h>
#include "account.h"
#include "dbgate.h"

const std::string AccountCommand::name() const {
	return "account";
}

const std::vector<std::string> AccountCommand::aliases() const {
	return {};
}

const std::string AccountCommand::description() const {
	return "Manage accounts and account data";
}

const std::string AccountCommand::usage() const {
	return "No documentation is available as of yet";
}

enum {
	SUBCMD_CREATE = 0,
	SUBCMD_DELETE,
	SUBCMD_EDIT,
	SUBCMD_PASSWD,
	SUBCMD_CNT
};

static int handleCreate(const Account*, unsigned int, const char* const*);
static int handleDelete(const Account*, unsigned int, const char* const*);
static int handleEdit(const Account*, unsigned int, const char* const*);
static int handlePasswd(const Account*, unsigned int, const char* const*);

struct Subcommand {
	const char* name;
	int (*handle)(const Account*, unsigned int, const char* const*);
};

static const Subcommand subcommands[] = {
	[SUBCMD_CREATE] = {"create", handleCreate},
	[SUBCMD_DELETE] = {"delete", handleDelete},
	[SUBCMD_EDIT] = {"edit", handleEdit},
	[SUBCMD_PASSWD] = {"passwd", handlePasswd},
	[SUBCMD_CNT] = {NULL, NULL}
};

int AccountCommand::handle(const Player* player, unsigned int argc, const char* const* argv) {
	const Account* account = NULL;
	if (player != NULL) {
		account = player->getAccount();
	}
	if (argc == 0) {
		// TODO Send this->usage()
		return 1;
	}
	for (unsigned int i = SUBCMD_CREATE; i < SUBCMD_CNT; i++) {
		if (subcommands[i].name == NULL) continue;
		if (strcmp(argv[0], subcommands[i].name) == 0) {
			if (subcommands[i].handle == NULL) {
				// TODO Send message to client
				return -3;
			}
			return subcommands[i].handle(account, argc-1, &argv[1]);
		}
	}
	// TODO Send this->usage()
	return 1;
}

static int handleCreate(const Account* account, unsigned int argc, const char* const* argv) {
	if (account != NULL) {
		// TODO Send message to client
		return -4;
	}
	if (argc < 1) {
		// TODO Send a usage string
		return 1;
	}
	Account* newAccount = globalDbGate->createAccount(argv[0]);
	if (newAccount == NULL) {
		return -1;
	}
	// TODO Password
	// TODO Reserved uid
	newAccount->saveToDb();
	delete newAccount;
	// TODO Send message to client
	return 0;
}

static int handleDelete(const Account* account, unsigned int argc, const char* const* argv) {
	const Account* targetAccount = NULL;
	if (argc < 1) {
		targetAccount = account;
	}
	else {
		targetAccount = globalDbGate->getAccountByUsername(argv[0]);
		if (targetAccount == NULL) {
			if (argv[0][0] == '@') {
				targetAccount = globalDbGate->getAccountByUsername(&argv[0][1]);
			}
		}
		if (targetAccount == NULL) {
			if (argv[0][0] == 'a') {
				targetAccount = globalDbGate->getAccountByAid(strtoul(&argv[0][1], NULL, 10));
			}
		}
		if (targetAccount == NULL) {
			if (argv[0][0] == 'u') {
				targetAccount = globalDbGate->getAccountByUid(strtoul(&argv[0][1], NULL, 10));
			}
		}
	}
	if (targetAccount != NULL) {
		int ret = globalDbGate->deleteAccount(*targetAccount);
		delete targetAccount;
		return ret;
	}
	return 0;
}

static int handleEdit(const Account* account, unsigned int argc, const char* const* argv) {
	return -10;
}

static int handlePasswd(const Account* account, unsigned int argc, const char* const* argv) {
	return -10;
}
