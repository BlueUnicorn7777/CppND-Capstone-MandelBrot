#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "MessageQueue.h"



class Render
{
public:

    Render(MessageQueue<cv::Mat> *MQ);
    ~Render();
    void abortRender();
    void RenderMandelBrot(double centerX, double centerY, double scaleFactor,
                              cv::Size resultSize);

    void runforever();

private:

    void rgbFromWaveLength(double wave, cv::Vec3b &point);

    bool restart = false;
    bool abort = false;
    bool running = false;

    std::mutex _mutex;
    std::condition_variable _condition;

    cv::Size resultSize;

    double centerX;
    double centerY;
    double scaleFactor;
    MessageQueue<cv::Mat> *MQ;
    std::thread RenderThread;
    enum { ColormapSize = 512 };
    cv::Vec3b colormap[ColormapSize];
};

#endif // RENDER_H
