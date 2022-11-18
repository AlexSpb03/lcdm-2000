#ifndef TTY_H
#define TTY_H

//#define NOMINMAX //����� API windows ��������� ������� min � max, ������������� � std::max � std::min � vector

#include <vector>
#include <string>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

struct TTY
{

    TTY();
    virtual ~TTY();

    bool IsOK() const;

    void Connect(const string &port, int baudrate);
    void Disconnect();

    int Write(const vector<unsigned char> &data);
    void Read(vector<unsigned char> &data, int size = 200);

    int F_ID = -1;

    class Exception : public std::exception
    {
        std::string m_error;
        std::string str;
        int code;

    public:
        Exception() : code(0), m_error("")
        {
            str = m_error + std::string(": ") + std::to_string(code);
        };

        Exception(std::string error) : m_error(error), code(0)
        {
            str = m_error + std::string(": ") + std::to_string(code);
        };

        Exception(std::string error, int _code) : code(_code), m_error(error)
        {
            str = m_error + std::string(": ") + std::to_string(code);
        };

        int getCode()
        {
            return code;
        }

        const char *what() const noexcept override
        {
            return str.c_str();
        }
    };
};

#endif