#include "Parameter.h"

#ifndef DATA_STORE_H

#define DATA_STORE_H

class DataStore {

public:
	static Parameter* para;

	static int allZoneNum;//全てのゾーン数
	static int centerZoneNum;//都心ゾーン数
	static int surroundingZoneNum;//周辺ゾーン数
	static double transTimeElement;//移動可能なゾーン数
	//移動制限ゾーン数
	//移動制限時間÷隣接ゾーンへの移動時間
	static int rep;
	//フィットネスを出す際のベース
	//一分あたりの移動にかかるコスト×隣接するゾーンへの移動時間×サービス受給率 × 0.73;
	static double fit;
	//施設生成率の最低確率(全体ゾーンに対する必要最低限の施設割合を基準値とする)
	static double refervalue;

	int** facility;//施設があるかどうかの二次元配列
	int** housePop;//各ゾーンの人口
	double** facilityPop;//各施設の1日当たりの使用人数
	int subsidy;//移転補助費用(段階数)
	int moveNum;//移転数
	int facilityNum;//施設数
	double moveProb;//移転確率
	double fitness;//最終的な財政支出
	double distancetoCity[4];//4隅から都心部までのゾーン数

	DataStore();
	~DataStore();
};
#endif
