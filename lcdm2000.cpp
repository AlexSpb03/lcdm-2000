#include "lcdm2000.h"

lcdm2000::lcdm2000()
{
}

lcdm2000::~lcdm2000()
{
}

cc_byte lcdm2000::GetCRC(vec_bytes bufData, size_t size)
{
	cc_byte crc = bufData[0];
	for (int i = 1; i < size; i++)
	{
		crc ^= bufData[i];
	}

	return crc;
}

bool lcdm2000::checkErrors(cc_byte test)
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

vec_bytes lcdm2000::compileCommand(LcdmCommands cmd, vec_bytes data)
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

int lcdm2000::sendCommand(LcdmCommands cmd, vec_bytes data)
{
	try
	{
		auto packet = compileCommand(cmd, data);
		print_b("Send", packet);
		return tty.Write(packet);
	}
	catch (std::exception &e)
	{
		throw e;
	}
}

cc_byte lcdm2000::getACK()
{
	vec_bytes buf;
	try
	{
		tty.Read(buf, 1);
		print_b("Recv ack", buf);
	}
	catch (std::exception &e)
	{
		throw e;
	}

	return buf[0];
}

void lcdm2000::sendACK()
{

	vec_bytes packet = {static_cast<cc_byte>(LcdmCommands::ACK)};
	tty.Write(packet);
}

vec_bytes lcdm2000::getResponse(int recv_bytes)
{
	vec_bytes buf;

	try
	{
		tty.Read(buf, recv_bytes);
	}
	catch (std::exception &e)
	{
		throw e;
	}

	print_b("Recv", buf);

	if (buf.size() < 4)
	{
		throw new lcdm2000Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
	}

	if (!testCRC(buf))
	{
		throw new lcdm2000Exception("Bad CRC response", EXCEPTION_BAD_CRC_CODE);
	}

	if (buf[0] != SOH)
	{
		throw new lcdm2000Exception("Bad SOH response", EXCEPTION_BAD_SOH_CODE);
	}

	if (buf[1] != ID)
	{
		throw new lcdm2000Exception("Bad ID response", EXCEPTION_BAD_ID_CODE);
	}

	if (buf[2] != STX)
	{
		throw new lcdm2000Exception("Bad STX response", EXCEPTION_BAD_STX_CODE);
	}

	sendACK();

	return buf;
}

bool lcdm2000::testCRC(vec_bytes bufData)
{
	cc_byte crc = bufData[0];
	for (int i = 1; i < bufData.size() - 1; i++)
	{
		crc ^= bufData[i];
	}
	return crc == bufData[bufData.size() - 1];
}

void lcdm2000::print_b(std::string msg, vec_bytes data)
{
	std::cout << msg << ": ";
	for (auto byte : data)
		std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)byte << " ";
	std::cout << std::endl;
}

void lcdm2000::connect(std::string com_port, int baudrate)
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

vec_bytes lcdm2000::go(LcdmCommands cmd, vec_bytes data, int recv_bytes)
{
	vec_bytes response;

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
		if (getACK() != ACK)
		{
			throw new lcdm2000Exception("Bad ACK response", EXCEPTION_BAD_ACK_RESPONSE_CODE);
		}
	}
	catch (const std::exception &e)
	{
		throw e;
	}

	//--- try recv response
	try
	{
//		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		response = getResponse(recv_bytes);
	}
	catch (const std::exception &e)
	{
		throw e;
	}

	return response;
}

void lcdm2000::purge()
{
	//--- length response in bytes. see docs
	int lenResponse = 7;

	//--- position error byte in response packet
	int numErrorByte = 4;

	auto response = go(LcdmCommands::PURGE, {}, lenResponse);

	//--- response STATUS command must be 10 bytes
	if (response.size() != lenResponse)
	{
		throw new lcdm2000Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
	}

	//--- check errors in response
	if (checkErrors(response[numErrorByte]))
	{
		throw new lcdm2000Exception(errorMessage, errorCode);
	}
}

void lcdm2000::status()
{
	//--- length response in bytes. see docs
	int lenResponse = 10;

	//--- position error byte in response packet
	int numErrorByte = 5;

	auto response = go(LcdmCommands::STATUS, {}, lenResponse);

	//--- response STATUS command must be 10 bytes
	if (response.size() != lenResponse)
	{
		throw new lcdm2000Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
	}

	//--- check errors in response
	if (checkErrors(response[numErrorByte]))
	{
		throw new lcdm2000Exception(errorMessage, errorCode);
	}
}

void lcdm2000::upperDispense(int _count)
{
	//--- length response in bytes. see docs
	int lenResponse = 14;

	//--- position error byte in response packet
	int numErrorByte = 8;

	if (_count < 1 || _count > 60)
	{
		throw new lcdm2000Exception("Bad count for upperDeipense", EXCEPTION_BAD_COUNT);
	}

	std::string count = std::to_string(_count);
	vec_bytes data;
	if(_count < 10){
		data.push_back(static_cast<cc_byte>(0x30));
		data.push_back(static_cast<cc_byte>(count[0]));
	}else{
		data.push_back(static_cast<cc_byte>(count[0]));
		data.push_back(static_cast<cc_byte>(count[1]));
	}

	//--- get response
	auto response = go(LcdmCommands::UPPER_DISPENSE, data, lenResponse);

	//--- response STATUS command must be 10 bytes
	if (response.size() != lenResponse)
	{
		throw new lcdm2000Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
	}

	//--- check errors in response
	if (checkErrors(response[numErrorByte]))
	{
		throw new lcdm2000Exception(errorMessage, errorCode);
	}
}

void lcdm2000::lowerDispense(int _count)
{
	//--- length response in bytes. see docs
	int lenResponse = 14;

	//--- position error byte in response packet
	int numErrorByte = 8;

	if (_count < 1 || _count > 60)
	{
		throw new lcdm2000Exception("Bad count for lowerDeipense", EXCEPTION_BAD_COUNT);
	}

	std::string count = std::to_string(_count);
	vec_bytes data;
	if(_count < 10){
		data.push_back(static_cast<cc_byte>(0x30));
		data.push_back(static_cast<cc_byte>(count[0]));
	}else{
		data.push_back(static_cast<cc_byte>(count[0]));
		data.push_back(static_cast<cc_byte>(count[1]));
	}

	//--- get response
	auto response = go(LcdmCommands::LOWER_DISPENSE, data, lenResponse);

	//--- response STATUS command must be 10 bytes
	if (response.size() != lenResponse)
	{
		throw new lcdm2000Exception("Bad response", EXCEPTION_BAD_RESPONSE_CODE);
	}

	//--- check errors in response
	if (checkErrors(response[numErrorByte]))
	{
		throw new lcdm2000Exception(errorMessage, errorCode);
	}
}