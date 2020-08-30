#include <utils/console_progress_bar.hpp>

#include <iostream>

using namespace kvk1920::utils;
//using namespace std;

#include <windows.h>

int main()
{
    std::unique_ptr<IProgressBar> pb{new ConsoleProgressBar(std::wcout)};
    pb->setMaxProgress(100, true);
    Sleep(1000);
    pb->reportJob(17);
    Sleep(1000);
    pb->reportJob(3);
}