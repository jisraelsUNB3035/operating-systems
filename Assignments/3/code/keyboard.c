#include <curses.h>
#include <sys/select.h>

#include "keyboard.h"
#include "player.h"
#include "centipede.h"
#include "threadwrappers.h"
#include "gameglobals.h"
#include "console.h"

#define KEY_TICKS 1

void* runKeyboard(void* data) {
	player* p = (player*)data;
	
	while(true) {
		char c;
		wrappedMutexLock(&screenMutex);
		if(kbhit()) {
			c = getch();
				switch(c) {
					case 'w':
						movePlayer(p, -1, 0);
						break;
					case 'a':
						movePlayer(p, 0, -1);
						break;
					case 's':
						movePlayer(p, 1, 0);
						break;
					case 'd':
						movePlayer(p, 0, 1);
						break;
					case 'q':
						wrappedMutexUnlock(&screenMutex);
						endGame();
			}
			flushinp();
		}
		wrappedMutexUnlock(&screenMutex);
		//sleepTicks(KEY_TICKS);
	}
}

int kbhit() {
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	return select(1, &fds, NULL, NULL, &tv) > 0;
}

void endGame() {
	wrappedMutexLock(&gameOverMutex);	
	wrappedCondSignal(&gameOverCond);
	gameOver = true;
	wrappedMutexUnlock(&gameOverMutex);
	pthread_exit(NULL);
}