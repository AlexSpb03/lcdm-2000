#ifndef LCDM_2000_H
#define LCDM_2000_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>

namespace Devices
{

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

#define EXCEPTION_BAD_RESPONSE_CODE 0
#define EXCEPTION_BAD_CRC_CODE 1
#define EXCEPTION_BAD_SOH_CODE 2
#define EXCEPTION_BAD_ID_CODE 3
#define EXCEPTION_BAD_STX_CODE 4
#define EXCEPTION_BAD_ACK_RESPONSE_CODE 5
#define EXCEPTION_BAD_COUNT 6

    class Clcdm2000
    {
        const cc_byte EOT = 0x04;
        const cc_byte ID = 0x50;
        const cc_byte STX = 0x02;
        const cc_byte ETX = 0x03;
        const cc_byte SOH = 0x01;
        const cc_byte ACK = 0x06;
        const cc_byte NCK = 0x15;

        cc_byte errorCode;
        std::string errorMessage;

    public: // sensors
        bool CheckSensor1;
        bool CheckSensor2;
        bool CheckSensor3;
        bool CheckSensor4;
        bool DivertSensor1;
        bool DivertSensor2;
        bool EjectSensor;
        bool ExitSensor;
        bool SolenoidSensor;
        bool UpperNearEnd;
        bool LowerNearEnd;
        bool CashBoxUpper;
        bool CashBoxLower;
        bool RejectTray;

    public:
        Clcdm2000();
        ~Clcdm2000();

        cc_byte GetCRC(vec_bytes bufData, size_t size);
        bool checkErrors(cc_byte test);
        vec_bytes compileCommand(LcdmCommands cmd, vec_bytes data = {});
        int sendCommand(LcdmCommands cmd, vec_bytes data = {});
        cc_byte getACK();
        void sendACK();
        void sendNAK();
        vec_bytes getResponse(int recv_bytes, int attempts = 3);
        vec_bytes go(LcdmCommands cmd, vec_bytes data = {}, int recv_bytes = 7);
        bool testCRC(vec_bytes bufData);
        void print(std::string msg, vec_bytes data);

        void connect(std::string com_port, int baudrate = 9600);
        void disconnect();

        void purge();
        void status();
        void testStatus();
        void printStatus();
        void upperDispense(int _count);
        void lowerDispense(int _count);
        vec_bytes upperLowerDispense(int _count_upper, int _count_lower);

    public:
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
            };

            const char *what() const noexcept override
            {
                return str.c_str();
            };
        };

        struct TTY
        {
            TTY();
            virtual ~TTY();
            bool IsOK() const;
            void Connect(const std::string &port, int baudrate);
            void Disconnect();
            int Write(const std::vector<unsigned char> &data);
            void Read(std::vector<unsigned char> &data, int size = 200);
            int fileDescriptor = -1;
        };

        TTY tty;
    };
} // namespace

#endif // LCDM_2000_H
