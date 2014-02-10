
"use strict";

/* global THREE: true */

var Quad = function() {
    this.object3d = new THREE.Object3D();

    var middle = new THREE.Mesh(
        new THREE.CubeGeometry( 0.14, 0.03, 0.14 ),
        new THREE.MeshLambertMaterial({ color: 0x778844 }) );
    this.object3d.add( middle );

    var xbeam1 = new THREE.Mesh(
        new THREE.CubeGeometry( 0.5, 0.01, 0.03 ),
        new THREE.MeshLambertMaterial({ color: 0xaaaaaa }) );
    var xbeam2 = new THREE.Mesh(
        new THREE.CubeGeometry( 0.5, 0.01, 0.03 ),
        new THREE.MeshLambertMaterial({ color: 0xaaaaaa }) );

    this.engines = [];
    this.enginePowers = [];

    for( var i = 0; i < 4; i++ ) {
        var xbeam = i % 2 ? xbeam1 : xbeam2;

        var engine = new THREE.Mesh(
            new THREE.CubeGeometry( 0.04, 0.05, 0.04 ),
            new THREE.MeshLambertMaterial({ color: 0xeeeeee }) );

        var enginePower = new THREE.Object3D();
        enginePower.position.y = 0.02;

        var engineIndicator = new THREE.Mesh(
            new THREE.CubeGeometry( 0.02, 0.10, 0.02 ),
            new THREE.MeshLambertMaterial({ color: 0x00ff00 }) );
        engineIndicator.position.y = 0.10/2;

        enginePower.add( engineIndicator );
        engine.add( enginePower );

        engine.position.x = i < 2 ? -0.25 : 0.25;
        engine.position.y = 0.015;
        xbeam.add( engine );

        this.engines[ i ] = engine;
        this.enginePowers[ i ] = enginePower;
    }

    xbeam1.rotation.y = Math.PI * (1/4);
    xbeam2.rotation.y = Math.PI * (-1/4);

    this.object3d.add( xbeam1 );
    this.object3d.add( xbeam2 );
};

Quad.prototype.setPosition = function( x, y, z ) {
    this.object3d.position = { x:x, y:y, z:z };
};

Quad.prototype.setOrientation = function( q ) {
    this.object3d.rotation.setFromQuaternion( q );
};

Quad.prototype.setEnginePowers = function( v ) {
    for( var i in v ) {
        this.enginePowers[ i ].scale.set( v[i]/1000, v[i]/1000, v[i]/1000 );
    }
};

module.exports = Quad;

