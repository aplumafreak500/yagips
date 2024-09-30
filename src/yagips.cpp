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
#include <execinfo.h>
#include <stdexcept>
#include "gameserver.h"
#include "dispatch.h"
#include "dbgate.h"
#include "runconfig.h"
#include "data.h"
#include "ec2b.h"
#include "keys.h"
#include "console.h"
#include "command.h"

extern "C" {
	static int exitSignal = 0;
	static pthread_t dispatch, gameserver, parent;
	static struct sigaction SA_DFL;

	// TODO Once log files are implemented, upon receiving a SIGHUP, just close and/or dup2 the log files over stdio then continue, instead of terminating
	static void handleTerm(int sig) {
		static int timesCalled = 0;
		pthread_t tid = pthread_self();
		GameserverSignal = 1;
		DispatchServerSignal = 1;
		stop_console();
		if (sig == SIGINT && pthread_equal(tid, parent)) {
			timesCalled++;
			if (timesCalled >= 3) {
				sigaction(sig, &SA_DFL, NULL);
				kill(getpid(), sig);
			}
		}
		exitSignal = sig;
	}

	static void fatal_handler(int sig) {
		void* array[16];
		size_t sz = backtrace(array, 16);
		fprintf(stderr, "Error: uncaught signal %d:\n", sig);
		backtrace_symbols_fd(array, sz, 2);
		exit(sig | 192);
	}

	// TODO SIGUSR1, if sent, should trigger a config reload

	// TODO maybe consider moving into dispatch.cpp and/or session.cpp
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
		signal(SIGSEGV, fatal_handler);
		signal(SIGBUS, fatal_handler);
		signal(SIGFPE, fatal_handler);
		signal(SIGABRT, fatal_handler);
		// TODO Getopt
		// TODO Path from command line arg
		globalConfig = new Config();
		loadKeys(globalConfig->getConfig()->dataPath);
		globalGameData = new GameData();
		try {
			globalDbGate = new dbGate(globalConfig->getConfig()->dbPath, globalConfig->getConfig()->ldbPath);
		}
		catch(const std::exception& e) {
			fprintf(stderr, "Unable to create db handle: %s\n", e.what());
			delete globalConfig;
			return -1;
		}
		catch(...) {
			fprintf(stderr, "Unable to create db handle: %s\n", "Unknown error");
			delete globalConfig;
			return -1;
		}
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
		registerDefaultCommands();
		// TODO Config option to disable the server console
		// Note: Despite using tret, the server console runs in the main thread.
		tret = console_main();
		if (tret < 0) {
			fprintf(stderr, "Server console thread returned status %d\n", tret);
		}
		GameserverSignal = 1;
		DispatchServerSignal = 1;
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
		printf("\n");
		return ret;
	}
}
