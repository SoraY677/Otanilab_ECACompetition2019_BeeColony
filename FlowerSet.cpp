#include "FlowerSet.h"

// コンストラクタ
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

// デストラクタ
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

// 収穫バチフェーズを実行する
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

// 追従バチフェーズを実行する
void FlowerSet::onlookerBeePhase()
{
   int i, j;
   Flower *tmp;
   double max, min, denom, prob, r;

   for(j = 0; j < OBEE_NUM; j++) {
      // 評価値を変換する
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

      // 収穫バチを選択する
      r = RAND_01;
      for(i = 0; i < EBEE_NUM - 1; i++) {
         prob = trValue[i] / denom;
         if(r <= prob) {
            break;
         }
         r -= prob;
      }

      // 収穫バチフェーズと同様に処理する
      newFlower->change(i);
      if(flower[i]->datastore->fitness > newFlower->datastore->fitness) {
         tmp = newFlower;
         newFlower = flower[i];
         flower[i] = tmp;
      }
      flower[i]->visitNum++;
   }
}

// 偵察バチフェーズを実行する
void FlowerSet::scoutBeePhase()
{
   int i;

   for(i = 0; i < EBEE_NUM; i++) {
      if(VISIT_MAX <= flower[i]->visitNum) {
         flower[i]->renew();
      }
   }

}

// 最良食糧源を記録する
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

// 結果を表示する
void FlowerSet::printResult(int trialnum,double timer)
{
	int i, j, best=-1;
	FILE* resultFP;
	double subsidyOne;
	int facilitynum = 0;

	// ファイルオープン
	if ((resultFP = fopen("result.csv", "w")) == NULL) {
		printf("Cannot open result.txt for output.\n");
		exit(1);
	}

	// 書込み
	subsidyOne = bestsubsidy / (double)DataStore::para->subsidyLevelNum * DataStore::para->subsidyMax;
	fprintf(resultFP, "%.0lf\n", subsidyOne);
	for (j = 0; j < DataStore::para->zoneVNum; j++) {
		for (i = 0; i < DataStore::para->zoneHNum; i++)
			fprintf(resultFP, "%d,", bestfacility[j][i]);
		fprintf(resultFP, "\n");
	}

	// ファイルクローズ
	fclose(resultFP);

	for (int vx = 0; vx < DataStore::para->zoneVNum; vx++) {
		for (int vy = 0; vy < DataStore::para->zoneHNum; vy++) {
			if (bestfacility[vx][vy])facilitynum++;
		}
	}

	//DELETE AFTER
	printf("%d,%f,%f,%.0lf,%d\n", trialnum, timer, bestfitness,subsidyOne,facilitynum);
}
