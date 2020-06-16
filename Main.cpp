#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>
#include <chrono>
#include <thread>

using namespace vizdoom;

DoomGame* game = new DoomGame();
const std::vector<double> actions[3] = {
	{ 1, 0, 0 }, // left
	{ 0, 1, 0 }, // right
	{ 0, 0, 1 }, // shoot
};

void sleep(unsigned int time) {
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

void game_init() {
	game->setViZDoomPath("../vizdoom/vizdoom");
	game->setDoomGamePath("../vizdoom/freedoom2.wad");
	game->loadConfig("../vizdoom/scenarios/basic.cfg");
	game->setScreenResolution(RES_640X480);
	game->setLabelsBufferEnabled(true); // add this
	game->setWindowVisible(true);
	game->setRenderWeapon(true);
	game->setRenderHud(true);
	game->init();
}

void find_demon_and_kill(GameStatePtr state) {
	double eps = 10; // monster's width
	if (state->labels[0].objectPositionY - eps > state->labels[1].objectPositionY) {
		game->makeAction(actions[0]); //left
	}
	else if (state->labels[0].objectPositionY + eps < state->labels[1].objectPositionY) {
		game->makeAction(actions[1]); //right
	}
	else {
		game->makeAction(actions[2]); // shoot
	}
}

int main() {
	game_init();

	int episodes = 10;
	unsigned int sleepTime = 2000 / DEFAULT_TICRATE;
	for (int i = 0; i < episodes; ++i) {
		std::cout << "Episode #" << i + 1 << "\n";
		game->newEpisode();
		while (!game->isEpisodeFinished()) {
			auto state = game->getState();
			auto screenBuf = state->screenBuffer;

			find_demon_and_kill(state);
			sleep(sleepTime);
		}
		sleep(sleepTime * 10);
		std::cout << "Total reward is: " << game->getTotalReward() << std::endl;
	}
	game->close();
	delete game;
}