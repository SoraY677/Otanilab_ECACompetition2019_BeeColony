#include "Parameter.h"

#ifndef DATA_STORE_H

#define DATA_STORE_H

class DataStore {

public:
	static Parameter* para;

	static int allZoneNum;//�S�Ẵ]�[����
	static int centerZoneNum;//�s�S�]�[����
	static int surroundingZoneNum;//���Ӄ]�[����
	static double transTimeElement;//�ړ��\�ȃ]�[����
	//�ړ������]�[����
	//�ړ��������ԁ��אڃ]�[���ւ̈ړ�����
	static int rep;
	//�t�B�b�g�l�X���o���ۂ̃x�[�X
	//�ꕪ������̈ړ��ɂ�����R�X�g�~�אڂ���]�[���ւ̈ړ����ԁ~�T�[�r�X�󋋗� �~ 0.73;
	static double fit;
	//�{�ݐ������̍Œ�m��(�S�̃]�[���ɑ΂���K�v�Œ���̎{�݊�������l�Ƃ���)
	static double refervalue;

	int** facility;//�{�݂����邩�ǂ����̓񎟌��z��
	int** housePop;//�e�]�[���̐l��
	double** facilityPop;//�e�{�݂�1��������̎g�p�l��
	int subsidy;//�ړ]�⏕��p(�i�K��)
	int moveNum;//�ړ]��
	int facilityNum;//�{�ݐ�
	double moveProb;//�ړ]�m��
	double fitness;//�ŏI�I�ȍ����x�o
	double distancetoCity[4];//4������s�S���܂ł̃]�[����

	DataStore();
	~DataStore();
};
#endif
