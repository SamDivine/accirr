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
#define SETEVENT(idx, name) \
	mEvents[idx].event_number = name##_EVTNR;\
	mEvents[idx].umask_value = name##_UMASK;
/* L2 Hit and L3 Hit */
	SETEVENT(0, ARCH_LLC_MISS);
	SETEVENT(1, MEM_LOAD_UOPS_LLC_HIT_RETIRED_XSNP_NONE);
	SETEVENT(2, MEM_LOAD_UOPS_LLC_HIT_RETIRED_XSNP);
	SETEVENT(3, MEM_LOAD_UOPS_RETIRED_L2_HIT);

/*
	SETEVENT(0, ARCH_LLC_REFERENCE);
	SETEVENT(1, ARCH_INST_RETIRED);
	SETEVENT(2, DTLB_LOAD_MISS_CAUSE_WALK);
	SETEVENT(3, UOPS_RETIRED_ALL);
*/
#undef SETEVENT

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
/* L2 Hit and L3 Hit */
	double l3Miss = mCounts[0];
	double l3UnsharedHit = mCounts[1];
	double l2HitM = mCounts[2];
	double l2Hit = mCounts[3];

	double l3Hit = l2HitM+l3UnsharedHit;
	double l3Load = l3Hit+l3Miss;
	double l2Load = l3Load+l2Hit;
	double l2HitRatio = l2Hit/l2Load;
	double l3HitRatio = l3Hit/l3Load;
	fprintf(stdout, "l3hit load hitratio %g %g %g l2hit load hitratio %g %g %g\n", l3Hit, l3Load, l3HitRatio, l2Hit, l2Load, l2HitRatio);
	fprintf(stderr, "l3hit load hitratio %g %g %g l2hit load hitratio %g %g %g\n", l3Hit, l3Load, l3HitRatio, l2Hit, l2Load, l2HitRatio);
/*
	double llcRef = mCounts[0];
	double instRetired = mCounts[1];
	double tlbLoadMiss = mCounts[2];
	double uopsRetired = mCounts[3];

	double tlbLoadMissPerSec = tlbLoadMiss/duration;
	fprintf(stdout, "llcRef %g instRetired %g tlbLoadMissPerSec %g uopsRetired %g\n", llcRef, instRetired, tlbLoadMissPerSec, uopsRetired);
	fprintf(stderr, "llcRef %g instRetired %g tlbLoadMissPerSec %g uopsRetired %g\n", llcRef, instRetired, tlbLoadMissPerSec, uopsRetired);
*/

}



#endif //_CUSTOM_COUNTERS_H

