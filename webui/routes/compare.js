
module.exports = function(app) {
	app.get('/compare', compare);
	app.get('/compare/pointer_chasing', pointer_chasing);
	app.get('/compare/BFS', BFS);
	app.get('/compare/Hash', Hash);
}

compare = function(req, res) {
	res.render('compare', { title: 'Compare'});
};

pointer_chasing = function(req, res) {
	res.render('pointer_chasing', { title: 'pointer_chasing'});
};

BFS = function(req, res) {
	res.render('BFS', { title: 'Breadth First Search'});
};

Hash = function(req, res) {
	res.render('Hash', { title: 'Hash Algorithm(WordCount)'});
};

