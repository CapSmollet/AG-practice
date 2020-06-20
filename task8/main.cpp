#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>

vizdoom::DoomGame* game = new vizdoom::DoomGame();
unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
std::string path = "C:\\practice\\practice\\vizdoom";
auto screenBuff = cv::Mat(480, 640, CV_8UC3);
const int EPS = 20;

void RunTask1(int episodes) {
	double reward = 0;
	try {
		game->loadConfig(path + "\\scenarios\\task8.cfg");
		game->init();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::vector<double>actions;
	auto greyscale = cv::Mat(480, 640, CV_8UC3);
	auto left = cv::Mat(480, 640, CV_8UC3);
	auto right = cv::Mat(480, 640, CV_8UC3);
	uchar max = 150;


	for (auto i = 0; i < episodes; i++) {
		//std::string filename = "C:\\practice\\practice\\vizdoom\\sprites\\Pickups\\media0.png";
		game->newEpisode();
		//std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished()) {
			int per = 0, run = 0;
			const auto& gamestate = game->getState();
			std::memcpy(screenBuff.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());

			cv::extractChannel(screenBuff, greyscale, 0);
			cv::threshold(greyscale, greyscale, 150, 255, cv::THRESH_BINARY);

			cv::Rect regl(0, 0, 320, 400);
			greyscale(regl).copyTo(left);
			cv::Rect reg2(320, 0, 320, 400);
			greyscale(reg2).copyTo(right);
			cv::Rect reg(310, 0, 20, 400);
			greyscale(reg).copyTo(greyscale);


			for (int i = 0; i < greyscale.rows; i++) {
				for (int j = 0; j < greyscale.cols; j++) {
					if (greyscale.at<uchar>(i, j) == 255) run = 1;
				}
			}

			for (int i = 0; i < left.rows; i++) {
				for (int j = 0; j < left.cols; j++) {
					if (left.at<uchar>(i, j) == 255) per = 1;
				}
			}

			if (run == 1) game->makeAction({ 0, 0, 1 });
			else {
				if (per == 1) game->makeAction({ 0, 0, 0 });
				else game->makeAction({ 0, 0, 0 });
			}
			//std::cout << per << std::endl;

			cv::imshow("Left", left);
			cv::imshow("Right", right);
			cv::imshow("Result", greyscale);
			cv::waitKey(1);
		}

		std::cout << std::endl << game->getTotalReward() << std::endl;
		reward = reward + game->getTotalReward();
	}
	std::cout << "TotalRew " << reward;
}

int main() {
	game->setViZDoomPath(path + "\\vizdoom.exe");
	game->setDoomGamePath(path + "\\freedoom2.wad");

	//cv::namedWindow("Output", cv::WINDOW_AUTOSIZE);

	auto episodes = 10;
	RunTask1(episodes);

	//game->close();
}
