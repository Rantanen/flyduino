
"strict on";

var common = require( '../common/socket' );
var interfaces = require( '../common/interfaces' );
var server = require( './server' );

var start = function( io ) {

    server.broadcast = common.createProxy( interfaces.Client, io.sockets );

    io.sockets.on( 'connection', function( socket ) {
        var proxy = common.bind( socket, interfaces.Server, server, interfaces.Client );

        server.connected( proxy );
    });
};

module.exports.start = start;
