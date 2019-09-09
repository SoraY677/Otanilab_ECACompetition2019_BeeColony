#include "Flower.h"

// �R���X�g���N�^
// argFSet: �����Ă���H�ƌ��W��
Flower::Flower(FlowerSet* argFSet)
{
	double r;
	double facilitygeneprob = 0.0;//�s�S�]�[�������m��
	double* facilitygeneprobtmp;
	double facilitygeneprobtmp2[2];

	fSet = argFSet;
	datastore = new DataStore();
	datastore->facility = new int* [datastore->para->zoneVNum];
	datastore->housePop = new int* [datastore->para->zoneVNum];
	datastore->facilityPop = new double* [datastore->para->zoneVNum];

	//�m���ŏ����������߂�
	datastore->subsidy = rand() % (datastore->para->subsidyLevelNum + 1);

	//�ړ]�m�������߂�
	datastore->moveProb = 1 / (1 + exp(-0.1 * ((double)datastore->subsidy / (double)datastore->para->subsidyLevelNum * datastore->para->subsidyMax) + 5));

	//�e�]�[���̎{�ݐ����m�������߂�
	facilitygeneprobtmp = calcFacilityGeneProb();

	//HACK:�������Ȃ��ƃ��������j�󂳂��I�I�I
	facilitygeneprobtmp2[0] = facilitygeneprobtmp[0];
	facilitygeneprobtmp2[1] = facilitygeneprobtmp[1];

	for (int i = 0; i < datastore->para->zoneVNum; i++) {
		datastore->facility[i] = new int[datastore->para->zoneHNum];
		datastore->housePop[i] = new int[datastore->para->zoneHNum];
		datastore->facilityPop[i] = new double[datastore->para->zoneHNum];
		
		for (int j = 0; j < datastore->para->zoneHNum; j++) {

			//�s�S�]�[���E���Ӄ]�[���ɏd�݂�t����
			if (i >= datastore->para->centerZoneVID1 && i <= datastore->para->centerZoneVID2) {
				if (j >= datastore->para->centerZoneHID1 && j <= datastore->para->centerZoneHID2)
					facilitygeneprob = facilitygeneprobtmp2[0];
			}
			else
				facilitygeneprob = facilitygeneprobtmp2[1];

			//�d�݂ɂ��m���Ŏ{�݂����Ă�
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

	//�]������
	evaluate();
}

// �f�X�g���N�^
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

//�e�]�[���ɑ΂���{�ݔ����m���̎Z�o
//return:facilitygeneprob�@�s�S�]�[���ɑ΂���{�݂̔����m��
double* Flower::calcFacilityGeneProb() {

	double townarearesidentamount, townarearesidentprob;//�ŏI�I�ɓs�S�]�[���ɈڏZ����l�̐��Ɗ���
	double townareaweight;//�s�S�̏d��
	double borderprob[2] = { 0, 0 };
	double borderprobtmp[2] = {0,0};

	//�Z���S�̐��ɑ΂��čŏI�I�ɓs�S�ɋ��Z����Z�����Ɗ������v�Z(�����e�J�����@�Ɋ�Â��T�Z)
	//�s�S�]�[���̍ŏI�Z����( �ړ]�m�� * ���Ӄ]�[���� * �����]�[����) + ���Ƃ��Ɠs�S�]�[���ɏZ��ł����Z����(�s�S�]�[����*�����]�[����)
	townarearesidentamount
		= (datastore->moveProb * datastore->surroundingZoneNum * datastore->para->initialZonePopNum + datastore->centerZoneNum * (double)datastore->para->initialZonePopNum);
	//�s�S�]�[���̍ŏI�Z���� / �S�̐�
	townarearesidentprob = townarearesidentamount / (datastore->para->zoneHNum * (double)datastore->para->zoneVNum * datastore->para->initialZonePopNum);

	//�s�S�]�[���̏d�݂����߂�
	townareaweight = townarearesidentprob * (1.0 - datastore->para->receiveRatio);
	for (int i = 0; i < 2; i++) {
		switch (i){
		case 0:
			//-----------�ړ����Ԃɂ�����{�[�_�[�����߂�
			borderprobtmp[0] = 1 / datastore->transTimeElement;
			//���Ӄ]�[���ɂ��Ē[��������s�S�]�[���ւ̋�����
			//�ړ��ł���]�[������葽�����Ŕ�r����
			for (int i = 0; i < 4; i++) {
				//�s�S�]�[���܂ł̋������ړ��\�ȃ]�[������菭�Ȃ��Ȃ��������ꍇ��
				//���Ӄ]�[���̎{�ݐ����m�����ړ��\�]�[�����Ɉˑ�����
				if (datastore->distancetoCity[i] >= datastore->transTimeElement) {
					borderprobtmp[1] = 1 / datastore->transTimeElement;
					break;
				}
			}
			break;
		case 1:
			//-----------�K�v�{�ݐ���2��ނ̃]�[���̊����ɂ��Ẵ{�[�_�[�����߂�
		//���Ӄ]�[���ł̎{�ݎg�p�������Z�o���Ă���
			borderprobtmp[1] = datastore->para->initialZonePopNum * datastore->para->receiveRatio * (1 - datastore->moveProb) / datastore->para->facilityCapacity;
			//�s�S�]�[���ł̎{�ݎg�p�����ɂ���
			borderprobtmp[0] = townarearesidentamount * datastore->para->receiveRatio / datastore->para->facilityCapacity / datastore->centerZoneNum;

			//�s�S�]�[���ł̎{�ݎ��v���s�S�]�[���S�ĂɎ{�݂����ĂĂ�����Ȃ��ƂȂ����ꍇ�D
			if (borderprobtmp[0] > 1) {
				//�s�S�]�[���S�Ă̌��݂͊m��
				borderprob[0] = 1;
				//�I�[�o�[�����s�S�]�[���̐l�̕��Ǝ��Ӄ]�[���̐l�̕��̎{�ݐ��������v�Z
				//(�s�S�]�[���ł̎{�ݎg�p���� - 1) * �s�S�]�[����  / ���Ӄ]�[���� = �s�����Ă���{�݊��� + 
				//���Ӄ]�[���̕K�v�{�ݐ��̊���
				borderprobtmp[1] = (borderprobtmp[0] - 1) * datastore->centerZoneNum / datastore->surroundingZoneNum
					+ borderprobtmp[1];
			}
			break;
		}
		
		borderprob[0] = (borderprob[0] < borderprobtmp[0]) ? borderprobtmp[0] : borderprob[0];
		borderprob[1] = (borderprob[1] < borderprobtmp[1]) ? borderprobtmp[1] : borderprob[1];
		
	}

	//�{�ݐ����m���̌���()
	borderprob[0] = borderprob[0] + datastore->refervalue * townareaweight;
	borderprob[1] = borderprob[1] + (1 - datastore->refervalue) * townareaweight;

	return borderprob;
}



// base�̋߂��̐H�ƌ��ɕύX����
// base: ���Ƃɂ���H�ƌ��̓Y����
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

// �V�����H�ƌ��ɕύX����
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

// �]���l���Z�o����
void Flower::evaluate()
{
	int i, j, k, x, y, neighbour, fromv, fromh, to, tov, toh;
	double oneDayPop;

	datastore->moveProb = 1 / (1 + exp(-0.1 * ((double)datastore->subsidy / (double)datastore->para->subsidyLevelNum * datastore->para->subsidyMax) + 5));
	
	// �{�ݐ��̎Z�o
	datastore->facilityNum = 0;
	for (i = 0; i < datastore->para->zoneVNum; i++) {
		for (j = 0; j < datastore->para->zoneHNum; j++) {
			datastore->facilityPop[i][j] = 0.0;
			datastore->facilityNum += datastore->facility[i][j];
			datastore->housePop[i][j] = datastore->para->initialZonePopNum;
		}
	}

	// ���Z�Ґ��̎Z�o(�}�C�N���V�~�����[�V����)
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

	// ��앟���{�ݐV��/�ێ��Ǘ���p�{�ړ]�⏕��p
	datastore->fitness = datastore->para->runningCost * (double)datastore->facilityNum + ((double)datastore->subsidy / (double)datastore->para->subsidyLevelNum * datastore->para->subsidyMax) * datastore->moveNum;

	// �ڑ��T�[�r�X��p
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