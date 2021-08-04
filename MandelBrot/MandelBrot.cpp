#include "MandelBrot.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "MessageQueue.h"

using namespace cv;
MandelBrot::MandelBrot(std::string filename, std::string windowname):_windowName(windowname),_dataPath(filename)

{
    centerX = DefaultCenterX;
    centerY = DefaultCenterY;
    pixmapScale = DefaultScale;
    curScale = DefaultScale;
    namedWindow(_windowName, cv::WINDOW_NORMAL);
    createBGImage(true);
    setMouseCallback (_windowName,CallBackFunc, this);
    UI_Thread = std::thread ([this](){this->UI();});
    Wait_Thread = std::thread ([this](){this->WaitForImage();});

}

MandelBrot::~MandelBrot(){

    if(UI_Thread.joinable())
        UI_Thread.join();
    if(Wait_Thread.joinable())
        Wait_Thread.join();
    destroyWindow(_windowName);
}

void MandelBrot::createBGImage(bool displayhelp){

    std::ifstream file(_dataPath+"/help.txt");
    cv::Mat background(600, 800, CV_8UC3,Scalar(255,255,255));
    cv::Mat textImg = Mat::zeros(600, 800, background.type());

    int i =10,j=10;
    if(displayhelp){
        if (file)
        {
            std::string lineStr;
            while (getline(file, lineStr)) // TODO limit the no of lines.
            {
                if (lineStr.size() > 0)
                {
                    cv::putText(textImg,
                                lineStr,
                                cv::Point(i,j),
                                cv::FONT_HERSHEY_SIMPLEX,
                                0.4,
                                CV_RGB(255, 255, 255),
                                1);
                    j+=11;

                }
            }
        }
        else {
            cv::putText(background,
                        "Help file not found",
                        cv::Point(10, 10),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.4,
                        CV_RGB(255, 255, 255),
                        1);
        }
    }

    if(_images.size()==0){  //initial creation
        _images.push_back(background);
        _images.push_back(textImg);

    }else{
        _images[0]=background;
        _images[1]=textImg;

    }

    RenderClass.RenderMandelBrot(centerX, centerY, curScale, _images[0].size());
}

void MandelBrot::saveImageToFile(){
    //create a file name with time stamp
    auto time = std::time(nullptr);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%F_%T");
    auto s = ss.str();
    std::replace(s.begin(), s.end(), ':', '-');
    std::string path =_dataPath+"/MandelBrot"+s+".jpg";
    std::unique_lock<std::mutex> uLock(_mutex);
    bool check = imwrite(path, _images[0]);
    uLock.unlock();

    if(check){ //TODO message file write sucees
    }else{
        // TODO handle write failure;
    }

}
void MandelBrot::WaitForImage(){
    std::unique_lock<std::mutex> uLock(_mutex);
    uLock.unlock();

    while(!quit){

        cv::Mat img(MQ.receive());
        //cv::Mat img (600, 800, CV_8UC3,Scalar(0,0,0));
        uLock.lock();
        _images[0]=img;
        uLock.unlock();

    }

    // destroyWindow(_windowName);
}

void MandelBrot::UI()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    uLock.unlock();

    while(!quit){

        uLock.lock();
        if(showHelp)
            imshow(_windowName, _images[0]+_images[1] );
        else
            imshow(_windowName, _images[0]);
        uLock.unlock();

        switch(waitKey(100)){

        case 'q' :
            RenderClass.abortRender();
            quit=true;
            break;
        case '+' :
            zoom(ZoomInFactor);
            break;
        case '-':
            zoom(ZoomOutFactor);
            break;
        case 0x52: //Up Arrow
            scroll(0, +ScrollStep);
            break;
        case 0x54 : //Down Arrow
            scroll(0, -ScrollStep);
            break;
        case 0x51: //Left Arrow
            scroll(-ScrollStep, 0);
            break;
        case 0x53 : //Right Arrow
            scroll(+ScrollStep, 0);
            break;
        case 'h':
            //createBGImage(true);
            showHelp=true;
            break;
        case 'c':
            //createBGImage(false);
            showHelp=false;
            break;
        case 'p':
            saveImageToFile();
            break;
        default:
            break;
        }
    }

 }

void MandelBrot::CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    MandelBrot *thisMandelBrot =static_cast<MandelBrot *>(userdata);

    if (event ==EVENT_LBUTTONDOWN) {
      //TODO

    }
    else if (event == EVENT_MOUSEMOVE) {
      //TODO

    }
    else if(event==EVENT_LBUTTONUP){
     //TODO

    }
    else if (event==EVENT_MOUSEHWHEEL){
     //TODO


    }
}

void MandelBrot::zoom(double zoomFactor)
{

    curScale *= zoomFactor;
    RenderClass.RenderMandelBrot(centerX, centerY, curScale, _images[0].size());

}

void MandelBrot::scroll(int deltaX, int deltaY)
{
   // std::cout<<"scroll";
    centerX += deltaX * curScale;
    centerY += deltaY * curScale;
    RenderClass.RenderMandelBrot(centerX, centerY, curScale, _images[0].size());

}
