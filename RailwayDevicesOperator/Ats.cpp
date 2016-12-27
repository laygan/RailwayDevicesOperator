// ------------------------------------------------------
// Railway Devices Opetator(Ra-DO)
//  inspired by BVE Workshop http://bvews.jpn.org/
//                                  and Youtube movies.
// Presented by laygan https://laygan.net/
// ------------------------------------------------------

#include <Windows.h>
#include <string>
#include "atsplugin.h"
#include "Ats.h"
#include "SerialWrapper.h"

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

int getSettingFromIni(String^, String^, int, String^);

SerialWrapper * arduino;
bool Atc = false;

ATS_API void WINAPI Load()
{
	// �ݒ�t�@�C����ǂݍ���
	String^ dllPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	String^ dllDir = Path::GetDirectoryName(dllPath);
	String^ iniPath = Path::GetDirectoryName(dllPath) + Path::DirectorySeparatorChar + Path::GetFileNameWithoutExtension(dllPath) + ".ini";

	// �}�C�R���ʐM�p�V���A���|�[�g�ԍ��擾
	int mcPort = getSettingFromIni("MC", "ComPort", 7, iniPath);

	// arduino�V���A���ʐM�J�n
	arduino = new SerialWrapper(mcPort);

	
	AtsInfo[0] = getSettingFromIni("AtsSx", "Power", 0, iniPath);
	AtsInfo[1] = getSettingFromIni("AtsSx", "Active", 1, iniPath);

	// ATS-P �p�l�����i�ԍ����擾
	AtsInfo[2] = getSettingFromIni("AtsP", "Power", 2, iniPath);
	AtsInfo[3] = getSettingFromIni("AtsP", "Pattern", 3, iniPath);
	AtsInfo[4] = getSettingFromIni("AtsP", "Break", 4, iniPath);
	AtsInfo[5] = getSettingFromIni("AtsP", "Release", 5, iniPath);
	AtsInfo[6] = getSettingFromIni("AtsP", "Active", 6, iniPath);
	AtsInfo[7] = getSettingFromIni("AtsP", "Fail", 7, iniPath);

	// ATC�����x���T�E���h�̌��o
	AtsInfo[8] = getSettingFromIni("Atc", "Bell", -1, iniPath);
	AtsInfo[9] = getSettingFromIni("Atc", "Break", 23, iniPath);

	// Atc�x���T�E���h�̌��o���s���邩�ǂ���
	if (AtsInfo[8] != -1)
	{
		Atc = true;
	}

	// �}�C�R���ڑ�
	if (!arduino->IsConnected())
	{
		MessageBoxA(NULL, "ERROR: Cannot connect to controller.", "RailwayDevicesOperator.dll", MB_OK);
		exit(1);
	}

	// handshake
	while (true)
	{
		if (arduino->read() == '1')
		{
			break;
		}
	}
	arduino->write("0\n");
	while (true)
	{
		if (arduino->read() == '1')
		{
			Sleep(1000);
			break;
		}
	}
}

ATS_API void WINAPI Dispose()
{
	// �R���g���[���Ƃ̐ڑ��C���X�^���X�j��
	delete arduino;
}

ATS_API int WINAPI GetPluginVersion()
{
	return ATS_VERSION;
}

ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
}

ATS_API void WINAPI Initialize(int brake)
{	
}

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	output.Brake = brake;
	output.Power = power;
	output.Reverser = reverser;
	output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	// Active ATC?
	if (Atc)
	{
		if (prevState[8] != sound[AtsInfo[8]])
		{
			char buff[10] = "";
			sprintf_s(buff, "%d,%d\n", mcPortNum[8], sound[AtsInfo[8]]);
			arduino->write(buff);
			prevState[8] = sound[AtsInfo[8]];
		}
	}

	else
	{
		// ATS�����ω����o
		for (int i = 0; i < 8; i++)
		{
			if (prevState[i] != panel[AtsInfo[i]])
			{
				char buff[10] = "";
				sprintf_s(buff, "%d,%d\n", mcPortNum[i], panel[AtsInfo[i]]);
				arduino->write(buff);
				prevState[i] = panel[AtsInfo[i]];
			}
		}
	}

	return output;
}

ATS_API void WINAPI SetPower(int notch)
{
	power = notch;
}

ATS_API void WINAPI SetBrake(int notch)
{
	brake = notch;
}

ATS_API void WINAPI SetReverser(int pos)
{
	reverser = pos;
}

ATS_API void WINAPI KeyDown(int atsKeyCode)
{
}

ATS_API void WINAPI KeyUp(int atsKeyCode)
{
}

ATS_API void WINAPI HornBlow(int atsHornBlowIndex)
{
}

ATS_API void WINAPI DoorOpen()
{
}

ATS_API void WINAPI DoorClose()
{
}

ATS_API void WINAPI SetSignal(int signal)
{
}

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
}

// ini�t�@�C������ݒ�l���󂯎�邽�߂ׂ̍�������
// Thankyou for Bve Workshop
// GetPrivateProfileInt�ɏ���
int getSettingFromIni(String^ section, String^ key, int defaultValue, String^ path)
{
	// String -> const TCHAR*
	IntPtr setcionPtr = Marshal::StringToHGlobalAuto(section);
	IntPtr keyPtr = Marshal::StringToHGlobalAuto(key);
	IntPtr pathPtr = Marshal::StringToHGlobalAuto(path);

	const TCHAR* sectionChar = static_cast<const TCHAR*>(setcionPtr.ToPointer());
	const TCHAR* keyChar = static_cast<const TCHAR*>(keyPtr.ToPointer());
	const TCHAR* pathChar = static_cast<const TCHAR*>(pathPtr.ToPointer());

	int result = GetPrivateProfileInt(sectionChar, keyChar, defaultValue, pathChar);

	Marshal::FreeHGlobal(setcionPtr);
	Marshal::FreeHGlobal(keyPtr);
	Marshal::FreeHGlobal(pathPtr);

	// const TCHAR* -> String
	return result;
}