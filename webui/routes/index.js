
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
	app.get('/compare', compare);
	app.get('/compare/pointer_chasing', pointer_chasing);
	app.get('/compare/BFS', BFS);
	app.get('/compare/Hash', Hash);
}

function translate(mStr) {
	var rst = mStr;
	var prefix = mStr.substr(0, mStr.indexOf('_'));
	var replaceStr;
	switch(prefix) {
	case 'pttest':
		replaceStr = '指针跟踪';
		break;
	case 'bfs':
		replaceStr = '宽度优先搜索';
		break;
	case 'wordcount':
		replaceStr = '基于Hash的查找';
		break;
	default:
		replaceStr = '测试程序';
		break;
	}
	rst = rst.replace(prefix, replaceStr);
	return rst;
};

index = function(req, res){
  res.render('index', { title: 'Accirr'});
};

running = function(req, res) {
	db.all("SELECT * FROM RUNNING_TBL ORDER BY STARTTIME ASC", function(err, rows) {
		req.listtype = 'running';
		if (rows.length === 0) {
			console.log("empty running result");
			res.render('list', { 
				tasklist: []
			});
		} else {
			for (i = 0; i < rows.length; i++) {
				rows[i].TRANSNAME = translate(rows[i].PROGRAM);
			}
			res.render('list', { 
				tasklist: rows
			});
		}
	});
};

tasktrack = function(req, res) {
	mTask = req.params.task;
	var cmd = "SELECT * FROM RUNNING_TBL WHERE PROGRAM = '" + mTask + "'";

	db.all(cmd, function(err, runnings) {
		if (runnings.length === 0) {
			req.listtype = 'finished';
		} else {
			req.listtype = 'running';
		}
		cmd = "SELECT * FROM TASKTRACK_TBL WHERE PROGRAM = '" + mTask + "' ORDER BY ATIME ASC";
		db.all(cmd, function(err, rows) {
			var mTitle = translate(mTask);
			if (rows.length === 0) {
				console.log("task " + mTask + " not found");
				res.render('tasktrack', {
					title: mTitle,
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
					title: mTitle,
					exectime: mExectime,
					workers: mWorkers,
					tasks: mTasks,
					switches: mSwitches
				});
			}
		});
	});
};

finished = function (req, res) {
	db.all("SELECT * FROM FINISHED_TBL ORDER BY FINISHTIME ASC", function(err, rows) {
		req.listtype = 'finished';
		if (rows.length === 0) {
			console.log("empty finished result");
			res.render('list', {
				tasklist: []
			});
		} else {
			for (i = 0; i < rows.length; i++) {
				rows[i].TRANSNAME = translate(rows[i].PROGRAM);
			}
			res.render('list', {
				tasklist: rows
			});
		}
	});
};
