
"use strict";

var Client = require( './Client' );
var app = require( './MonitorApp' );

app.controller( 'MonitorCtrl', [ '$scope', 'socket', function( $scope, socket ) {
    $scope.loading = true;

    var server = socket.connect({
        initialize: function( server, state ) {
            console.dir( state );
            $scope.ports = state.ports;

            if( state.port ) {
                $scope.connectedPort = state.port;
                $scope.selectedPort = state.port;
            } else {
                $scope.selectedPort = state.ports[0];
            }

            $scope.loading = false;
            $scope.$apply();
        },

        setPorts: function( server, ports ) {
            $scope.ports = ports;
            $scope.$apply();
            console.dir( ports );
        },

        setPort: function( server, port ) {

            if( port ) { $scope.selectedPort = port; }

            $scope.connectedPort = port;
            $scope.$apply();
        },

        onData: function( server, data ) {
            console.dir( data );
        },

        onStatus: function( server, data ) {
            $scope.raw = JSON.stringify( data, null, "  " );
            $scope.orientation = data.orientation;
            $scope.enginePower = data.enginePower;
            $scope.$apply();
        },

        setReset: function( server, value ) {
            $scope.reset = value;
            $scope.$apply();
        },

        error: function( server, msg ) {
            console.warn( "ERROR: " + msg );
        }
    });

    $scope.selectedPort = null;
    $scope.connect = function() {
        server.connectSerial( $scope.selectedPort );
    };

    $scope.disconnect = function() {
        server.disconnectSerial();
    };

    $scope.stuff = "Foo";
    $scope.orientation = "Foo";

    return;
}]);



