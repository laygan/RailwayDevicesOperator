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

bool purgeBZ21 = false;

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

	// ATS-Sx �T�E���h | �ԗ��p�l�����i �Ď��ݒ�
	AtsInfo[0] = getSettingFromIni("AtsSx", "Power", -1, iniPath);		// ��
	AtsInfo[1] = getSettingFromIni("AtsSx", "Active", -1, iniPath);		// ��
	AtsInfo[8] = getSettingFromIni("AtsSx", "Bell", -1, iniPath);		// ATS�x�����o�iEB���쓮���ɂ��������炷���߁j

	// ATS-P �T�E���h | �ԗ��p�l�����i �Ď��ݒ�
	AtsInfo[2] = getSettingFromIni("AtsP", "Power", -1, iniPath);		// P�d��
	AtsInfo[3] = getSettingFromIni("AtsP", "Pattern", -1, iniPath);		// �p�^�[���ڋ�
	AtsInfo[4] = getSettingFromIni("AtsP", "Break", -1, iniPath);		// �u���[�L����i��p,��팓�p�j
	AtsInfo[5] = getSettingFromIni("AtsP", "Release", -1, iniPath);		// �u���[�L�J��
	AtsInfo[6] = getSettingFromIni("AtsP", "Active", -1, iniPath);		// ATS-P
	AtsInfo[7] = getSettingFromIni("AtsP", "Fail", -1, iniPath);		// �̏�
	AtsInfo[9] = getSettingFromIni("AtsP", "Bell", -1, iniPath);		// ding

	// ATC �T�E���h|�ԗ��p�l�����i �Ď��ݒ�
	if (AtsInfo[9] == -1)
	{
		AtsInfo[9] = getSettingFromIni("Atc", "Bell", -1, iniPath);		// ding
	}
//	AtsInfo[10] = getSettingFromIni("Atc", "Break", -1, iniPath);		// �u���[�L����\�����i��p,��팓�p->ATS-P �u���[�L����j

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
	// end of handshake
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

	/***** �p�l���ω����o�� *****/
	// ATS�����ω����o
	for (int i = 0; i <= 7; i++)
	{
		if (AtsInfo[i] == -1) continue;
		if (prevState[i] != panel[AtsInfo[i]])
		{
			char buff[10] = "";
			sprintf_s(buff, "%d,%d\n", mcPortNum[i], panel[AtsInfo[i]]);
			arduino->write(buff);
			prevState[i] = panel[AtsInfo[i]];
		}
	}

	/*
	// ATS-Sx�`���C���؂藣��
	if (AtsInfo[9] != -1 && panel[AtsInfo[0]] < 1)
	{
		if (! purgeBZ21)
		{
			char buff[10] = "";
			sprintf_s(buff, "%d,%d\n", mcPortNum[11], 1);
			arduino->write(buff);
		}
		purgeBZ21 = true;
	}
	else
	{
		if (purgeBZ21)
		{
			char buff[10] = "";
			sprintf_s(buff, "%d,%d\n", mcPortNum[11], 0);
			arduino->write(buff);
		}
		purgeBZ21 = false;
	}
	*/
	/***** �T�E���h�ω����o�� *****/

	for (int i = 8; i <= 9 ; i++)
	{
		if (AtsInfo[i] == -1) continue;
		if (prevState[i] != sound[AtsInfo[i]])
		{
			char buff[10] = "";
			sprintf_s(buff, "%d,%d\n", mcPortNum[i], sound[AtsInfo[i]]);
			arduino->write(buff);
			prevState[i] = sound[AtsInfo[i]];
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
	// for debug
	if (atsKeyCode == ATS_KEY_B1)
	{
		char buff[10] = "";
		sprintf_s(buff, "%d,%d\n", mcPortNum[7], 0);
		arduino->write(buff);
	}

	// BZ21�x���~ ATS_KEY_A1
	if (atsKeyCode == ATS_KEY_A1) // �L�[��������Ă�����
	{
		char buff[10] = "";
		sprintf_s(buff, "%d,%d\n", mcPortNum[11], 1);
		arduino->write(buff);

		sprintf_s(buff, "%d,%d\n", mcPortNum[11], 0);
		arduino->write(buff);

	}

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