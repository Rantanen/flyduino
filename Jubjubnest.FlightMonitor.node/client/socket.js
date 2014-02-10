
"strict on";

var app = require( './MonitorApp' );
var common = require( '../common/socket' );
var interfaces = require( '../common/interfaces' );

app.factory('socket', function( $rootScope ) {

    return {
        connect: function( client ) {

            var socket = io.connect();
            var proxy = common.bind( socket, interfaces.Client, client, interfaces.Server );
            return proxy;
        }
    }
});
