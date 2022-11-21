# LCDM-2000

Класс c++ для работы с диспенсером купюр PULOON LCDM-2000.

Компиляция:

```bash
g++ -g main.cpp lcdm2000.cpp -o main
```

Пример
```c++
int main()
{
    Devices::Clcdm2000 lcdm2000;
    std::string port = "/dev/ttyUSB0";
    try
    {
        lcdm2000.connect(port, B9600);
    }
    catch (const Devices::Clcdm2000::Exception &e)
    {
        std::cerr << "Cannot connect to port:" << port << std::endl;
        return -1;
    }

    try
    {

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
    }
    catch (Devices::Clcdm2000::Exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
```
