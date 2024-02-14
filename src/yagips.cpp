/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/random.h>
#include <time.h>
#include <pthread.h>
#include "gameserver.h"
#include "dispatch.h"
#include "dbgate.h"
#include "config.h"
#include "data.h"
#include "ec2b.h"
#include "keys.h"

extern "C" {
	static int exitSignal = 0;
	static pthread_t dispatch, gameserver, parent;
	static struct sigaction SA_DFL;

	static void handleTerm(int sig) {
		static int timesCalled = 0;
		pthread_t tid = pthread_self();
		GameserverSignal = 1;
		DispatchServerSignal = 1;
		if (sig == SIGINT && pthread_equal(tid, parent)) {
			timesCalled++;
			if (timesCalled >= 3) {
				sigaction(sig, &SA_DFL, NULL);
				kill(getpid(), sig);
			}
		}
		exitSignal = sig;
	}

	int loadKeys(const char* basePath) {
		char keyPathBuf[4096];
		keyPathBuf[4095] = '\0';
		FILE* fp;
		snprintf(keyPathBuf, 4095, "%s/keys/dispatchSeed.bin", basePath);
		fp = fopen(keyPathBuf, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Can't open %s: %d (%s)\n", keyPathBuf, errno, strerror(errno));
		}
		else {
			Ec2b* ec2b = new Ec2b(fp);
			fclose(fp);
			memcpy(dispatchKey, ec2b->getXorpad().c_str(), 4096);
			dispatchXorSeed = ec2b->getSeed();
			hasDispatchSeed = 1;
			delete ec2b;
		}
		// TODO: Signing/auth rsa keys
		return 0;
	}

	int main(int argc, char** argv) {
		int ret = 0;
		int tret;
		struct sigaction termAction;
		sigset_t sigblock;
		struct sigaction prevAction;
		parent = pthread_self();
		memset(&SA_DFL, 0, sizeof(SA_DFL));
		SA_DFL.sa_handler = SIG_DFL;
		termAction.sa_handler = handleTerm;
		termAction.sa_flags = 0;
		sigemptyset(&sigblock);
		sigaddset(&sigblock, SIGINT);
		sigaddset(&sigblock, SIGQUIT);
		sigaddset(&sigblock, SIGTERM);
		sigaddset(&sigblock, SIGHUP);
		termAction.sa_mask = sigblock;
		sigaction(SIGINT, NULL, &prevAction);
		if (prevAction.sa_handler != SIG_IGN) {
			if (sigaction(SIGINT, &termAction, NULL) < 0) {
				fprintf(stderr, "Unable to set signal handler for %s, errno = %d (%s)\n", "SIGINT", errno, strerror(errno));
			}
		}
		sigaction(SIGTERM, NULL, &prevAction);
		if (prevAction.sa_handler != SIG_IGN) {
			if (sigaction(SIGTERM, &termAction, NULL) < 0) {
				fprintf(stderr, "Unable to set signal handler for %s, errno = %d (%s)\n", "SIGTERM", errno, strerror(errno));
			}
		}
		sigaction(SIGHUP, NULL, &prevAction);
		if (prevAction.sa_handler != SIG_IGN) {
			if (sigaction(SIGHUP, &termAction, NULL) < 0) {
				fprintf(stderr, "Unable to set signal handler for %s, errno = %d (%s)\n", "SIGHUP", errno, strerror(errno));
			}
		}
		// TODO Getopt
		// TODO Path from command line arg
		globalConfig = new Config();
		loadKeys(globalConfig->getConfig()->dataPath);
		globalGameData = new GameData();
		globalDbGate = new dbGate(globalConfig->getConfig()->dbPath);
		// TODO Thread attributes
		int terrno = pthread_create(&gameserver, NULL, GameserverMain, NULL);
		if (terrno) {
			fprintf(stderr, "Unable to create gameserver thread, errno = %d (%s)\n", terrno, strerror(terrno));
			delete globalDbGate;
			delete globalConfig;
			return -terrno;
		}
		terrno = pthread_create(&dispatch, NULL, DispatchServerMain, NULL);
		if (terrno) {
			fprintf(stderr, "Unable to create dispatch server thread, errno = %d (%s)\n", terrno, strerror(terrno));
			delete globalDbGate;
			delete globalConfig;
			return -terrno;
		}
		// TODO Server console
		terrno = pthread_join(gameserver, (void**) &tret);
		if (terrno) {
			fprintf(stderr, "Unable to watch the gameserver thread, errno = %d (%s)\n", terrno, strerror(terrno));
			delete globalDbGate;
			delete globalConfig;
			return -terrno;
		}
		if (tret) {
			fprintf(stderr, "Gameserver thread returned status %d\n", tret);
			ret = tret;
		}
		terrno = pthread_join(dispatch, (void**) &tret);
		if (terrno) {
			fprintf(stderr, "Unable to watch the dispatch server thread, errno = %d (%s)\n", terrno, strerror(terrno));
			delete globalDbGate;
			delete globalConfig;
			return -terrno;
		}
		if (tret) {
			fprintf(stderr, "Dispatch server thread returned status %d\n", tret);
			ret += tret;
		}
		delete globalDbGate;
		delete globalConfig;
		if (!(exitSignal == 0 || exitSignal == SIGINT)) {
			sigaction(exitSignal, &SA_DFL, NULL);
			kill(getpid(), exitSignal);
		}
		return ret;
	}
}
