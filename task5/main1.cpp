#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>
#include <chrono>
#include <thread>
#include <Windows.h>

vizdoom::DoomGame* game = new vizdoom::DoomGame();
auto screenBuff = cv::Mat(480, 640, CV_8UC3);
const unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
double sum = 0;

void RunTask1(int episodes)
{
	try
	{
		game->loadConfig("../vizdoom/scenarios/task5.cfg");
		game->init();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	auto greyscale = cv::Mat(480, 640, CV_8UC1);
	auto walls = cv::Mat(480, 640, CV_8UC1);

	int* run = new int[4];
	unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
	for (auto i = 0; i < episodes; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;
		while (!game->isEpisodeFinished()) {
			for (int k = 0; k < 4; k++) run[k] = 0;

			const auto& gamestate = game->getState();

			std::memcpy(screenBuff.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());

			cv::extractChannel(screenBuff, greyscale, 0);
			cv::threshold(greyscale, greyscale, 200, 255, cv::THRESH_BINARY);
			//cv::extractChannel(screenBuff, walls, 0);


			for (int k = 0; k < 640; k++)
			{
				if ((int)greyscale.at<uchar>(cv::Point(k, 220)) == 255)
				{
					if (i <= 160)
					{
						run[0] = 1;
						//game->makeAction({ 0, 1 });
					}
					if ((i > 160) && (i <= 320))
					{
						run[1] = 11;
						//game->makeAction({ 0, 1 });
					}
					if ((i > 320) && (i <= 480))
					{
						run[2] = 2;
						//game->makeAction({ 1, 0 });
					}
					if ((i > 480) && (i <= 640))
					{
						run[2] = 22;
						//game->makeAction({ 1, 0 });
					}
					break;
				}
			}

			if (run[0] == 1) game->makeAction({ 0, 1 });
			if (run[1] == 11) game->makeAction({ 1, 0 });
			if (run[0] == 2) game->makeAction({ 0, 1 });
			if (run[1] == 22) game->makeAction({ 1, 0 });

			//for(int )

			game->makeAction({ 0, 0 });
			cv::imshow("Output Window", greyscale);
			cv::waitKey(1);
			cv::threshold(greyscale, walls, 150, 255, cv::THRESH_BINARY);
		}

		sum += game->getTotalReward();
		std::cout << std::endl << game->getTotalReward() << std::endl;

	}
}

int main() {
	game->setViZDoomPath("../vizdoom/vizdoom");
	game->setDoomGamePath("../vizdoom/freedoom2.wad");

	//cv::namedWindow("Output Window", cv::WINDOW_AUTOSIZE);

	auto episodes = 10;

	//------------------
	RunTask1(episodes);
	//------------------

	std::cout << sum / 10;

	game->close();
	delete game;
}
