
/*
 * GET pages.
 */

var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('/home/samdivine/programming/accirr/webui/accirr.db');


module.exports = function(app) {
	app.get('/', index);
	app.get('/running', running);
	app.get('/tt/:task', function(req, res){
	});
	app.get('/finished', finished);
}

index = function(req, res){
  res.render('index', { title: 'Accirr'});
};

running = function(req, res) {
  var tmp = [{"PROGRAM": "hahaha", "STARTTIME": 0},
			 {"PROGRAM": "hehe", "STARTTIME": 1}];
  res.render('running', { title: 'Running',
						  tasklist: tmp});
};

finished = function (req, res) {
  var tmp = [{"PROGRAM": "ha", "EXECTIME": 10, "MAXWORKERS": 3, "FINISHTIME": 40},
			 {"PROGRAM": "he", "EXECTIME": 11, "MAXWORKERS": 4, "FINISHTIME": 50}];
  res.render('finished', { title: 'Finished',
						   tasklist: tmp});
};
