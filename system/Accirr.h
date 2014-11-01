#ifndef _ACCIRR_H
#define _ACCIRR_H

#include "TaskScheduler.h"

void AccirrInit(int *argc_c, char ** argv_p[]);

int AccirrFinalize();

void createTask(thread_func f, void *arg);

void AccirrRun();

#endif //_ACCIRR_H


