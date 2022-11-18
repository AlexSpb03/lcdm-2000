#include "lcdm2000.h"

namespace Devices
{
	lcdm2000::lcdm2000()
	{
	}

	lcdm2000::~lcdm2000()
	{
	}

	/**
	 * @brief
	 * Calc CRC packet
	 *
	 * @param bufData - buffer
	 * @param size - size of buffer 0..size
	 * @return cc_byte
	 */
	cc_byte
	lcdm2000::GetCRC(vec_bytes bufData, size_t size)
	{
		cc_byte crc = bufData[0];
		for (int i = 1; i < size; i++)
		{
			crc ^= bufData[i];
		}

		return crc;
	}

	/**
	 * @brief
	 * test crc in response packet
	 *
	 * @param bufData
	 * @return true
	 * @return false
	 */
	bool
	lcdm2000::testCRC(vec_bytes bufData)
	{
		//--- Begin from 0 byte in buffer
		cc_byte crc = bufData[0];

		//--- for buffer size - 1
		//--- last byte - crc from device
		for (int i = 1; i < bufData.size() - 1; i++)
		{
			crc ^= bufData[i];
		}

		//--- test crc in packet and calc
		return crc == bufData[bufData.size() - 1];
	}

	/**
	 * @brief
	 * check error in response
	 *
	 * @param test - byte with data
	 * @return true
	 * @return false
	 */
	bool
	lcdm2000::checkErrors(cc_byte test)
	{
		errorCode = test;
		bool error = true;
		switch (errorCode)
		{
		case 0x30:
			error = false;
			errorMessage = "Good";
			break;

		case 0x31:
			error = false;
			errorMessage = "Normal stop";
			break;

		case 0x32:
			errorMessage = "Pickup error";
			break;

		case 0x33:
			errorMessage = "JAM at CHK1,2 Sensor";
			break;

		case 0x34:
			errorMessage = "Overflow bill";
			break;

		case 0x35:
			errorMessage = "JAM at EXIT Sensor or EJT Sensor";
			break;

		case 0x36:
			errorMessage = "JAM at DIV Sensor";
			break;

		case 0x37:
			errorMessage = "Undefined command";
			break;

		case 0x38:
			errorMessage = "Upper Bill- End";
			break;

		case 0x3A:
			errorMessage = "Counting Error(between CHK3,4 Sensor and DIV Sensor)";
			break;

		case 0x3B:
			errorMessage = "Note request error";
			break;

		case 0x3C:
			errorMessage = "Counting Error(between DIV Sensor and EJT Sensor)";
			break;

		case 0x3D:
			errorMessage = "Counting Error(between EJT Sensor and EXIT Sensor)";
			break;

		case 0x3F:
			errorMessage = "Reject Tray is not recognized";
			break;

		case 0x40:
			errorMessage = "Lower Bill-End";
			break;

		case 0x41:
			errorMessage = "Motor Stop";
			break;

		case 0x42:
			errorMessage = "JAM at Div Sensor";
			break;

		case 0x43:
			errorMessage = "Timeout (From DIV Sensor to EJT Sensor)";
			break;

		case 0x44:
			errorMessage = "Over Reject";
			break;

		case 0x45:
			errorMessage = "Upper Cassette is not recognized";
			break;

		case 0x46:
			errorMessage = "Lower Cassette is not recognized";
			break;

		case 0x47:
			errorMessage = "Dispensing timeout";
			break;

		case 0x48:
			errorMessage = "JAM at EJT Sensor";
			break;

		case 0x49:
			errorMessage = "Diverter solenoid or SOL Sensor error";
			break;

		case 0x4A:
			errorMessage = "SOL Sensor error";
			break;

		case 0x4C:
			errorMessage = "JAM at CHK3,4 Sensor";
			break;

		case 0x4E:
			errorMessage = "Purge error(Jam at Div Sensor)";
			break;

		default:
			errorMessage = "Unknown error";
		}

		return error;
	}

	/**
	 * @brief Open port with device
	 *
	 * @param com_port ex. /dev/ttyUSB0
	 * @param baudrate ex. B9600
	 */
	void
	lcdm2000::connect(std::string com_port, int baudrate)
	{
		try
		{
			tty.Connect(com_port, baudrate);
		}
		catch (const std::exception &e)
		{
			throw e;
		}
	}

	/**
	 * @brief
	 * compile packet to send in device LCDM-2000
	 *
	 * @param cmd
	 * @param data
	 * @return vec_bytes
	 */
	vec_bytes
	lcdm2000::compileCommand(LcdmCommands cmd, vec_bytes data)
	{
		vec_bytes packet;

		packet.push_back(EOT);
		packet.push_back(ID);
		packet.push_back(STX);
		packet.push_back(static_cast<cc_byte>(cmd));
		for (int i = 0; i < data.size(); i++)
		{
			packet.push_back(data[i]);
		}
		packet.push_back(ETX);
		cc_byte CRC = GetCRC(packet, packet.size());
		packet.push_back(CRC);

		return packet;
	}

	/**
	 * @brief Send commant to device via tty
	 *
	 * @param cmd
	 * @param data
	 * @return int
	 */
	int
	lcdm2000::sendCommand(LcdmCommands cmd, vec_bytes data)
	{
		try
		{
			auto packet = compileCommand(cmd, data);
			print("Send", packet);
			return tty.Write(packet);
		}
		catch (const TTY::Exception &e)
		{
			throw e;
		}
	}

	/**
	 * @brief Recv byte ACK from LCDM-2000 after send command
	 *
	 * @return cc_byte
	 */
	cc_byte
	lcdm2000::getACK()
	{
		vec_bytes buf;
		try
		{
			tty.Read(buf, 1);
		}
		catch (const TTY::Exception &e)
		{
			throw e;
		}

		if (buf.size() != 1)
		{
			throw new Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		return buf[0];
	}

	/**
	 * @brief Send ACK to Device after recv response
	 *
	 */
	void
	lcdm2000::sendACK()
	{

		vec_bytes packet = {static_cast<cc_byte>(LcdmCommands::ACK)};
		try
		{
			tty.Write(packet);
		}
		catch (const TTY::Exception &e)
		{
			throw e;
		}
	}

	/**
	 * @brief Send NAK to Device after recv response
	 *
	 */
	void
	lcdm2000::sendNAK()
	{

		vec_bytes packet = {static_cast<cc_byte>(LcdmCommands::NAK)};
		try
		{
			tty.Write(packet);
		}
		catch (const TTY::Exception &e)
		{
			throw e;
		}
	}

	/**
	 * @brief Recv response data from device
	 *
	 * @param recv_bytes
	 * @param attempts - count attempts to get response. see 2.2.3 manual. Default 3 attempts.
	 * @return vec_bytes
	 */
	vec_bytes
	lcdm2000::getResponse(int recv_bytes, int attempts)
	{
		bool errorGet = false;

		//--- if attempts = 0 exit with exception
		if (!attempts)
		{
			throw Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		vec_bytes buf;

		//--- try recv data from device
		try
		{
			tty.Read(buf, recv_bytes);
		}
		catch (const TTY::Exception &e)
		{
			throw e;
		}

		print("Recv", buf);

		if (buf.size() < 4)
		{
			errorGet = true;
		}

		//--- test error in response
		if (errorGet || !testCRC(buf) || buf[0] != SOH || buf[1] != ID || buf[2] != STX)
		{
			errorGet = true;
		}

		//--- if error in response
		if (errorGet)
		{
			//--- send NAK byte to device
			sendNAK();

			//--- new try get response
			return getResponse(recv_bytes, attempts - 1);
		}

		//--- Send ACK byte to device
		sendACK();

		//--- return response
		return buf;
	}

	/**
	 * @brief print recv data
	 *
	 * @param msg
	 * @param data
	 */
	void
	lcdm2000::print(std::string msg, vec_bytes data)
	{
		std::cout << msg << ": ";
		for (auto byte : data)
			std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)byte << " ";
		std::cout << std::endl;
	}

	/**
	 * @brief Send command and get response
	 *
	 * @param cmd
	 * @param data
	 * @param recv_bytes
	 * @return vec_bytes
	 */
	vec_bytes
	lcdm2000::go(LcdmCommands cmd, vec_bytes data, int recv_bytes)
	{
		vec_bytes response;
		bool success = false;
		int attempts_count = 2;

		for (int attempt{0}; attempt < attempts_count; attempt++)
		{
			//--- try send command
			try
			{
				sendCommand(cmd, data);
			}
			catch (std::exception &e)
			{
				throw e;
			}

			//--- try recv ACK response
			try
			{
				cc_byte res = getACK();

				if (res == static_cast<cc_byte>(LcdmCommands::ACK))
				{
					success = true;
					break;
				}

				if (res == static_cast<cc_byte>(LcdmCommands::NAK))
				{
					continue;
				}
			}
			catch (const Exception &e)
			{
				throw e;
			}
		}

		if (!success)
		{
			throw Exception("Bad ACK response", EXCEPTION_BAD_ACK_RESPONSE_CODE);
		}

		//--- try recv response
		try
		{
			response = getResponse(recv_bytes);
		}
		catch (const Exception &e)
		{
			throw e;
		}

		return response;
	}

	/**
	 * @brief Send purge command
	 *
	 */
	void
	lcdm2000::purge()
	{
		//--- length response in bytes. see docs
		int lenResponse = 7;

		//--- position error byte in response packet
		int numErrorByte = 4;

		auto response = go(LcdmCommands::PURGE, {}, lenResponse);

		//--- compare length
		if (response.size() != lenResponse)
		{
			throw Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		//--- check errors in response
		if (checkErrors(response[numErrorByte]))
		{
			throw Exception(errorMessage, errorCode);
		}
	}

	/**
	 * @brief Send status command
	 *
	 */
	void
	lcdm2000::status()
	{
		//--- length response in bytes. see docs
		int lenResponse = 10;

		//--- position error byte in response packet
		int numErrorByte = 5;

		auto response = go(LcdmCommands::STATUS, {}, lenResponse);

		//--- compare length
		if (response.size() != lenResponse)
		{
			throw Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		//--- check errors in response
		if (checkErrors(response[numErrorByte]))
		{
			throw Exception(errorMessage, errorCode);
		}

		CheckSensor1 = (response[6] & 0b00000001) ? true : false;
		CheckSensor2 = (response[6] & 0b00000010) ? true : false;
		CheckSensor3 = (response[7] & 0b00001000) ? true : false;
		CheckSensor4 = (response[7] & 0b00010000) ? true : false;
		DivertSensor1 = (response[6] & 0b00000100) ? true : false;
		DivertSensor2 = (response[6] & 0b00001000) ? true : false;
		EjectSensor = (response[6] & 0b00010000) ? true : false;
		ExitSensor = (response[6] & 0b00100000) ? true : false;
		SolenoidSensor = (response[7] & 0b00000001) ? true : false;
		UpperNearEnd = (response[6] & 0b01000000) ? true : false;
		LowerNearEnd = (response[7] & 0b00100000) ? true : false;
		CashBoxUpper = (response[7] & 0b00000010) ? true : false;
		CashBoxLower = (response[7] & 0b00000100) ? true : false;
		RejectTray = (response[7] & 0b01000000) ? true : false;
	}

	/**
	 * @brief Send UPPER_DISPENSE command
	 *
	 * @param _count - count bills to dispense from upper box
	 */
	void
	lcdm2000::upperDispense(int _count)
	{
		//--- length response in bytes. see docs
		int lenResponse = 14;

		//--- position error byte in response packet
		int numErrorByte = 8;

		if (_count < 1 || _count > 60)
		{
			throw Exception("Bad count for upperDeipense", EXCEPTION_BAD_COUNT);
		}

		std::string count = std::to_string(_count);
		vec_bytes data;
		if (_count < 10)
		{
			data.push_back(static_cast<cc_byte>(0x30));
			data.push_back(static_cast<cc_byte>(count[0]));
		}
		else
		{
			data.push_back(static_cast<cc_byte>(count[0]));
			data.push_back(static_cast<cc_byte>(count[1]));
		}

		//--- get response
		auto response = go(LcdmCommands::UPPER_DISPENSE, data, lenResponse);

		//--- compare length
		if (response.size() != lenResponse)
		{
			throw Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		//--- check errors in response
		if (checkErrors(response[numErrorByte]))
		{
			throw Exception(errorMessage, errorCode);
		}
	}

	/**
	 * @brief Send LOWER_DISPENSE command
	 *
	 * @param _count - count bills to dispense from lower box
	 */
	void
	lcdm2000::lowerDispense(int _count)
	{
		//--- length response in bytes. see docs
		int lenResponse = 14;

		//--- position error byte in response packet
		int numErrorByte = 8;

		if (_count < 1 || _count > 60)
		{
			throw Exception("Bad count for lowerDeipense", EXCEPTION_BAD_COUNT);
		}

		std::string count = std::to_string(_count);
		vec_bytes data;
		if (_count < 10)
		{
			data.push_back(static_cast<cc_byte>(0x30));
			data.push_back(static_cast<cc_byte>(count[0]));
		}
		else
		{
			data.push_back(static_cast<cc_byte>(count[0]));
			data.push_back(static_cast<cc_byte>(count[1]));
		}

		//--- get response
		auto response = go(LcdmCommands::LOWER_DISPENSE, data, lenResponse);

		//--- compare length
		if (response.size() != lenResponse)
		{
			throw Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		//--- check errors in response
		if (checkErrors(response[numErrorByte]))
		{
			throw Exception(errorMessage, errorCode);
		}
	}

	vec_bytes
	lcdm2000::upperLowerDispense(int _count_upper, int _count_lower)
	{
		//--- length response in bytes. see docs
		int lenResponse = 21;

		//--- position error byte in response packet
		int numErrorByte = 12;

		if (_count_upper < 0 || _count_upper > 60)
		{
			throw Exception("Bad _count_upper for upperLowerDispense", EXCEPTION_BAD_COUNT);
		}

		if (_count_lower < 0 || _count_lower > 60)
		{
			throw Exception("Bad _count_lower for upperLowerDispense", EXCEPTION_BAD_COUNT);
		}

		std::string count_upper = std::to_string(_count_upper);
		std::string count_lower = std::to_string(_count_lower);
		vec_bytes data;

		//--- Add Upper bills
		if (_count_upper < 10)
		{
			data.push_back(static_cast<cc_byte>(0x30));
			data.push_back(static_cast<cc_byte>(count_upper[0]));
		}
		else
		{
			data.push_back(static_cast<cc_byte>(count_upper[0]));
			data.push_back(static_cast<cc_byte>(count_upper[1]));
		}

		//--- Add Lower bills
		if (_count_lower < 10)
		{
			data.push_back(static_cast<cc_byte>(0x30));
			data.push_back(static_cast<cc_byte>(count_lower[0]));
		}
		else
		{
			data.push_back(static_cast<cc_byte>(count_lower[0]));
			data.push_back(static_cast<cc_byte>(count_lower[1]));
		}

		//--- get response
		auto response = go(LcdmCommands::UPPER_AND_LOWER_DISPENSE, data, lenResponse);

		//--- compare length
		if (response.size() != lenResponse)
		{
			throw Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
		}

		//--- check errors in response
		if (checkErrors(response[numErrorByte]))
		{
			throw Exception(errorMessage, errorCode);
		}

		vec_bytes result;
		std::vector<vec_bytes> poss = {
			{6, 7},	  //--- EXIT UPPER
			{10, 11}, //--- EXIT LOWER
			{15, 16}, //--- REJECTED UPPER
			{17, 18}, //--- REJECTED LOWER
			{4, 5},	  //--- CHECK UPPER
			{8, 9}	  //--- CHECK LOWER
		};

		for (auto &pos : poss)
		{
			result.push_back((response[pos[0]] - 0x30) * 10 + (response[pos[1]] - 0x30));
		}

		return result;
	}

} // namespace