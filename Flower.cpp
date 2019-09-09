#include "Flower.h"

// コンストラクタ
// argFSet: 属している食糧源集合
Flower::Flower(FlowerSet* argFSet)
{
	double r;
	double facilitygeneprob = 0.0;//都心ゾーン生成確率
	double* facilitygeneprobtmp;
	double facilitygeneprobtmp2[2];

	fSet = argFSet;
	datastore = new DataStore();
	datastore->facility = new int* [datastore->para->zoneVNum];
	datastore->housePop = new int* [datastore->para->zoneVNum];
	datastore->facilityPop = new double* [datastore->para->zoneVNum];

	//確率で助成金をきめる
	datastore->subsidy = rand() % (datastore->para->subsidyLevelNum + 1);

	//移転確率を求める
	datastore->moveProb = 1 / (1 + exp(-0.1 * ((double)datastore->subsidy / (double)datastore->para->subsidyLevelNum * datastore->para->subsidyMax) + 5));

	//各ゾーンの施設生成確率を求める
	facilitygeneprobtmp = calcFacilityGeneProb();

	//HACK:こうしないとメモリが破壊される！！！
	facilitygeneprobtmp2[0] = facilitygeneprobtmp[0];
	facilitygeneprobtmp2[1] = facilitygeneprobtmp[1];

	for (int i = 0; i < datastore->para->zoneVNum; i++) {
		datastore->facility[i] = new int[datastore->para->zoneHNum];
		datastore->housePop[i] = new int[datastore->para->zoneHNum];
		datastore->facilityPop[i] = new double[datastore->para->zoneHNum];
		
		for (int j = 0; j < datastore->para->zoneHNum; j++) {

			//都心ゾーン・周辺ゾーンに重みを付ける
			if (i >= datastore->para->centerZoneVID1 && i <= datastore->para->centerZoneVID2) {
				if (j >= datastore->para->centerZoneHID1 && j <= datastore->para->centerZoneHID2)
					facilitygeneprob = facilitygeneprobtmp2[0];
			}
			else
				facilitygeneprob = facilitygeneprobtmp2[1];

			//重みによる確率で施設を建てる
			r = (double)rand() / RAND_MAX;
			if (r < facilitygeneprob)
				datastore->facility[i][j] = 1;
			else
				datastore->facility[i][j] = 0;

			datastore->housePop[i][j] = datastore->para->initialZonePopNum;
			datastore->facilityPop[i][j] = 0.0;
		}
	}

	datastore->moveNum = 0;
	datastore->facilityNum = 0;

	//評価する
	evaluate();
}

// デストラクタ
Flower::~Flower()
{
	for (int i = 0; i < datastore->para->zoneVNum; i++) {
		delete[] datastore->facility[i];
		delete[] datastore->housePop[i];
		delete[] datastore->facilityPop[i];
	}
	delete[] datastore->facility;
	delete[] datastore->housePop;
	delete[] datastore->facilityPop;
}

//各ゾーンに対する施設発生確率の算出
//return:facilitygeneprob　都心ゾーンに対する施設の発生確率
double* Flower::calcFacilityGeneProb() {

	double townarearesidentamount, townarearesidentprob;//最終的に都心ゾーンに移住する人の数と割合
	double townareaweight;//都心の重み
	double borderprob[2] = { 0, 0 };
	double borderprobtmp[2] = {0,0};

	//住民全体数に対して最終的に都心に居住する住民数と割合を計算(モンテカルロ法に基づく概算)
	//都心ゾーンの最終住民数( 移転確立 * 周辺ゾーン数 * 初期ゾーン数) + もともと都心ゾーンに住んでいた住民数(都心ゾーン数*初期ゾーン数)
	townarearesidentamount
		= (datastore->moveProb * datastore->surroundingZoneNum * datastore->para->initialZonePopNum + datastore->centerZoneNum * (double)datastore->para->initialZonePopNum);
	//都心ゾーンの最終住民数 / 全体数
	townarearesidentprob = townarearesidentamount / (datastore->para->zoneHNum * (double)datastore->para->zoneVNum * datastore->para->initialZonePopNum);

	//都心ゾーンの重みを求める
	townareaweight = townarearesidentprob * (1.0 - datastore->para->receiveRatio);
	for (int i = 0; i < 2; i++) {
		switch (i){
		case 0:
			//-----------移動時間におけるボーダーを求める
			borderprobtmp[0] = 1 / datastore->transTimeElement;
			//周辺ゾーンについて端っこから都心ゾーンへの距離が
			//移動できるゾーン数より多いかで比較する
			for (int i = 0; i < 4; i++) {
				//都心ゾーンまでの距離が移動可能なゾーン数より少ないないし同じ場合は
				//周辺ゾーンの施設生成確率が移動可能ゾーン数に依存する
				if (datastore->distancetoCity[i] >= datastore->transTimeElement) {
					borderprobtmp[1] = 1 / datastore->transTimeElement;
					break;
				}
			}
			break;
		case 1:
			//-----------必要施設数と2種類のゾーンの割合についてのボーダーを求める
		//周辺ゾーンでの施設使用割合を算出しておく
			borderprobtmp[1] = datastore->para->initialZonePopNum * datastore->para->receiveRatio * (1 - datastore->moveProb) / datastore->para->facilityCapacity;
			//都心ゾーンでの施設使用割合について
			borderprobtmp[0] = townarearesidentamount * datastore->para->receiveRatio / datastore->para->facilityCapacity / datastore->centerZoneNum;

			//都心ゾーンでの施設需要が都心ゾーン全てに施設を建てても足りないとなった場合．
			if (borderprobtmp[0] > 1) {
				//都心ゾーン全ての建設は確定
				borderprob[0] = 1;
				//オーバーした都心ゾーンの人の分と周辺ゾーンの人の分の施設生成率を計算
				//(都心ゾーンでの施設使用割合 - 1) * 都心ゾーン数  / 周辺ゾーン数 = 不足している施設割合 + 
				//周辺ゾーンの必要施設数の割合
				borderprobtmp[1] = (borderprobtmp[0] - 1) * datastore->centerZoneNum / datastore->surroundingZoneNum
					+ borderprobtmp[1];
			}
			break;
		}
		
		borderprob[0] = (borderprob[0] < borderprobtmp[0]) ? borderprobtmp[0] : borderprob[0];
		borderprob[1] = (borderprob[1] < borderprobtmp[1]) ? borderprobtmp[1] : borderprob[1];
		
	}

	//施設生成確率の決定()
	borderprob[0] = borderprob[0] + datastore->refervalue * townareaweight;
	borderprob[1] = borderprob[1] + (1 - datastore->refervalue) * townareaweight;

	return borderprob;
}



// baseの近くの食糧源に変更する
// base: もとにする食糧源の添え字
void Flower::change(int base)
{
	int i, j;
	for (i = 0; i < datastore->para->zoneVNum; i++) {
		for (j = 0; j < datastore->para->zoneHNum; j++) {
			datastore->facility[i][j] = fSet->flower[base]->datastore->facility[i][j];
				if (rand() > RAND_MAX * R_ADJUST_FACILITY)
					datastore->facility[i][j] = (fSet->flower[base]->datastore->facility[i][j] + 1) % 2;
		}
	}

	datastore->subsidy = fSet->flower[base]->datastore->subsidy;
	if (rand() > RAND_MAX * R_ADJUST_SUBSIDY) {

		if (fSet->flower[base]->datastore->subsidy == 0)
			datastore->subsidy = 1;
		else if (fSet->flower[base]->datastore->subsidy == datastore->para->subsidyLevelNum)
			datastore->subsidy = fSet->flower[base]->datastore->para->subsidyLevelNum - 1;
		else
			datastore->subsidy = fSet->flower[base]->datastore->subsidy + (rand() % 2) * 2 - 1;
	
	}
	
   

   visitNum = 0;
   evaluate();
}

// 新しい食糧源に変更する
void Flower::renew()
{
   int i,j;

   for (i = 0; i < datastore->para->zoneVNum; i++) {
	   for (j = 0; j < datastore->para->zoneHNum; j++) {
		   if (rand() < (R_CONSIDER_FACILITY + 0.0001 * fSet->tryNum) * RAND_MAX ) {
			   if (rand() > RAND_MAX * datastore->refervalue) datastore->facility[i][j] = 0;
			   else datastore->facility[i][j] = 1;
		   }
	   }
   }

   if (rand() < (R_CONSIDER_SUBSIDY + 0.00001 * fSet->tryNum)* RAND_MAX)
	   datastore->subsidy = rand() % (datastore->para->subsidyLevelNum + 1);

   visitNum = 0;
   evaluate();
}

// 評価値を算出する
void Flower::evaluate()
{
	int i, j, k, x, y, neighbour, fromv, fromh, to, tov, toh;
	double oneDayPop;

	datastore->moveProb = 1 / (1 + exp(-0.1 * ((double)datastore->subsidy / (double)datastore->para->subsidyLevelNum * datastore->para->subsidyMax) + 5));
	
	// 施設数の算出
	datastore->facilityNum = 0;
	for (i = 0; i < datastore->para->zoneVNum; i++) {
		for (j = 0; j < datastore->para->zoneHNum; j++) {
			datastore->facilityPop[i][j] = 0.0;
			datastore->facilityNum += datastore->facility[i][j];
			datastore->housePop[i][j] = datastore->para->initialZonePopNum;
		}
	}

	// 居住者数の算出(マイクロシミュレーション)
	datastore->moveNum = 0;
	for (fromv = 0; fromv < datastore->para->centerZoneVID1; fromv++) {
		for (fromh = 0; fromh < datastore->para->zoneHNum; fromh++) {
			for (i = 0; i < datastore->para->initialZonePopNum; i++) {
				if ((double)rand() / (double)RAND_MAX < datastore->moveProb) {
					datastore->housePop[fromv][fromh]--;
					to = rand() % datastore->centerZoneNum;
					toh = to % (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneHID1;
					tov = to / (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneVID1;
					datastore->housePop[tov][toh]++;
					datastore->moveNum++;
				}
			}
		}
	}
	for (; fromv <= datastore->para->centerZoneVID2; fromv++) {
		for (fromh = 0; fromh < datastore->para->centerZoneHID1; fromh++) {
			for (i = 0; i < datastore->para->initialZonePopNum; i++) {
				if ((double)rand() / (double)RAND_MAX < datastore->moveProb) {
					datastore->housePop[fromv][fromh]--;
					to = rand() % datastore->centerZoneNum;
					toh = to % (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneHID1;
					tov = to / (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneVID1;
					datastore->housePop[tov][toh]++;
					datastore->moveNum++;
				}
			}
		}
		for (fromh = datastore->para->centerZoneHID2 + 1; fromh < datastore->para->zoneHNum; fromh++) {
			for (i = 0; i < datastore->para->initialZonePopNum; i++) {
				if ((double)rand() / (double)RAND_MAX < datastore->moveProb) {
					datastore->housePop[fromv][fromh]--;
					to = rand() % datastore->centerZoneNum;
					toh = to % (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneHID1;
					tov = to / (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneVID1;
					datastore->housePop[tov][toh]++;
					datastore->moveNum++;
				}
			}
		}
	}
	for (; fromv < datastore->para->zoneVNum; fromv++) {
		for (fromh = 0; fromh < datastore->para->zoneHNum; fromh++) {
			for (i = 0; i < datastore->para->initialZonePopNum; i++) {
				if ((double)rand() / (double)RAND_MAX < datastore->moveProb) {
					datastore->housePop[fromv][fromh]--;
					to = rand() % datastore->centerZoneNum;
					toh = to % (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneHID1;
					tov = to / (datastore->para->centerZoneHID2 - datastore->para->centerZoneHID1 + 1) + datastore->para->centerZoneVID1;
					datastore->housePop[tov][toh]++;
					datastore->moveNum++;
				}
			}
		}
	}

	// 介護福祉施設新設/維持管理費用＋移転補助費用
	datastore->fitness = datastore->para->runningCost * (double)datastore->facilityNum + ((double)datastore->subsidy / (double)datastore->para->subsidyLevelNum * datastore->para->subsidyMax) * datastore->moveNum;

	// 移送サービス費用
	for (y = 0; y < datastore->para->zoneVNum; y++) {
		for (x = 0; x < datastore->para->zoneHNum; x++) {
			if (datastore->housePop[y][x]) {
				if (datastore->facility[y][x]) {
					oneDayPop = datastore->housePop[y][x] * datastore->para->receiveRatio;
					datastore->facilityPop[y][x] += oneDayPop;
					if (datastore->facilityPop[y][x] > datastore->para->facilityCapacity) {
						datastore->fitness = DBL_MAX;
						return;
					}
					else {
						datastore->fitness += datastore->para->transCostUnit * (double)datastore->para->transTimeUnit * oneDayPop * 0.365;
					}
				}
				else {
					neighbour = 0;
					for (i = 1; i <= datastore->rep; i++) {
						for (j = y - i, k = x; j < y; j++, k++)
							if ((0 <= j) && (k < datastore->para->zoneHNum) && datastore->facility[j][k])
								neighbour++;
						for (; j < y + i; j++, k--)
							if ((j < datastore->para->zoneVNum) && (k < datastore->para->zoneHNum) && datastore->facility[j][k])
								neighbour++;
						for (; j > y; j--, k--)
							if ((j < datastore->para->zoneVNum) && (0 <= k) && datastore->facility[j][k])
								neighbour++;
						for (; j > y - i; j--, k++)
							if ((0 <= j) && (0 <= k) && datastore->facility[j][k])
								neighbour++;
						if (neighbour) {
							oneDayPop = datastore->housePop[y][x] * datastore->para->receiveRatio / neighbour;
							for (j = y - i, k = x; j < y; j++, k++) {
								if ((0 <= j) && (k < datastore->para->zoneHNum) && datastore->facility[j][k]) {
									datastore->facilityPop[j][k] += oneDayPop;
									if (datastore->facilityPop[j][k] > datastore->para->facilityCapacity) {
										datastore->fitness = DBL_MAX;
										return;
									}
								}
							}
							for (; j < y + i; j++, k--) {
								if ((j < datastore->para->zoneVNum) && (k < datastore->para->zoneHNum) && datastore->facility[j][k]) {
									datastore->facilityPop[j][k] += oneDayPop;
									if (datastore->facilityPop[j][k] > datastore->para->facilityCapacity) {
										datastore->fitness = DBL_MAX;
										return;
									}
								}
							}
							for (; j > y; j--, k--) {
								if ((j < datastore->para->zoneVNum) && (0 <= k) && datastore->facility[j][k]) {
									datastore->facilityPop[j][k] += oneDayPop;
									if (datastore->facilityPop[j][k] > datastore->para->facilityCapacity) {
										datastore->fitness = DBL_MAX;
										return;
									}
								}
							}
							for (; j > y - i; j--, k++) {
								if ((0 <= j) && (0 <= k) && datastore->facility[j][k]) {
									datastore->facilityPop[j][k] += oneDayPop;
									if (datastore->facilityPop[j][k] > datastore->para->facilityCapacity) {
										datastore->fitness = DBL_MAX;
										return;
									}
								}
							}
							datastore->fitness += i * (double)datastore->housePop[y][x] * datastore->fit;
							break;
						}
					}
					if (!neighbour) {
						datastore->fitness = DBL_MAX;
						return;
					}
				}
			}
		}
	}
}