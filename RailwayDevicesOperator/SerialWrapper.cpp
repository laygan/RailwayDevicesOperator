#include "SerialWrapper.h"
#include <tchar.h>

SerialWrapper::SerialWrapper(int comNum)
{
	//We're not yet connected
	this->connected = false;

	//Try to connect to the given port throuh CreateFile
	TCHAR portName[10];
	_stprintf_s(portName, 10, _T("COM%d"), comNum);

	this->hSerial = CreateFile(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND){

			//Print Error if neccessary
			MessageBoxA(NULL, "ERROR: Handle was not attached.\n Reason: Defined pore is not available.\n", "RailwayDevicesOperator.dll", MB_OK);

		}
		else
		{
			MessageBoxA(NULL, "ERROR!!!", "RailwayDevicesOperator.dll", MB_OK);
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			MessageBoxA(NULL, "ERROR: failed to get current serial parameters!", "RailwayDevicesOperator.dll", MB_OK);
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_115200;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			//Setting the DTR to Control_Enable ensures that the Arduino is properly
			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				MessageBoxA(NULL, "ALERT: Could not set Serial Port parameters", "RailwayDevicesOperator.dll", MB_OK);
			}
			else
			{
				//If everything went fine we're connected
				this->connected = true;
				//Flush any remaining characters in the buffers 
				PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				//We wait 2s as the arduino board will be reseting
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}
}

// 開いていたシリアルデバイスを破棄する
SerialWrapper::~SerialWrapper()
{
	//Check if we are connected before trying to disconnect
	if (this->connected)
	{
		//We're no longer connected
		this->connected = false;
		//Close the serial handler
		CloseHandle(this->hSerial);
	}
}

size_t SerialWrapper::write(const char buf[])
{
	DWORD lengthOfSent = strnlen_s(buf, 6); // 送信するbyte数
	DWORD numberOfPut; // 実際に送信したbyte数

	// ポートへ送信
	WriteFile(this->hSerial, buf, lengthOfSent, &numberOfPut, NULL);

	return numberOfPut;
}

int SerialWrapper::available()
{
	//受信データ数を調べる
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(this->hSerial, &errors, &comStat);

	int lengthOfRecieved = comStat.cbInQue; // 受信したメッセージ長を取得する

	return lengthOfRecieved;
}

int SerialWrapper::read()
{
	//受信データがない場合は読み込まない
	if (available() < 1)
	{
		return -1;
	}

	char buf[1]; // 受信データ格納用
	DWORD numberOfRead; // 実際に受信したバイト数

	//データ受信
	bool result = ReadFile(this->hSerial, buf, 1, &numberOfRead, NULL);

	if (result == FALSE)
	{
		return -1;
	}
	else
	{
		return buf[0];
	}
}

bool SerialWrapper::IsConnected()
{
	//Simply return the connection status
	return this->connected;
}