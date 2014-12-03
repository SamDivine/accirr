#ifndef _CUSTOM_COUNTERS_H
#define _CUSTOM_COUNTERS_H

#include "cpucounters.h"
#include "utils.h"
#include <iostream>
#include <stdint.h>

int customPcmInit() {
	set_signal_handlers();
	PCM *m = PCM::getInstance();
	PCM::CustomCoreEventDescription mEvents[4];
	mEvents[0].event_number = DTLB_LOAD_MISS_CAUSE_WALK_EVTNR;
	mEvents[0].umask_value = DTLB_LOAD_MISS_CAUSE_WALK_UMASK;
	mEvents[1].event_number = ARCH_INST_RETIRED_EVTNR;
	mEvents[1].umask_value = ARCH_INST_RETIRED_UMASK;
	mEvents[2].event_number = MEM_LOAD_UOPS_RETIRED_L2_HIT_EVTNR;
	mEvents[2].umask_value = MEM_LOAD_UOPS_RETIRED_L2_HIT_UMASK;
	mEvents[3].event_number = MEM_LOAD_UOPS_RETIRED_L2_MISS_EVTNR;
	mEvents[3].umask_value = MEM_LOAD_UOPS_RETIRED_L2_MISS_UMASK;
	if (m->good()) {
		m->resetPMU();
		m->program(PCM::CUSTOM_CORE_EVENTS, &mEvents);
		return 0;
	} else {
		std::cerr << "can't access PMU" << std::endl;
		return -1;
	}
}

template <class CounterStateType>
void customPcmPrint(CounterStateType& before, CounterStateType& after, double duration) {
	uint64_t mCounts[4];
	for (int i = 0; i < 4; i++) {
		mCounts[i] = getNumberOfCustomEvents(i, before, after);
	}
	double tlbLoadMissPerSec = mCounts[0]/duration;
	double instRetired = mCounts[1];
	double l2Hit = mCounts[2];
	double l2Miss = mCounts[3];
	double l2Load = l2Hit+l2Miss;
	double l2HitRatio = l2Hit/l2Load;
	std::cout << "tlbloadmiss/s " << tlbLoadMissPerSec << " instRetired " << instRetired << " l2hit,load,hitratio " << l2Hit << ", " << l2Load << ", " << l2HitRatio << std::endl;
	std::cerr << "tlbloadmiss/s " << tlbLoadMissPerSec << " instRetired " << instRetired << " l2hit,load,hitratio " << l2Hit << ", " << l2Load << ", " << l2HitRatio << std::endl;
}



#endif //_CUSTOM_COUNTERS_H

