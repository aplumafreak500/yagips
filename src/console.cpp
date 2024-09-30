/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#ifdef HAS_READLINE
#include <readline/readline.h>
#endif
#include "player.h"
#include "command.h"

static unsigned int console_signal = 0;

void stop_console() {
	console_signal = 1;
}

int console_main() {
	int cmdret;
	char* cmd = NULL;
#ifdef HAS_READLINE
	rl_bind_key('\t', rl_insert);
#else
	char* buf = NULL;
	ssize_t len = 0;
	size_t buflen = 0;
#endif
	while(!console_signal) {
#ifdef HAS_READLINE
		if (cmd != NULL) {
			free(cmd);
			cmd = NULL;
		}
		static char prompt[32];
		prompt[31] = '\0';
		snprintf(prompt, 31, "%s> ", PACKAGE_NAME);
		cmd = readline(prompt);
		if (cmd == NULL) {
			break;
		}
#else
		// Only print a prompt if both stdin and stdout are connected to a tty.
		if (isatty(fileno(stdin)) && isatty(fileno(stdout))) {
			printf("\r%s> ", PACKAGE_NAME);
		}
		len = getline(&buf, &buflen, stdin);
		if (len < 0) {
			if (feof(stdin)) {
				break;
			}
			if (ferror(stdin)) {
				fprintf(stderr, "Warning: getline returned error %d (%s)\n", errno, strerror(errno));
				// TODO is it really safe to just keep chugging along here?
				continue;
			}
		}
		// Strip away the newline at the end.
		buf[len - 1] = '\0';
		cmd = buf;
#endif
		if (strlen(cmd) == 0) continue;
		// Call the command parser (setting player to NULL to indicate a command ran from the console)
		cmdret = handleCommand(NULL, cmd);
		if (cmdret < 0) {
			fprintf(stderr, "Warning: Command `%s` failed with error %d\n", cmd, cmdret);
		}
		else if (cmdret > 0) {
			fprintf(stderr, "Warning: Command `%s` returned nonzero status %d\n", cmd, cmdret);
		}
		else {
			fprintf(stderr, "Command `%s` ran successfully.\n", cmd);
		}
	}
#ifndef HAS_READLINE
	buf = NULL;
#endif
	if (cmd != NULL) {
		free(cmd);
		cmd = NULL;
	}
	return 0;
}
