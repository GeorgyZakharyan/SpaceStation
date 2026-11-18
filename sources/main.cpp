#include <iostream>
#include "Lab1Demo.h"
#include "Lab2Demo.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    runLab1Demo();
    runLab1Tests();

    runLab2Demo();
    runLab2Tests();

    std::cout << std::endl << "Конец программы" << std::endl;
    return 0;
}
