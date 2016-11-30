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
	// arg1 : COM�|�[�g�̔ԍ����w��
	// �w�肵���V���A���f�o�C�X���J��
	SerialWrapper(int comNum);

	// �J���Ă����V���A���f�o�C�X��j������
	~SerialWrapper();

	// arg1 : ���M����byte��
	// ����byte�̃f�[�^����x�ɑ��M����
	// ���ۂɑ��M����byte����Ԃ�
	virtual size_t write(const char buf[]);

	// ��M�o�b�t�@�ɗ��܂���byte����Ԃ�
	virtual int available();

	// ��M�o�b�t�@����1byte�ǂݍ���
	// ���s�����ꍇ-1��Ԃ�
	virtual int read();

	//Check if we are actually connected
	bool IsConnected();
};

#endif __SerialWrapper__
