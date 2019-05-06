#include "MyTime.h"

bool timeBiggerThan(struct tm* a, struct tm* b){
	
	if(a->tm_year > b->tm_year){
		return true;
	} else if(a->tm_year < b->tm_year){
		return false;
	}

	if(a->tm_mon > b->tm_mon){
		return true;
	} else if(a->tm_mon < b->tm_mon){
		return false;
	}

	if(a->tm_mday > b->tm_mday){
		return true;
	} else if(a->tm_mday < b->tm_mday){
		return false;
	}
	
	if(a->tm_hour > b->tm_hour){
		return true;
	} else if(a->tm_hour < b->tm_hour){
		return false;
	}

	if(a->tm_min > b->tm_min){
		return true;
	} else if(a->tm_min < b->tm_min){
		return false;
	}

	if(a->tm_sec > b->tm_sec){
		return true;
	} else if(a->tm_sec < b->tm_sec){
		return false;
	}
	
	return false;
}

/*printf("b: %d %d %d %d %d %d\n", b->tm_mday, b->tm_mon, b->tm_year, b->tm_hour, b->tm_min, b->tm_sec);*/