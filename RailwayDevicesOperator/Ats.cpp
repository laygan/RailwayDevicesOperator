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
bool Atc = false;

SerialWrapper * arduino;

ATS_API void WINAPI Load()
{
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
	// �ݒ�t�@�C����ǂݍ���
	String^ dllPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	String^ dllDir = Path::GetDirectoryName(dllPath);
	String^ iniPath = Path::GetDirectoryName(dllPath) + Path::DirectorySeparatorChar + Path::GetFileNameWithoutExtension(dllPath) + ".ini";

	// �}�C�R���ʐM�p�V���A���|�[�g�ԍ��擾
	int mcPort = getSettingFromIni("MC", "ComPort", 7, iniPath);

	// arduino�V���A���ʐM�J�n
	arduino = new SerialWrapper(mcPort);

	// ATS-P �p�l�����i�ԍ����擾
	mcPortNum[2] = getSettingFromIni("AtsP", "Power", 0, iniPath);
	mcPortNum[3] = getSettingFromIni("AtsP", "Pattern", 0, iniPath);
	mcPortNum[4] = getSettingFromIni("AtsP", "Break", 0, iniPath);
	mcPortNum[5] = getSettingFromIni("AtsP", "Release", 0, iniPath);
	mcPortNum[6] = getSettingFromIni("AtsP", "Active", 0, iniPath);
	mcPortNum[7] = getSettingFromIni("AtsP", "Fail", 0, iniPath);
	
	// ATC�����x���T�E���h�̌��o
	mcPortNum[10] = getSettingFromIni("Atc", "Bell", 0, iniPath);

	// ATC/ATS�r�����p
	if (mcPortNum[10] != 0)
	{
		Atc = true;
	}

	if (!arduino->IsConnected())
	{
		MessageBoxA(NULL, "ERROR: Cannot connect to controller.", "RailwayDevicesOperator.dll", MB_OK);
		exit(1);
	}

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

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	output.Brake = brake;
	output.Power = power;
	output.Reverser = reverser;
	output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	// ATC���L���̏ꍇ�́AATS-P/Sx�͕ω��������ω��Ȃǌ��o���Ȃ�
	if (Atc)
	{
		if (prevState[mcPortNum[10]] != sound[mcPortNum[10]])
		{
			char buff[10] = "";
			sprintf_s(buff, "%d,1\n", mcPortNum[10]);
			arduino->write(buff);
			prevState[mcPortNum[10]] = sound[mcPortNum[10]];
		}
	}
	else{
		// ATS-P�����ω����o
		for (int i = 2; i < 8; i++)
		{
			if (prevState[i] != panel[i])
			{
				char buff[10] = "";
				sprintf_s(buff, "%d,%d\n", mcPortNum[i], panel[i]);
				arduino->write(buff);
				prevState[i] = panel[i];
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