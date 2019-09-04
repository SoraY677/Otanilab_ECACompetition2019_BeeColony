#include "DataStore.h"

Parameter* DataStore::para = new Parameter();
int DataStore::allZoneNum = para->zoneVNum * para->zoneHNum;
int DataStore::centerZoneNum = (para->centerZoneVID2 - para->centerZoneVID1 + 1) * (para->centerZoneHID2 - para->centerZoneHID1 + 1);
int DataStore::surroundingZoneNum = allZoneNum - centerZoneNum;
int DataStore::rep = para->transTimeLimit / para->transTimeUnit;
double DataStore::fit = para->transCostUnit * (double)para->transTimeUnit * para->receiveRatio * 0.73;
double DataStore::refervalue = para->initialZonePopNum * para->receiveRatio / para->facilityCapacity;
double DataStore::transTimeElement=  para->transTimeLimit / para->transTimeUnit;


DataStore::DataStore() {
	facility = 0;
	facilityNum = 0;
	facilityPop = 0;
	fitness = 0;
	housePop = 0;
	moveNum = 0;
	moveProb = 0;
	subsidy = 0;
	distancetoCity[0] = para->centerZoneHID1 + (double)para->centerZoneVID1;
	distancetoCity[1] = (double)para->zoneHNum - 1 - (double)para->centerZoneHID2 + para->centerZoneVID1;
	distancetoCity[2] = (double)para->zoneHNum - 1 - (double)para->centerZoneHID2 + para->zoneVNum - 1 - para->centerZoneVID2;
	distancetoCity[3] = para->centerZoneHID1 + (double)para->zoneVNum - 1 - para->centerZoneVID2;
}

DataStore::~DataStore() {

}