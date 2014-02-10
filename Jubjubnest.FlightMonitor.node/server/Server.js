
"strict on";

var connection = require( './Connection' );

var Server = function() {
    var self = this;

    connection.on( 'portschanged', function( ports ) {
        self.broadcast.setPorts( ports );
    });
};

Server.prototype.connected = function( client ) {
    console.dir( connection );

    client.initialize({
        ports: connection.ports,
        port: this.serial && this.serial.port
    });
    client.setPorts( connection.ports );
};

Server.prototype.disconnected = function( client ) {
};

Server.prototype.connectSerial = function( client, port ) {
    var self = this;

    if( !port || !port.comName ) {
        throw "Invalid port";
    }

    // Disconnect the current port.
    if( this.serial ) {
        self.broadcast.setPort( null );
        this.serial.close();
        this.serial = null;
    }

    connection.construct( port, function( err, serial ) {
        if( err ) return client.error( err );

        self.serial = serial;
        serial.resetUsers = {};

        self.broadcast.setPort( port, self.serialState );
        serial.on( 'message', function( msg ) {
            self.broadcast.onData( msg );
        });
        serial.on( 'status', function( msg ) {
            self.broadcast.onStatus( msg );
        });
    });
};

Server.prototype.disconnectSerial = function( client ) {
    var self = this;

    // Disconnect the current port.
    if( this.serial ) {
        this.serial.close();
        this.serial = null;
        self.broadcast.setPort( null );
    }
};

Server.prototype.setReset = function( client, value ) {
    if( !this.serial )
        client.error( "No serial connection" );

    // Check whether this user is requesting or canceling reset
    if( value )
    {
        // Mark the user as resetting user
        this.serial.resetUsers[ client.id ] = true;

        // If the port is not in reset state, mark it for reset.
        if( !this.serial.dtr )
        {
            this.serial.setDtr( true );
            this.broadcast.setReset( true );
        }
    }
    else
    {
        delete this.serial.resetUsers[ client.id ];

        if( Object.keys( this.serial.resetUsers ).length == 0 )
        {
            this.serial.setDtr( false );
            this.broadcast.setReset( false );
        }
    }

};

Server.prototype.echo = function( client, msg, cb ) {
    cb( "> " + msg );
};

module.exports = new Server();

