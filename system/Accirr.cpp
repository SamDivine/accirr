#include "Accirr.h"


Worker *master_thread;

#ifdef WEBUI
double programStart;
double prevPoint;
double presentPoint;
unsigned long long maxWorkerNum;
unsigned long long contextSwitches;
unsigned long long finishedTasks;
char programName[256];
sqlite3 *db;
char* errMsg;

char dbFile[] = "/home/localhost/accirr/webui/accirr.db";
char timeInText[20];
void getSystemTime(timeval pNow) {
	int year = 1970, month = 1, day = 1, hour = 0, minute = 0, second = 0;
	int daySec = 86400;
	int daysMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	long pSec = pNow.tv_sec;
	pSec += 8*3600;
	long pUsec= pNow.tv_usec;
	int dayMore = ((year%100==0 && year%400==0) || year%4==0);
	int daysYear = dayMore + 365;
	int daysPassed = pSec / daySec;
	while (daysPassed >= daysYear) {
		daysPassed -= daysYear;
		year++;
		dayMore = ((year%100==0 && year%400==0) || year%4==0);
		daysYear = dayMore + 365;
	}
	int realDays;
	while ((realDays = daysMonth[month-1]+(month==2&&dayMore==1)) <= daysPassed) {
		daysPassed -= realDays;
		month++;
	}
	day += daysPassed;
	int secInDay = pSec % daySec;
	hour = secInDay/3600;
	secInDay %= 3600;
	minute = secInDay/60;
	secInDay %= 60;
	second = secInDay;
	int ms = pUsec/1000;
	sprintf(timeInText, "%d%.2d%.2d%.2d%.2d%.2d%.3d", year, month, day, hour, minute, second, ms);
}
#endif

void AccirrInit(int *argc_p, char **argv_p[]) {
	master_thread = convert_to_master();
#ifdef WEBUI
	timeval pNow;
	programStart = asecd(&pNow);
	prevPoint = programStart;
	maxWorkerNum = 0;
	contextSwitches = 0;
	finishedTasks = 0;
	errMsg = NULL;
	getSystemTime(pNow);
	sprintf(programName, "%s_%s", strrchr((*argv_p)[0], '/')+1, timeInText);
	for (int i = 1; i < *argc_p; i++) {
		sprintf(programName, "%s_%s", programName, (*argv_p)[i]);
	}
	int rc;
	if ((rc = sqlite3_open(dbFile, &db)) != SQLITE_OK) {
		fprintf(stderr, "Can't open database %s: %s\n", dbFile, sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(-1);
	}
	if ((rc = sqlite3_exec(db, "CREATE TABLE RUNNING_TBL (PROGRAM TEXT PRIMARY KEY, STARTTIME TEXT);", NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at create running table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	char cmd[256];
	sprintf(cmd, "INSERT INTO RUNNING_TBL (PROGRAM, STARTTIME) VALUES ('%s', %s);", programName, timeInText);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at insert into running table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	if ((rc = sqlite3_exec(db, "CREATE TABLE TASKTRACK_TBL (ATIME REAL PRIMARY KEY, PROGRAM TEXT, EXECTIME REAL, WORKERS INTEGER, FINISHEDTASKS INTEGER, CONTEXTSWITCHES INTEGER);", NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at create tasktrack table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sprintf(cmd, "INSERT INTO TASKTRACK_TBL (ATIME, PROGRAM, EXECTIME, WORKERS, FINISHEDTASKS, CONTEXTSWITCHES) VALUES (%.2f, '%s', 0.0, %llu, %llu, 0);", programStart, programName, maxWorkerNum, finishedTasks);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at first insert into tasktrack table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
#endif

	global_scheduler.init(master_thread);
}

void createTask(thread_func f, void *arg) {
	Worker *t = worker_spawn(global_scheduler.get_current_thread(), &global_scheduler, f, arg);
	global_scheduler.ready(t);
}

int AccirrFinalize() {
#ifdef WEBUI
	timeval pNow;
	presentPoint = asecd(&pNow);
	double execTime = presentPoint-programStart;
	int rc;
	if ((rc = sqlite3_exec(db, "CREATE TABLE FINISHED_TBL (PROGRAM TEXT PRIMARY KEY, EXECTIME REAL, MAXWORKERS INTEGER, FINISHTIME TEXT, FINISHEDTASKS IETEGER);", NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at create finished table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	char cmd[256];
	getSystemTime(pNow);
	sprintf(cmd, "INSERT INTO FINISHED_TBL (PROGRAM, EXECTIME, MAXWORKERS, FINISHTIME, FINISHEDTASKS) VALUES ('%s', %.2f, %llu, '%s', %llu);", programName, execTime, maxWorkerNum, timeInText, finishedTasks);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at insert into finished table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sprintf(cmd, "DELETE FROM RUNNING_TBL WHERE PROGRAM = '%s';", programName);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error at delete from running table: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
#endif
	destroy_thread(master_thread);
	return 0;
}

void AccirrRun() {
	global_scheduler.run();
}

