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

struct TTY {

    TTY();
    virtual ~TTY();

    bool IsOK() const;

    void Connect(const string& port, int baudrate);
    void Disconnect();

    int Write(const vector<unsigned char>& data);
    void Read(vector<unsigned char>& data, int size = 200);

    int F_ID = -1;

};

struct TTYException {
};

#endif