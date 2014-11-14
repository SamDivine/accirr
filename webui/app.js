
/**
 * Module dependencies.
 */

var express = require('express')
  , routes = require('./routes')
  , compare = require('./routes/compare');

var app = module.exports = express.createServer();

app.dynamicHelpers({
  listtype: function(req, res) {
    return req.listtype;
  },
});

// Configuration

app.configure(function(){
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  //app.use(app.router);
  app.use(express.router(routes));
  app.use(express.router(compare));
  app.use(express.static(__dirname + '/public'));
});

app.configure('development', function(){
  app.use(express.errorHandler({ dumpExceptions: true, showStack: true }));
});

app.configure('production', function(){
  app.use(express.errorHandler());
});

app.listen(9527, function(){
  console.log("Express server listening on port %d in %s mode", app.address().port, app.settings.env);
});
