
"use strict";

/* global THREE: true, requestAnimationFrame: true */

var app = require( './MonitorApp' );
var Quad = require( './Quad' );
console.log( "Quad" );
console.dir( Quad );

var Scene = function( scope, element, attrs ) {
    this.scope = scope;
    this.element = element[0];
    this.attrs = attrs;
    this.aspectRatio = this.element.offsetWidth / this.element.offsetHeight;
};

Scene.prototype.init = function() {

    this.scene = new THREE.Scene();

    this.renderer = new THREE.WebGLRenderer();
    this.renderer.setSize( this.element.offsetWidth, this.element.offsetHeight);
    console.dir( this.element );
    this.element.appendChild( this.renderer.domElement );
    // var $elem = $( this.renderer.domElement );
    // $(this.element).append( this.renderer.domElement );

    this.renderer.domElement.style.position = "absolute";
    this.renderer.domElement.style.top = 0;
    this.renderer.domElement.style.left = 0;
    /*
    $elem.css( {
        position: "absolute",
        top: 0, bottom: 0, left: 0, right: 0,
        "z-index": -1
    } );
    */


    this.quad = new Quad();
    this.quad.setPosition( 0, 1.7, 0 );
    this.scene.add( this.quad.object3d );

    // Set up the camera on average human height
    this.camera = new THREE.PerspectiveCamera( 70, this.aspectRatio, 0.1, 1000 );
    this.camera.position = { x: 0, y: 1.70, z: 0.50 };

    this.scene.add( this.camera );

    var mat = new THREE.MeshLambertMaterial({ color: 0x00ff00 });
    this.ground = new THREE.Mesh( new THREE.PlaneGeometry( 100, 100 ), mat );
    this.ground.position.y = 0;
    this.ground.rotation.x = -Math.PI / 2;
    this.scene.add( this.ground );

    this.sun = new THREE.HemisphereLight( 0xffffff, 0xffffff, 0.5 );
    this.sun.color.setHSL( 0.6, 1, 0.6 );
    this.sun.groundColor.setHSL( 0.095, 1, 0.75 );
    //this.ambient = new THREE.AmbientLight( 0x808080 );
    //this.sun.position.set( 0, 1, 0 );
    this.scene.add( this.sun );
    //this.scene.add( this.ambient );
};

Scene.prototype.animate = function() {
    var self = this;

    // On call to animate set animate to true.
    this.animation = true;
    
    // Set up the animate frame callback without the animte=true.
    var doFrame = function() {

        // Queue an animation frame request with the browser.
        requestAnimationFrame(function() {
            if( !self.animation ) { return false; }

            // If the animation is still going on, render the thing and request new frame.
            self.render();
            doFrame();
        });
    };

    doFrame();
};

Scene.prototype.render = function() {
    this.renderer.render( this.scene, this.camera );
};

app.directive( 'scene', function() {
    return {
        restrict: 'E',
        link: function( scope, element, attrs ) {
            var scene = new Scene( scope, element, attrs );
            scene.init();
            scene.animate();

            scope.$watch( 'orientation', function( value ) {
                var quat = {
                    x: value.x,
                    y: value.z,
                    z: -value.y,
                    w: value.w
                };

                scene.quad.setOrientation( quat );
                    //new THREE.Quaternion( value.x, value.y, value.z, value.w ) );
            });

            scope.$watch( 'enginePower', function( value ) {
                scene.quad.setEnginePowers( value );
            });
        }
    };
});
