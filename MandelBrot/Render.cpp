#include <iostream>
#include "Render.h"

using namespace cv;


Render::Render(MessageQueue<cv::Mat> *MQ)
{
     for (int i = 0; i < ColormapSize; ++i)
     rgbFromWaveLength(380.0 + (i * 400.0 / ColormapSize),colormap[i]);

     this->MQ=MQ;
}

Render::~Render()
{
    _mutex.lock();
    abort = true;
    _condition.notify_one();
    _mutex.unlock();
    if(RenderThread.joinable())
    RenderThread.join();
}

void Render::abortRender()
{
    _mutex.lock();
    abort = true;
    _condition.notify_one();
    _mutex.unlock();

}

void Render::RenderMandelBrot(double centerX, double centerY, double scaleFactor,
                          cv::Size resultSize)
{
    std::unique_lock<std::mutex> locker(_mutex);

    this->centerX = centerX;
    this->centerY = centerY;
    this->scaleFactor = scaleFactor;
    this->resultSize= resultSize;

    if (!running) {
    RenderThread = std::thread ([this](){this->runforever();});
    running=true;
    } else {
        restart = true;
       _condition.notify_one();
    }
}

void Render::runforever()

{



   while(true){

    _mutex.lock();
    cv::Size resultSize = this->resultSize;
    double scaleFactor = this->scaleFactor;
    double centerX = this->centerX;
    double centerY = this->centerY;
    _mutex.unlock();

    int halfWidth = resultSize.width/2 ;
    int halfHeight = resultSize.height/2;

    const int NumPasses = 8;
    int pass = 0;
    cv::Vec3b pixelBlack(0,0,0);

    while (pass < NumPasses) {
        cv::Mat image(600, 800, CV_8UC3,Scalar(0,0,0));
        const int MaxIterations = (1 << (2 * pass + 6)) + 32;
        const int Limit = 4;
        bool allBlack = true;

        for (int y = -halfHeight; y < halfHeight; ++y) {
            if (restart) break;
            if (abort) {MQ->send(std::move(image));return;}

            double ay = centerY + (y * scaleFactor);

            for (int x = -halfWidth; x < halfWidth; ++x) {

                double ax = centerX + (x * scaleFactor);
                double a1 = ax;
                double b1 = ay;
                int numIterations = 0;

                do {
                    ++numIterations;
                    double a2 = (a1 * a1) - (b1 * b1) + ax;
                    double b2 = (2 * a1 * b1) + ay;
                    if ((a2 * a2) + (b2 * b2) > Limit)
                        break;

                    ++numIterations;
                    a1 = (a2 * a2) - (b2 * b2) + ax;
                    b1 = (2 * a2 * b2) + ay;
                    if ((a1 * a1) + (b1 * b1) > Limit)
                        break;
                } while (numIterations < MaxIterations);


                if (numIterations < MaxIterations) {

                   image.at<Vec3b>(Point(x+halfWidth,y+halfHeight)) = colormap[(numIterations) % ColormapSize];

                    allBlack = false;
                } else {

                    image.at<Vec3b>(Point(x+halfWidth,y+halfHeight)) = pixelBlack;

                }
            }
        }

        if (allBlack && pass == 0) {
            pass = 4;
        } else {
            if (!restart)

                std::cout<<pass<<"image sent\n";
                MQ->send(std::move(image));
                ++pass;
        }
    }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::cout<<pass<<"thread sleeping\n";
        std::unique_lock<std::mutex> uLock(_mutex);
        if (!restart)
        _condition.wait(uLock);
        restart = false;

   }

}

void Render::rgbFromWaveLength(double wave , Vec3b &point)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
        r = 1.0;
    }

    double s = 1.0;
    if (wave > 700.0)
        s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    else if (wave <  420.0)
        s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);

    r = std::pow(r * s, 0.8);
    g = std::pow(g * s, 0.8);
    b = std::pow(b * s, 0.8);
    point[0]=int(r * 255);
    point[1]=int(g * 255);
    point[2]=int(b * 255);

}

