#pragma once

#include "DataStore.h"
#include "FlowerSet.h"
class FlowerSet;

// �萔�̒�`
#define REPEAT_NUM 1000 // �J�Ԃ���
#define EBEE_NUM   100 // ���n�o�`�̐�
#define OBEE_NUM   60   // �Ǐ]�o�`�̐�
#define VISIT_MAX  200  // ���̍̎�\��
#define R_CONSIDER_FACILITY 0.15
#define R_ADJUST_FACILITY 0.95
#define R_CONSIDER_SUBSIDY 0.05
#define R_ADJUST_SUBSIDY 0.8


// 0�ȏ�1�ȉ��̎�������
#define RAND_01 ((double)rand() / RAND_MAX)

class Flower
{
public:
	DataStore* datastore;

    Flower(FlowerSet *argFSet);
    ~Flower();
    void change(int base); // �߂��̐H�ƌ��ɕύX����
    void renew();          // �V�����H�ƌ��ɕύX����

	void writeResult();

    FlowerSet *fSet;       // �����Ă���H�ƌ��W��
    int visitNum;          // ���̍̎��

private:
   void evaluate();       // �]���l���Z�o����
   double *calcFacilityGeneProb(); //�{�ݐ������̌v�Z
};
