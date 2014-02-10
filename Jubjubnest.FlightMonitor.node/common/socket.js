
/**
 * Binds a
 *
 * Handles message registration and proxy generation
 *
 * @param socket The socket to bind
 * @param localInterface The local interface which is available through the socket
 * @param handler The handler implementing the local interface
 * @param remoteInterface The remote proxy interface for the socket
 *
 * @returns The proxy stub
 */
var bind = function( socket, localInterface, handler, remoteInterface ) {

    var proxy = createProxy( remoteInterface, socket );
    setUpHandler( socket, localInterface, handler, proxy );

    return proxy;
};

var createProxy = function( remoteInterface, socket ) {

    var proxy = { _socket: socket, id: socket.id };

    Object.keys( remoteInterface ).forEach( function( method ) {
        if( typeof( remoteInterface[ method ] ) !== 'function' ) return;

        proxy[ method ] = function() {

            console.log( 'Calling: ' + method );

            var args = Array.prototype.slice.call(arguments);
            args.unshift( method );
            socket.emit.apply( socket, args );
        };
    });

    return proxy;
};

/**
 * Sets up the handler for the socket
 *
 * @param socket Socket to monitor for new messages
 * @param localInterface Local interface for the methods
 * @param handler The handler which implements the local interface
 * @param proxy The proxy class for the remote connection
 */
var setUpHandler = function( socket, localInterface, handler, proxy ) {
    if( !proxy._socket ) {
        proxy = createProxy( proxy, socket );
    }

    Object.keys( localInterface ).forEach( function( method ) {
        if( typeof( localInterface[ method ] ) !== 'function' ) return;

        // Make sure the handler function is valid
        var interfaceFunction = localInterface[ method ];
        var handlerFunction = handler[ method ];

        if( !handlerFunction ) throw "Handler does not implement " + method;
        if( handlerFunction.length !== interfaceFunction.length + 1 )
            throw method + " has wrong amount of arguments. Expected Proxy + " + interfaceFunction.length;

        socket.on( method, function() {
            try {

                var args = Array.prototype.slice.call(arguments);
                args.unshift( proxy );
                handler[ method ].apply( handler, args );

            } catch( e ) {
                socket.emit( 'error', e );
            }
        });
    });
};

exports.bind = bind;
exports.createProxy = createProxy;

