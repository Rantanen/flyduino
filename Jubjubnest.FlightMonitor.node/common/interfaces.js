
exports.Server = {

    connectSerial: function( port ) {},
    disconnectSerial: function() {},
    setReset: function( value ) {}

};

exports.Client = {

    initialize: function( state ) {},
    setPorts: function( ports ) {},
    setPort: function( port ) {},
    setReset: function( value ) {},
    onData: function( data ) {},
    onStatus: function( data ) {},

    error: function( msg ) {}
};

