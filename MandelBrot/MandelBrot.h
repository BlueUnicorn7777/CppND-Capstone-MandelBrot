#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include "Render.h"
#include <mutex>
#include <deque>
#include <condition_variable>
#include "MessageQueue.h"
const double DefaultCenterX = -0.637011f;
const double DefaultCenterY = -0.0395159f;
const double DefaultScale = 0.00403897f;
const double ZoomInFactor = 0.8f;
const double ZoomOutFactor = 1 / ZoomInFactor;
const int ScrollStep = 20;

using namespace cv;


class MandelBrot
{

public:

      MandelBrot(std::string filename,std::string windowname);
      ~MandelBrot();

protected:

private:
      struct Point{
          int x;
          int y;
      };

    void WaitForImage();
    MessageQueue<cv::Mat> Rendering_finished;

    String _windowName;
    double centerX;
    double centerY;
    double pixmapScale;
    double curScale;

    void UI();
    std::thread UI_Thread;
    std::thread Wait_Thread;
    void zoom(double zoomFactor);
    void scroll(int deltaX, int deltaY);

    static void CallBackFunc(int event, int x, int y, int flags, void* userdata);

    void createBGImage(bool displayhelp);

    void saveImageToFile();

    std::string _dataPath;
    std::vector<cv::Mat> _images;
    MessageQueue<cv::Mat> MQ;
    std::mutex _mutex;

    Render RenderClass=Render(&MQ);
    bool quit=false;
    bool showHelp=true;
    Point lastDragPos;

};

#endif // MANDELBROT_H
