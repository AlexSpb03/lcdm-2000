#ifndef LCDM_2000_H
#define LCDM_2000_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include "TTY.h"

typedef unsigned char cc_byte;
typedef unsigned short cc_short;
typedef std::vector<cc_byte> vec_bytes;

enum class LcdmCommands : cc_byte
{
    NAK = 0xFF,
    ACK = 0x06,
    PURGE = 0x44,
    STATUS = 0x46,
    UPPER_DISPENSE = 0x45,
    LOWER_DISPENSE = 0x55,
    UPPER_AND_LOWER_DISPENSE = 0x56,
    UPPER_TEST_DISPENSE = 0x76,
    LOWER_TEST_DISPENSE = 0x77
};

#define EXCEPTION_BAD_RESPONSE_CODE                 0
#define EXCEPTION_BAD_CRC_CODE                      1
#define EXCEPTION_BAD_SOH_CODE                      2
#define EXCEPTION_BAD_ID_CODE                       3
#define EXCEPTION_BAD_STX_CODE                      4
#define EXCEPTION_BAD_ACK_RESPONSE_CODE             5
#define EXCEPTION_BAD_COUNT                         6

class lcdm2000Exception : public std::exception
{
    std::string m_error;
    std::string str;
    int code;

public:
    lcdm2000Exception() : code(0), m_error("")
    {
        str = m_error + std::string(": ") + std::to_string(code);
    };

    lcdm2000Exception(std::string error) : m_error(error), code(0)
    {
        str = m_error + std::string(": ") + std::to_string(code);
    };

    lcdm2000Exception(std::string error, int _code) : code(_code), m_error(error)
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

class lcdm2000
{
    TTY tty;
    const cc_byte EOT = 0x04;
    const cc_byte ID = 0x50;
    const cc_byte STX = 0x02;
    const cc_byte ETX = 0x03;
    const cc_byte SOH = 0x01;
    const cc_byte ACK = 0x06;
    const cc_byte NCK = 0x15;

    cc_byte errorCode;
    std::string errorMessage;

public:
    lcdm2000();
    ~lcdm2000();

    cc_byte GetCRC(vec_bytes bufData, size_t size);
    bool checkErrors(cc_byte test);
    vec_bytes compileCommand(LcdmCommands cmd, vec_bytes data = {});
    int sendCommand(LcdmCommands cmd, vec_bytes data = {});
    cc_byte getACK();
    void sendACK();
    vec_bytes getResponse(int recv_bytes);
    vec_bytes go(LcdmCommands cmd, vec_bytes data = {}, int recv_bytes = 7);
    bool testCRC(vec_bytes bufData);
    void print_b(std::string msg, vec_bytes data);

    void connect(std::string com_port, int baudrate = 9600);

    void purge();
    void status();
    void upperDispense(int _count);
    void lowerDispense(int _count);
};

#endif // LCDM_2000_H
