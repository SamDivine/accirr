
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
		replaceStr = '基于Hash的文本计数';
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
		if (err) {
			console.log(err);
			return res.redirect('/running');
		}
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
	db.serialize(function() {
		var cmd = "SELECT * FROM RUNNING_TBL WHERE PROGRAM = '" + mTask + "'";
		db.all(cmd, function(err, rows) {
			if (err) {
				console.log(err);
				return res.redirect('/tt/'+mTask);
			}
			if (rows.length !== 0) {
				req.graphtype = 'running';
			} else {
				req.graphtype = 'notrunning';
			}
		});
		cmd = "SELECT * FROM TASKTRACK_TBL WHERE PROGRAM = '" + mTask + "' ORDER BY ATIME ASC";
		db.all(cmd, function(err, rows) {
			if (err) {
				req.flash('error', err);
				return res.redirect('/');
			}
			var mTitle = translate(mTask);
			if (rows.length === 0) {
				req.flash('error', '没有该任务');
				return res.redirect('/');
			} else {	
				mExectime = new Array();
				mWorkers = new Array();
				mTasks = new Array();
				mSwitches = new Array();
				var mMaxWorkers = 0;
				for (i = 0; i < rows.length; i++) {
					mExectime[i] = rows[i].EXECTIME;
					mWorkers[i] = rows[i].WORKERS;
					mTasks[i] = rows[i].FINISHEDTASKS;
					mSwitches[i] = rows[i].CONTEXTSWITCHES;
					if (mWorkers[i] > mMaxWorkers) {
						mMaxWorkers = mWorkers[i];
					}
				}
				res.render('tasktrack', {
					title: mTitle,
					exectime: mExectime,
					workers: mWorkers,
					tasks: mTasks,
					switches: mSwitches,
					maxWorkers: mMaxWorkers
				});
			}
		});
	});
};

finished = function (req, res) {
	db.all("SELECT * FROM FINISHED_TBL ORDER BY FINISHTIME ASC", function(err, rows) {
		if (err) {
			cosole.log(err);
			return res.redirect('/finished');
		}
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
