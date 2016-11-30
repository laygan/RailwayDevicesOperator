#ifndef __SerialWrapper__
#define __SerialWrapper__

#define ARDUINO_WAIT_TIME 2000

#include <Windows.h>

class SerialWrapper
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;

public:
	// arg1 : COMポートの番号を指定
	// 指定したシリアルデバイスを開く
	SerialWrapper(int comNum);

	// 開いていたシリアルデバイスを破棄する
	~SerialWrapper();

	// arg1 : 送信するbyte列
	// 複数byteのデータを一度に送信する
	// 実際に送信したbyte数を返す
	virtual size_t write(const char buf[]);

	// 受信バッファに溜まったbyte数を返す
	virtual int available();

	// 受信バッファから1byte読み込む
	// 失敗した場合-1を返す
	virtual int read();

	//Check if we are actually connected
	bool IsConnected();
};

#endif __SerialWrapper__
