#include "FlowerSet.h"
#include "time.h"

#define TRIAL_NUM 10//DELETE AFTER

int main()
{
   int i;
   FlowerSet *fSet;
   clock_t start_c, stop_c;

   srand((unsigned int)time(NULL));
   
   for (int trial = 0; trial < TRIAL_NUM; trial++) {//DELETE AFTER
	   start_c = clock();//DELETE AFTER

	   fSet = new FlowerSet();
	   for (i = 1; i <= REPEAT_NUM; i++) {
		   fSet->employedBeePhase();
		   fSet->onlookerBeePhase();
		   fSet->scoutBeePhase();
		   fSet->saveBestPos();
		   fSet->tryNum++;
	   }

	   stop_c = clock();//DELETE AFTER
	   fSet->printResult(trial+1,(double)(stop_c-start_c)/CLOCKS_PER_SEC);//DELETE AFTER
   }//DELETE AFTER
   
   delete fSet;

   return 0;
}
