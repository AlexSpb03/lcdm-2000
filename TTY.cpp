#include "TTY.h"
#include <iostream>
#include <assert.h>

using namespace std;

TTY::TTY()
{
    F_ID = -1;
}

TTY::~TTY()
{
    Disconnect();
}

bool TTY::IsOK() const
{
    return F_ID != -1;
}

void TTY::Connect(const string &port, int baudrate)
{

    Disconnect();

    F_ID = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (F_ID == -1)
    {
        char *errmsg = strerror(errno);
        printf("%s\n", errmsg);
        throw TTYException();
    }
    struct termios options;    /*структура для установки порта*/
    tcgetattr(F_ID, &options); /*читает пораметры порта*/

    cfsetispeed(&options, baudrate); /*установка скорости порта*/
    cfsetospeed(&options, baudrate); /*установка скорости порта*/

    options.c_cc[VTIME] = 1; /*Время ожидания байта 20*0.1 = 2 секунды */
    options.c_cc[VMIN] = 0;  /*минимальное число байт для чтения*/

    options.c_cflag &= ~PARENB; /*бит четности не используется*/
    options.c_cflag &= ~CSTOPB; /*1 стоп бит */
    options.c_cflag &= ~CSIZE;  /*Размер байта*/
    options.c_cflag |= CS8;     /*8 бит*/

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_cflag |= CREAD | CLOCAL;
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    options.c_lflag = 0;
    options.c_oflag &= ~OPOST; /*Обязательно отключить постобработку*/

    tcsetattr(F_ID, TCSANOW, &options); /*сохронения параметров порта*/
}

void TTY::Disconnect()
{

    if (F_ID != -1)
    {
        close(F_ID);
        F_ID = -1;
    }
}

int TTY::Write(const vector<unsigned char> &data)
{

    if (F_ID == -1)
    {
        throw TTYException();
    }

    int n = write(F_ID, &data[0], data.size());
    if (n == -1)
    {
        char *errmsg = strerror(errno);
        printf("%s\n", errmsg);
        throw TTYException();
    }
    return n;
}

void TTY::Read(vector<unsigned char> &data, int size)
{
    int timeout_sec = 60;
    data.clear();
    if (F_ID == -1)
    {
        throw TTYException();
    }

    unsigned char *buf = new unsigned char[size];
    int len = size;

    struct pollfd fds;
    fds.fd = F_ID;
    fds.events = POLLIN;

    //-- count attempts for read data. each byte two times
    int attempt = size << 1;

    for (;;)
    {
        if (!attempt)
            break;
        //--- wait ready read bytes
        poll(&fds, 1, timeout_sec * 1000);

        //--- if ready to read
        if (fds.revents & POLLIN)
        {
            //--- read len bytes
            int n = read(F_ID, buf, len);
            if (n == -1)
            {
                char *errmsg = strerror(errno);
                printf("%s\n", errmsg);
                break;
            }
            len -= n;

            //--- add read bytes to result
            for (int i{0}; i < n; i++)
            {
                data.push_back(buf[i]);
            }

            //--- if read all bytes -> return
            if (len <= 0)
            {
                break;
            }
        }
        attempt--;
    }
}