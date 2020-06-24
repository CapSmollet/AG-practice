#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>
#include <chrono>
#include <thread>

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

	//std::vector<double> action;

	unsigned int sleepTime = 2000 / vizdoom::DEFAULT_TICRATE;
	for (auto i = 0; i < episodes; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;
		while (!game->isEpisodeFinished())
		{
			const auto& gamestate = game->getState();

			std::memcpy(screenBuff.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());

			cv::extractChannel(screenBuff, greyscale, 2);

		cv:threshold(greyscale, greyscale, 200, 255, cv::THRESH_BINARY);


                        //Надо боковые допилить
			for (int i = 0; i < 640; i++)
			{
				if ((int)greyscale.at<uchar>(cv::Point(i, 220)) == 255)
				{
					if (i <= 320)
					{
						game->makeAction({ 0, 1 });
						break;
					}
					if (i > 320)
					{
						game->makeAction({ 1, 0 });
						break;
					}
				}
			}

			game->makeAction({ 0, 0 });
			cv::imshow("Output Window", greyscale);
			cv::waitKey(1);
		}

		sum += game->getTotalReward();
		std::cout << std::endl << game->getTotalReward() << std::endl;

	}
}

int main() {
	game->setViZDoomPath("../vizdoom/vizdoom");
	game->setDoomGamePath("../vizdoom/freedoom2.wad");

	cv::namedWindow("Output Window", cv::WINDOW_AUTOSIZE);

	auto episodes = 10;

	//------------------
	RunTask1(episodes);
	//------------------

	std::cout << sum / 10;

	game->close();
	delete game;
}
