var serverEngine;
serverEngine = module.exports = function () {

    var userManager,
        worldManager,
        mapManager,
        commands;

    userManager = function () {
        var user;
        
        user = function (spec) {
            var userID,
                userName, 
                userIP;
            userID = spec.uid || -1;
            userName = spec.uname || '';
            userIP = spec.uip || -1;
        };    
    };
    
    // to be used later
    worldManager = function () {
    
    };
    
    // to be used later
    mapManager = function () {
    
    };
    
    commands = {
        0 : '',
        1 : 'connected',
        2 : 'join',
        3 : 'disconnected',
        4 : 'leave'
        
    };
    
    return {
        processCommand : function () {
        
        },
        
        sendGameState : function () {
        
        },
    };
};

/**
  * requestAnim shim layer by Paul Irish
  * Finds the first API that works to optimize the animation loop,
  * otherwise defaults to setTimeout().
  */

window.requestAnimFrame = (function(){
    return window.requestAnimationFrame     ||
        window.webkitRequestAnimationFrame  ||
        window.mozRequestAnimationFrame     ||
        window.oRequestAnimationFrame       ||
        window.msRequestAnimationFrame      ||
        function(/* function */ callback, /* DOMElement */ element){
	        window.setTimeout(callback, 1000 / 60);
        };
}());

