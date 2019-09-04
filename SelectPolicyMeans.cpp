#include "FlowerSet.h"
#include "time.h"

#define TRIAL_NUM 3

int main()
{
   int i;
   FlowerSet *fSet;
   clock_t start_c, stop_c;

   srand((unsigned int)time(NULL));

   printf("‰ñ”,Œv‘ªŠÔ,Å—Ç’l\n");
   
   for (int trial = 0; trial < TRIAL_NUM; trial++) {
	   start_c = clock();

	   fSet = new FlowerSet();
	   for (i = 1; i <= REPEAT_NUM; i++) {
		   fSet->employedBeePhase();
		   fSet->onlookerBeePhase();
		   fSet->scoutBeePhase();
		   fSet->saveBestPos();
		   fSet->tryNum++;
	   }
	   stop_c = clock();
	   printf("%d,%f,%f\n",trial+1, (double)(stop_c - start_c) / CLOCKS_PER_SEC,fSet->bestfitness);

   }
   fSet->printResult();
   delete fSet;

   return 0;
}
