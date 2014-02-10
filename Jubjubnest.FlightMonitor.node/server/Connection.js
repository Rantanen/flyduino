
var EventEmitter = require( 'events' ).EventEmitter;
var util = require( 'util' );

var serialport = require( 'serialport' );
var SerialPort = serialport.SerialPort;

var native = require( '../flightcontrol-native' );

var events = module.exports = new EventEmitter();
module.exports.ports = [];

/**
 * Adds constructor methods to individual ports in the port array.
 *
 * @param COM port array as returned by serialport.list
 *
 * @returns Same array
 */
var addConstructors = function( portsArray ) {
    portsArray.forEach( function( port ) {

        port.construct = function( cb ) {
            construct( port.comName, cb );
        };
    });

    return portsArray;
};

/**
 * Checks whether ports have been updated
 */
var updateAvailablePorts = function() {
    serialport.list( function( err, newPorts ) {

        // If the amount of ports has changed, signal update
        if( module.exports.ports.length !== newPorts.length ) {
            module.exports.ports = addConstructors( newPorts );
            events.emit( 'portschanged', module.exports.ports );
            return;
        }

        // Amount of ports was equal. Make sure the individual ports
        // have stayed same
        var i = 0;
        newPorts.forEach( function( newPort ) {

            var oldPort = module.exports.ports[i];

            if( oldPort.comName !== newPort.comName ||
                oldPort.pnpId !== newPort.pnpId ||
                oldPort.manufacturer !== newPort.manufacturer )
            {
                module.exports.ports = addConstructors( newPorts );
                events.emit( 'portschanged', module.exports.ports );
                return;
            }

            i++;
        });
    });
};

/**
 * Starts the serial port update checks
 */
var scheduleUpdate = function() {
    setTimeout( function() {
        updateAvailablePorts();
        scheduleUpdate();
    }, 500 );
};

scheduleUpdate();


var construct = function( port, done ) {

    try {

        // If SerialPort auto-opens the connection it'll
        // throw uncatchable exception
        var serial = new SerialPort( port.comName, {
            baudrate: 115200
        }, false);

        // Manually open it so the exception can be caught here.
        serial.open( function() {
            done( null, new Connection( serial, port ) );
        });

    } catch( e ) { done( e.toString() ); }
};

module.exports.construct = construct;


/**
 * Serial connection
 */
var Connection = function( serial, port ) {
    var self = this;
    this.serial = serial;
    this.port = port;
    this.dtr = false;

    this.buffers = [];
    this.started = false;
    this.bufferLength = 0;
    serial.on( 'data', function( data ) { self.receive( data ); } );
};
util.inherits( Connection, EventEmitter );

Connection.prototype.receive = function( data ) {

    var i;

    // If we haven't found start yet, search for it.
    if( !this.started ) {

        for( i = 0; i < data.length; i++ ) {
            if( data[i] === 0 ) {
                // Remove the noise (and the start zero) from the packet.
                data = data.slice( i + 1 );
                this.started = true;
                break;
            }
        }
    }

    // If we are still waiting for start, ignore this packet.
    if( !this.started ) {
        return;
    }

    // Start found. We need to wait for the end.
    var endLength = -1;
    for( i = 1; i < data.length; i++ ) {

        // check for the end zero
        if( data[i] === 0 ) {
            endLength = i;
            break;
        }
    }

    // If we had no end, store the packet and return here.
    if( endLength < 0 ) {
        this.buffers.push( data );
        this.bufferLength += data.length;
        return;
    }

    // We had end. Construct the full buffer.
    var b = new Buffer( this.bufferLength + endLength );

    var written = 0;
    for( i = 0; i < this.buffers.length; i++ ) {
        this.buffers[ i ].copy( b, written );
        written += this.buffers[ i ].length;
    }

    // Finally add the remaining packet.
    data.copy( b, written, 0, endLength );

    // Clear the queue
    this.started = false;
    this.buffers = [];
    this.bufferLength = 0;

    // Calculate the checksum ignoring the last byte
    var checksum = 0;
    for( i = 0; i < b.length - 1; i++ )
        checksum ^= b[ i ];

    if( checksum !== b[ b.length - 1 ] )
    {
        console.log( "Checksum mismatch. Expected: " + b[ b.length - 1 ] + ", was: " + checksum );

        // We were at zero byte.
        // Remove everything before the zero and recurse.
        // The current zero might be a real start byte.
        data = data.slice( endLength );
        this.receive( data );
        return;
    }

    // Remove the checksum from the buffer.
    b = b.slice( 0, b.length - 1 );
    
    // Decode the status and emit it
    var status = native.decodeStatus( b );
    this.emit( 'status', status );

    // Recurse with the remaining data.
    data = data.slice( endLength + 1 );
    if( data.length > 0 )
        this.receive( data );
};

Connection.prototype.write = function( data, cb ) {
    this.serial.write( data, cb );
};

Connection.prototype.close = function( cb ) {
    this.removeAllListeners();
    this.serial.close( cb );
};

Connection.prototype.setDtr = function( cb ) {
};

module.exports.Connection = Connection;

