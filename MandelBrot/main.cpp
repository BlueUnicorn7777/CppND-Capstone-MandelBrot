#include <iostream>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "MandelBrot.h"

using namespace std;
int main(int argc, char* argv[]) {

    std::string datapath = "../data";

    MandelBrot MB(datapath,"MandelBrot Image");

    //MB.show();

    return 0;
}
