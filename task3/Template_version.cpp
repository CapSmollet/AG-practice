#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>
#include <chrono>
#include <thread>
#include <stdlib.h>

#define CV_BLUR_NO_SCALE 0
#define CV_BLUR  1
#define CV_GAUSSIAN  2
#define CV_MEDIAN 3
#define CV_BILATERAL 4

vizdoom::DoomGame* game = new vizdoom::DoomGame();
auto screenBuff = cv::Mat(480, 640, CV_8UC3);
const unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
const int EPS = 100;
double sum = 0;

void RunTask1(int episodes)
{
	try
	{
		game->loadConfig("../vizdoom/scenarios/task3.cfg");
		game->init();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	auto greyscale = cv::Mat(480, 640, CV_8UC1);

	std::vector<double> actions[4];
	actions[0] = { 1, 0, 0, 0 };
	actions[1] = { 0, 1, 0, 0 };
	actions[2] = { 0, 0, 1, 0 };
	actions[3] = { 0, 0, 0, 1 };

	unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
	for (auto i = 0; i < episodes; i++)
	{

		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;
		while (!game->isEpisodeFinished())
		{
			const auto& gamestate = game->getState();

			std::memcpy(screenBuff.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());

			cv::Mat img = screenBuff;

			cv::Mat templ = cv::imread("../vizdoom/sprites/Pickups/media0.png");

			cv::Mat result;

			int result_cols = img.cols - templ.cols + 1;
			int result_rows = img.rows - templ.rows + 1;

			result.create(result_rows, result_cols, CV_32FC1);
			matchTemplate(img, templ, result, 0);
			normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

			double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
			cv::Rect rec(0, 0, 600, 400);
			img(rec).copyTo(img);
			result(rec).copyTo(result);
			rectangle(img, minLoc, cv::Point(minLoc.x + templ.cols, minLoc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
			rectangle(result, minLoc, cv::Point(minLoc.x + templ.cols, minLoc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
			if (minLoc.x + templ.cols < 320 + EPS && minLoc.x + templ.cols > 320 - EPS)
			{
				game->makeAction({ 0, 0, 0, 1 });

			}
			else if (minLoc.x + templ.cols < 320)
			{
				game->makeAction({ 1, 0, 1, 0 });
			}
			else if (minLoc.x + templ.cols > 320)
			{
				game->makeAction({ 0, 1, 1, 0 });
			}
	
			//else if ()
			//game->makeAction({ 0, 0, 0, 1 });

			cv::imshow("Origin", img);
			cv::imshow("Result", result);
			//double reward = game->makeAction({1});
			cv::waitKey(sleepTime);
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
