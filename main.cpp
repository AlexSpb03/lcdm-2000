// lcdm-2000.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <iomanip>
#include "lcdm2000.h"

int main()
{
    Devices::lcdm2000 lcdm2000;
    std::string port = "/dev/ttyUSB0";
    try
    {
        lcdm2000.connect(port, B9600);
    }
    catch (const TTYException &e)
    {
        std::cerr << "Cannot connect to port:" << port << std::endl;
        return -1;
    }

    try
    {
        // lcdm2000.purge();

        // std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        // lcdm2000.status();

        // std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        auto result = lcdm2000.upperLowerDispense(1, 0);

        //--- print result:
        //--- 0 - upper exit count
        //--- 1 - lower exit count
        //--- 2 - upper rejected count
        //--- 3 - lower rejected count
        //--- 4 - upper check count
        //--- 5 - lower check count
        std::cout << "=========== Result ===========" << std::endl
                  << std::setfill(' ')
                  << std::setw(20) << "upper exit: " << std::dec << static_cast<int>(result[0]) << std::endl
                  << std::setw(20) << "lower exit: " << std::dec << static_cast<int>(result[1]) << std::endl
                  << std::setw(20) << "upper rejected: " << std::dec << static_cast<int>(result[2]) << std::endl
                  << std::setw(20) << "lower rejected: " << std::dec << static_cast<int>(result[3]) << std::endl
                  << std::setw(20) << "upper check: " << std::dec << static_cast<int>(result[4]) << std::endl
                  << std::setw(20) << "lower check: " << std::dec << static_cast<int>(result[5]) << std::endl
                  << "============ End =============" << std::endl;

        // lcdm2000.upperDispense(1);
        // std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        // lcdm2000.lowerDispense(2);
    }
    catch (Devices::lcdm2000::Exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}