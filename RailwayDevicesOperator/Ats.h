int brake;
int power;
int reverser;

// �e�ۈ����u�̏�Ԃ��o���Ă������߂̔z��
int prevState[10] = {};

/* ATS�v���O�C���ۈ����u�[�q��
 0 ATS-Sx	panel	�d��(��)
 1 ATS-Sx	panel	����(��)
 2 ATS-P	panel	P�d��
 3 ATS-P	panel	�p�^�[���ڋ�
 4 ATS-P	panel	�u���[�L����
 5 ATS-P	panel	�u���[�L�J��
 6 ATS-P	panel	ATS-P
 7 ATS-P	panel	�̏�
 8 ATC/S-Sx	SOUND	�x��x��
 9 ATC/S-P	SOUND	�`���x��
*/
int AtsInfo[10] = {};

/* ��ԕۈ����u����}�C�R�� �s���A�T�C��
 (arduino�����s���z��)				
 0 ** NC(SPI RX) **
 1 ** NC(SPI TX) **
 2 ATS-P	P�d��
 3 ATS-P	�p�^�[���ڋ�
 4 ATS-P	�u���[�L����
 5 ATS-P	�u���[�L�J��
 6 ATS-P	ATS-P
 7 ATS-P	�̏�
 8 ATC/S-P	�`���x��
 9 ATS-Sx	�d��(��)
10 ATS-Sx	����(��)
11 ** NC **
12 ATC/ATS-Sx �x��
13 ATS-Sx �`���C����~(��H�J��)

/* ��ԕۈ����u����p�}�C�R���֑���M��

 0 ATS-Sx	�d��(��)
 1 ATS-Sx	����(��) and �x��
 2 ATS-P	P�d��
 3 ATS-P	�p�^�[���ڋ�
 4 ATS-P	�u���[�L����
 5 ATS-P	�u���[�L�J��
 6 ATS-P	ATS-P
 7 ATS-P	�̏�
 8 ATC/S-P	�`���x��
 9 ATC		�`���x��
10 ATC/EB	�x��
11 ATS-Sx	�`���C����~(��H�J��)
*/
int mcPortNum[12] = {11, 12, 2, 3, 4, 5, 6, 7, 8, 8, 12, 13};

ATS_HANDLES output;