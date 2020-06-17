#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "Header.h"

vizdoom::DoomGame* game = new vizdoom::DoomGame();
unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
std::string path = "C:\\practice\\practice\\vizdoom";
auto screenBuff = cv::Mat(480, 640, CV_8UC3);

void RunTask1(int episodes) {
	try {
		game->loadConfig(path + "\\scenarios\\task2.cfg");
		game->init();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::vector<double>actions;
	auto greyscale = cv::Mat(480, 640, CV_8UC3);
	uchar max = 150;


	for (auto i = 0; i < episodes; i++) {
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished()) {
			const auto& gamestate = game->getState();
			std::memcpy(screenBuff.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());
			cv::extractChannel(screenBuff, greyscale, 1);
			cv::threshold(greyscale, greyscale, 200, 255, cv::THRESH_BINARY);
			cv::Rect reg(320, 200, 100, 100);
			greyscale(reg).copyTo(greyscale);

			for (int i = 0; i < greyscale.rows; i++) {
				for (int j = 0; j < greyscale.rows; j++) {
					if(greyscale.at<uchar>(j, i) == 255) game->makeAction({ 0, 0, 0, 1 });
					//std::cout << greyscale.at<uchar>(1, 5) << std::endl;
					//greyscale.at<uchar>(j, i) = 255;
				}
			}

			double reward = game->makeAction({ 1, 0, 0, 0 });
			cv::imshow("Output", greyscale);

			//vizdoom::BufferPtr screenBuf = gamestate->screenBuffer;

			cv::waitKey(sleepTime);
		}
		std::cout << std::endl << game->getTotalReward() << std::endl;
	}
}

int main() {
	game->setViZDoomPath(path + "\\vizdoom.exe");
	game->setDoomGamePath(path + "\\freedoom2.wad");

	cv::namedWindow("Output", cv::WINDOW_AUTOSIZE);

	auto episodes = 10;
	RunTask1(episodes);

	game->close();
}
