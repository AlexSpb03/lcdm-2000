// lcdm-2000.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "lcdm2000.h"

int main()
{
    Devices::lcdm2000 lcdm2000;
    lcdm2000.connect("/dev/ttyUSB0", B9600);

    try
    {
        lcdm2000.purge();
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        lcdm2000.status();
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        lcdm2000.upperDispense(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        lcdm2000.lowerDispense(2);
    }
    catch (Devices::lcdm2000::Exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}