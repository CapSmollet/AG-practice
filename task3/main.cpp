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
		game->loadConfig(path + "\\scenarios\\task3.cfg");
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
		std::string filename = "C:\\practice\\practice\\vizdoom\\sprites\\Pickups\\media0.png";
		game->newEpisode();
		//std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished()) {
			int per = 0, run = 0;
			const auto& gamestate = game->getState();
			std::memcpy(screenBuff.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());

			cv::extractChannel(screenBuff, greyscale, 1);
			cv::threshold(greyscale, greyscale, 200, 255, cv::THRESH_BINARY);

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

			if(run == 1) game->makeAction({ 0, 0, 0, 1 });
			else {
				if (per == 1) game->makeAction({ 1, 0, 0, 0 });
				else game->makeAction({ 0, 1, 0, 0 });
			}
			//std::cout << per << std::endl;

			/*cv::imshow("Left", left);
			cv::imshow("Right", right);
			cv::imshow("Result", greyscale);*/
			//cv::waitKey(1);
		}

		/*while (!game->isEpisodeFinished())
		{
			int per = 0;
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
				per = 1;
				//game->makeAction({ 1, 0, 1, 0 });
			}
			else if (minLoc.x + templ.cols > 320)
			{
				per = 0;
				//game->makeAction({ 0, 1, 1, 0 });
			}

			if(per == 1) game->makeAction({ 1, 0, 1, 0 });
			else game->makeAction({ 0, 1, 1, 0 });
			//else if ()
			//game->makeAction({ 0, 0, 0, 1 });

			cv::imshow("Origin", img);
			cv::imshow("Result", result);
			//double reward = game->makeAction({1});
			cv::waitKey(sleepTime);
		}*/
		//Sleep(500);
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
