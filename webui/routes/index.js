
/*
 * GET pages.
 */

var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('/home/localhost/accirr/webui/accirr.db');


module.exports = function(app) {
	app.get('/', index);
	app.get('/running', running);
	app.get('/tt/:task', tasktrack);
	app.get('/finished', finished);
}

index = function(req, res){
  res.render('index', { title: 'Accirr'});
};

running = function(req, res) {
	db.all("SELECT * FROM RUNNING_TBL ORDER BY STARTTIME ASC", function(err, rows) {
		if (!rows) {
			console.log("empty running result");
			res.render('running', { 
				title: 'Running',
				tasklist: []
			});
		} else {
			res.render('running', { 
				title: 'Running',
				tasklist: rows
			});
		}
	});
};

tasktrack = function(req, res) {
	mTask = req.params.task;
	var cmd = "SELECT * FROM TASKTRACK_TBL WHERE PROGRAM = '" + mTask + "' ORDER BY ATIME ASC";
	db.all(cmd, function(err, rows) {
		if (!rows) {
			console.log("task " + mTask + " not found");
			res.render('tasktrack', {
				title: mTask,
				exectime: [],
				workers: [],
				tasks: [],
				switches: []
			});
		} else {
			mExectime = new Array();
			mWorkers = new Array();
			mTasks = new Array();
			mSwitches = new Array();
			for (i = 0; i < rows.length; i++) {
				mExectime[i] = rows[i].EXECTIME;
				mWorkers[i] = rows[i].WORKERS;
				mTasks[i] = rows[i].FINISHEDTASKS;
				mSwitches[i] = rows[i].CONTEXTSWITCHES;
			}
			res.render('tasktrack', {
				title: mTask,
				exectime: mExectime,
				workers: mWorkers,
				tasks: mTasks,
				switches: mSwitches
			});
		}
	});
};

finished = function (req, res) {
	db.all("SELECt * FROM FINISHED_TBL ORDER BY FINISHTIME ASC", function(err, rows) {
		if (!rows) {
			console.log("empty finished result");
			res.render('finished', {
				title: 'Finished',
				tasklist: []
			});
		} else {
			res.render('finished', {
				title: 'Finished',
				tasklist: rows
			});
		}
	});
};
