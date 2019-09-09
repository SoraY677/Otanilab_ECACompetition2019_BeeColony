#include "FlowerSet.h"

// �R���X�g���N�^
FlowerSet::FlowerSet()
{
   int i,j, best;

   flower = new Flower* [EBEE_NUM];

   best = 0;
   for(i = 0; i < EBEE_NUM; i++) {
      flower[i] = new Flower(this);
      if(flower[i]->datastore->fitness > flower[i]->datastore->fitness) {
         best = i;
      }
   }

   bestfacility = new int* [DataStore::para->zoneVNum];
   for(i = 0; i < DataStore::para->zoneVNum; i++) {
	   bestfacility[i] = new int[DataStore::para->zoneHNum];
	   for (j = 0; j < DataStore::para->zoneHNum; j++)
		   bestfacility[i][j] = flower[best]->datastore->facility[i][j];
   }
   bestsubsidy = 0;
   bestfitness = flower[best]->datastore->fitness;

   newFlower = new Flower(this);
   trValue = new double [EBEE_NUM];
   tryNum = 0;
}

// �f�X�g���N�^
FlowerSet::~FlowerSet()
{
   int i;

   for(i = 0; i < EBEE_NUM; i++) {
      delete flower[i];
   }
   delete [] flower;
   delete [] bestfacility;
   delete newFlower;
   delete [] trValue;
}

// ���n�o�`�t�F�[�Y�����s����
void FlowerSet::employedBeePhase()
{
   int i;
   Flower *tmp;

   for(i = 0; i < EBEE_NUM; i++) {
      newFlower->change(i);
      if(flower[i]->datastore->fitness > newFlower->datastore->fitness) {
         tmp = newFlower;
         newFlower = flower[i];
         flower[i] = tmp;
      }
      flower[i]->visitNum++;
   }
}

// �Ǐ]�o�`�t�F�[�Y�����s����
void FlowerSet::onlookerBeePhase()
{
   int i, j;
   Flower *tmp;
   double max, min, denom, prob, r;

   for(j = 0; j < OBEE_NUM; j++) {
      // �]���l��ϊ�����
      max = DBL_MIN;
      min = DBL_MAX;
      for(i = 0; i < EBEE_NUM; i++) {
         if(max < flower[i]->datastore->fitness) {
            max = flower[i]->datastore->fitness;
         }
         if(min < flower[i]->datastore->fitness) {
            min = flower[i]->datastore->fitness;
         }
      }
      denom = 0.0;
      for(i = 0; i < EBEE_NUM; i++) {
         trValue[i] = (max - flower[i]->datastore->fitness) / (max - min);
         denom += trValue[i];
      }

      // ���n�o�`��I������
      r = RAND_01;
      for(i = 0; i < EBEE_NUM - 1; i++) {
         prob = trValue[i] / denom;
         if(r <= prob) {
            break;
         }
         r -= prob;
      }

      // ���n�o�`�t�F�[�Y�Ɠ��l�ɏ�������
      newFlower->change(i);
      if(flower[i]->datastore->fitness > newFlower->datastore->fitness) {
         tmp = newFlower;
         newFlower = flower[i];
         flower[i] = tmp;
      }
      flower[i]->visitNum++;
   }
}

// ��@�o�`�t�F�[�Y�����s����
void FlowerSet::scoutBeePhase()
{
   int i;

   for(i = 0; i < EBEE_NUM; i++) {
      if(VISIT_MAX <= flower[i]->visitNum) {
         flower[i]->renew();
      }
   }

}

// �ŗǐH�ƌ����L�^����
void FlowerSet::saveBestPos()
{
   int i, best;

   best = -1;
   for(i = 0; i < EBEE_NUM; i++) {
      if(bestfitness > flower[i]->datastore->fitness) {
         best = i;
      }
   }
   if(best != -1) {
      for(i = 0; i < DataStore::para->zoneVNum; i++) {
		  for (int j = 0; j < DataStore::para->zoneHNum; j++)
			  bestfacility[i][j] = flower[best]->datastore->facility[i][j];
      }
	  bestsubsidy = flower[best]->datastore->subsidy;
      bestfitness = flower[best]->datastore->fitness;
   }
}

// ���ʂ�\������
void FlowerSet::printResult(int trialnum,double timer)
{
	int i, j, best=-1;
	FILE* resultFP;
	double subsidyOne;
	int facilitynum = 0;

	// �t�@�C���I�[�v��
	if ((resultFP = fopen("result.csv", "w")) == NULL) {
		printf("Cannot open result.txt for output.\n");
		exit(1);
	}

	// ������
	subsidyOne = bestsubsidy / (double)DataStore::para->subsidyLevelNum * DataStore::para->subsidyMax;
	fprintf(resultFP, "%.0lf\n", subsidyOne);
	for (j = 0; j < DataStore::para->zoneVNum; j++) {
		for (i = 0; i < DataStore::para->zoneHNum; i++)
			fprintf(resultFP, "%d,", bestfacility[j][i]);
		fprintf(resultFP, "\n");
	}

	// �t�@�C���N���[�Y
	fclose(resultFP);

	for (int vx = 0; vx < DataStore::para->zoneVNum; vx++) {
		for (int vy = 0; vy < DataStore::para->zoneHNum; vy++) {
			if (bestfacility[vx][vy])facilitynum++;
		}
	}

	//DELETE AFTER
	printf("%d,%f,%f,%.0lf,%d\n", trialnum, timer, bestfitness,subsidyOne,facilitynum);
}
