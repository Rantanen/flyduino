
"strict on";

var Client = function( $scope ) {
    this.$scope = $scope;
};

Client.prototype.initialize = function( server, state ) {
    this.$scope.ports = state.ports;
    this.$scope.currentPort = state.port;
    this.$scope.$apply();
};

Client.prototype.setPorts = function( server, ports ) {
    this.$scope.ports = ports;
    this.$scope.$apply();
    console.dir( ports );
};

Client.prototype.setPort = function( server, port ) {
    this.$scope.currentPort = port;
    this.$scope.$apply();
};

Client.prototype.onData = function( server, data ) {
    console.log( data );
};

module.exports = Client;

