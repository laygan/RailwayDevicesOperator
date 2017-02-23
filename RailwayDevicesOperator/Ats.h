int brake;
int power;
int reverser;

// 各保安装置の状態を覚えておくための配列
int prevState[10] = {};

/* ATSプラグイン保安装置端子台
 0 ATS-Sx	panel	電源(白)
 1 ATS-Sx	panel	動作(赤)
 2 ATS-P	panel	P電源
 3 ATS-P	panel	パターン接近
 4 ATS-P	panel	ブレーキ動作
 5 ATS-P	panel	ブレーキ開放
 6 ATS-P	panel	ATS-P
 7 ATS-P	panel	故障
 8 ATC/S-Sx	SOUND	警報ベル
 9 ATC/S-P	SOUND	チンベル
*/
int AtsInfo[10] = {};

/* 列車保安装置制御マイコン ピンアサイン
 (arduino向けピン配列)				
 0 ** NC(SPI RX) **
 1 ** NC(SPI TX) **
 2 ATS-P	P電源
 3 ATS-P	パターン接近
 4 ATS-P	ブレーキ動作
 5 ATS-P	ブレーキ開放
 6 ATS-P	ATS-P
 7 ATS-P	故障
 8 ATC/S-P	チンベル
 9 ATS-Sx	電源(白)
10 ATS-Sx	動作(赤)
11 ** NC **
12 ATC/ATS-Sx 警報
13 ATS-Sx チャイム停止(回路開放)

/* 列車保安装置制御用マイコンへ送る信号

 0 ATS-Sx	電源(白)
 1 ATS-Sx	動作(赤) and 警報
 2 ATS-P	P電源
 3 ATS-P	パターン接近
 4 ATS-P	ブレーキ動作
 5 ATS-P	ブレーキ開放
 6 ATS-P	ATS-P
 7 ATS-P	故障
 8 ATC/S-P	チンベル
 9 ATC		チンベル
10 ATC/EB	警報
11 ATS-Sx	チャイム停止(回路開放)
*/
int mcPortNum[12] = {11, 12, 2, 3, 4, 5, 6, 7, 8, 8, 12, 13};

ATS_HANDLES output;