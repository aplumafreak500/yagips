/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <sys/random.h>
#include <time.h>
#include <pthread.h>
#include "gameserver.h"
#include "dispatch.h"

extern "C" {
	int main(int argc, char** argv) {
		pthread_t dispatch, gameserver;
		int ret = 0;
		int tret;
		// TODO Thread attributes
		// TODO Getopt
		// TODO Set up signal handlers
		int terrno = pthread_create(&gameserver, NULL, GameserverMain, NULL);
		if (terrno) {
			fprintf(stderr, "Unable to create gameserver thread, errno = %d (%s)\n", terrno, strerror(terrno));
			return -terrno;
		}
		terrno = pthread_create(&dispatch, NULL, DispatchServerMain, NULL);
		if (terrno) {
			fprintf(stderr, "Unable to create dispatch server thread, errno = %d (%s)\n", terrno, strerror(terrno));
			return -terrno;
		}
		terrno = pthread_join(gameserver, (void**) &tret);
		if (terrno) {
			fprintf(stderr, "Unable to watch the gameserver thread, errno = %d (%s)\n", terrno, strerror(terrno));
			return -terrno;
		}
		if (tret) {
			fprintf(stderr, "Gameserver thread returned status %d\n", tret);
			ret = tret;
		}
		DispatchServerSignal = 1; // TODO enum constant
		// TODO it may segfault here
		terrno = pthread_join(dispatch, (void**) &tret);
		if (terrno) {
			fprintf(stderr, "Unable to watch the dispatch server thread, errno = %d (%s)\n", terrno, strerror(terrno));
			return -terrno;
		}
		if (tret) {
			fprintf(stderr, "Dispatch server thread returned status %d\n", tret);
			ret += tret;
		}
		return ret;
	}
}
