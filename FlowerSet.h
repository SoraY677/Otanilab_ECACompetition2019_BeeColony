#pragma once

#include "Flower.h"
class Flower;

class FlowerSet
{
public:

	int tryNum;//���s��

	FlowerSet();
   ~FlowerSet();
   void employedBeePhase();  // ���n�o�`�t�F�[�Y�����s����
   void onlookerBeePhase();  // �Ǐ]�o�`�t�F�[�Y�����s����
   void scoutBeePhase();     // ��@�o�`�t�F�[�Y�����s����
   void saveBestPos();       // �ŗǐH�ƌ����L�^����
   void printResult(int trial,double timer);       // ���ʂ�\������

   Flower **flower;          // �H�ƌ��̏W���̃����o
   int **bestfacility;      // �ŗǐH�ƌ��̈ʒu
   int bestsubsidy;
   double bestfitness;         // �ŗǐH�ƌ��̕]���l

private:
   Flower *newFlower;        // �V�����H�ƌ�
   double *trValue;          // �e�H�ƌ��̕ϊ���̕]���l
};
