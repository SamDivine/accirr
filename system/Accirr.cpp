#include "Accirr.h"


Worker *master_thread;

#ifdef WEBUI
double programStart;
double prevPoint;
double presentPoint;
uint64_t maxWorkerNum;
char programName[256];
sqlite3 *db;
char* errMsg;

char dbFile[] = "home/samdivine/programming/accirr/webui/accirr.db";
#endif

void AccirrInit(int *argc_p, char **argv_p[]) {
	master_thread = convert_to_master();
#ifdef WEBUI
	programStart = asecd();
	prevPoint = programStart;
	maxWorkerNum = 0;
	errMsg = NULL;
	sprintf(programName, "%s_%.2f", strrchr((*argv_p)[0], '/')+1, programStart);
	for (int i = 1; i < *argc_p; i++) {
		sprintf(programName, "%s_%s", programName, (*argv_p)[i]);
	}
	int rc;
	if ((rc = sqlite3_open(dbFile, &db)) != SQLITE_OK) {
		fprintf(stderr, "Can't open database %s: %s\n", dbFile, sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(-1);
	}
	if ((rc = sqlite3_exec(db, "CREATE TABLE RUNNING_TBL (PROGRAM TEXT PRIMARY KEY, STARTTIME REAL);", NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	char cmd[256];
	sprintf(cmd, "INSERT INTO RUNNING_TBL (PROGRAM, STARTTIME) VALUES (%s, %.2f);", programName, programStart);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	if ((rc = sqlite3_exec(db, "CREATE TABLE TASKTRACK_TBL (ATIME REAL PRIMARY KEY, PROGRAM TEXT, EXECTIME REAL, WORKERS INTEGER);", NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errMsg);
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
	presentPoint = asecd();
	double execTime = presentPoint-programStart;
	int rc;
	if ((rc = sqlite3_exec(db, "CREATE TABLE FINISHED_TBL (PROGRAM TEXT PRIMARY KEY, EXECTIME REAL, MAXWORKERS INTEGER, FINISHTIME REAL);", NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	char cmd[256];
	sprintf(cmd, "INSERT INTO FINISHED_TBL (PROGRAM, EXECTIME, MAXWORKERS, FINISHTIME) VALUES (%s, %.2f, %llu, %.2f);", programName, execTime, maxWorkerNum, presentPoint);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	sprintf(cmd, "DELETE * FROM RUNNING_TBL WHERE PROGRAM = %s;", programName);
	if ((rc = sqlite3_exec(db, cmd, NULL, NULL, &errMsg)) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
#endif
	destroy_thread(master_thread);
	return 0;
}

void AccirrRun() {
	global_scheduler.run();
}

