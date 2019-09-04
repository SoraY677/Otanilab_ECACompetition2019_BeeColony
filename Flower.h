#pragma once

#include "DataStore.h"
#include "FlowerSet.h"
class FlowerSet;

// 定数の定義
#define REPEAT_NUM 1000 // 繰返し数
#define EBEE_NUM   100 // 収穫バチの数
#define OBEE_NUM   60   // 追従バチの数
#define VISIT_MAX  200  // 蜜の採取可能回数
#define R_CONSIDER_FACILITY 0.15
#define R_ADJUST_FACILITY 0.95
#define R_CONSIDER_SUBSIDY 0.05
#define R_ADJUST_SUBSIDY 0.8


// 0以上1以下の実数乱数
#define RAND_01 ((double)rand() / RAND_MAX)

class Flower
{
public:
	DataStore* datastore;

    Flower(FlowerSet *argFSet);
    ~Flower();
    void change(int base); // 近くの食糧源に変更する
    void renew();          // 新しい食糧源に変更する

	void writeResult();

    FlowerSet *fSet;       // 属している食糧源集合
    int visitNum;          // 蜜の採取回数

private:
   void evaluate();       // 評価値を算出する
   double *calcFacilityGeneProb(); //施設生成率の計算
};
