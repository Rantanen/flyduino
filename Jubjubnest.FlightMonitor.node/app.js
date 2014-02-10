
/**
 * Module dependencies.
 */

var monitorServer = require( './server/server' );

var express = require('express');
var http = require('http');
var path = require('path');
var browserify = require('browserify-middleware');

var app = express();

// all environments
app.set('port', process.env.PORT || 3000);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'hjs');
app.use(express.favicon());
app.use(express.logger('dev'));
app.use(express.json());
app.use(express.urlencoded());
app.use(express.methodOverride());
app.use(app.router);
app.use(require('stylus').middleware(path.join(__dirname, 'public')));
app.use('/', browserify('./client'));
app.use(express.static(path.join(__dirname, 'public')));

// development only
if ('development' == app.get('env')) {
  app.use(express.errorHandler());
}

// Set up socket IO
var server = http.createServer( app );
var io = require('socket.io').listen( server, { log: false } );
var socket = require( './server/socket' ).start( io );

server.listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});
