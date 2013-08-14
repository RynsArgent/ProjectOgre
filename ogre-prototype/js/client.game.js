$.noConflict();

jQuery(document).ready(function () {
    'use strict';
    var gameClient;
    
    jQuery("#enterBtn").click(function(ev) {
        var username = jQuery("#username").val();
        gameClient.requestLogin(username);
    });

    gameClient = (function () {
        // Declare private variables and methods
        var that,
            player,
            
            tick,
            update,
            animate,
            handleInput,
            
            socket = io.connect(),
            
            bgCanvas,
            stCanvas,
            mainCanvas,
            fgCanvas;
            
            bgCanvas = jQuery("#bg").get(0);
            stCanvas = jQuery("#st").get(0);
            mainCanvas = jQuery("#main").get(0);
            fgCanvas = jQuery("#fg").get(0);
            
            bgCanvas.width = document.body.clientWidth;
            bgCanvas.height = document.body.clientHeight;
            
            console.log( "bgCanvas width: " + bgCanvas.width );
            
            stCanvas.width = document.body.clientWidth;
            stCanvas.height = document.body.clientHeight;
            
            mainCanvas.width = document.body.clientWidth;
            mainCanvas.height = document.body.clientHeight;
            
            fgCanvas.width = document.body.clientWidth;
            fgCanvas.height = document.body.clientHeight;

        // connection communications //
        // Handle when we connect to the server, showing state and storing id's.
        socket.on('onconnected', function (data) {
            console.log('gameClient : onconnected');
            console.log(data);
            
            player = {
                userid : data.userid,
                userip : data.userip
            };
        });
        
        socket.on('onloggedin', function (data) {
            console.log('gameClient : onloggedin');
            console.log(data);
            
            if (data.status === 'success') {
                player.username = data.username;
                jQuery("div#frontPage").hide("slide", { direction: "left" }, "slow", function() {
                    jQuery("div#gamePage").show("slide", { direction: "right" }, "slow");
                });
            }
        });
        ///////////////////////////////

        // define privileged functions
        return {
            requestLogin : function (username) {
                socket.emit('requestlogin', username);
            }
        };
        
    }());
});

////////////////////////////////////////////////////////////////////////////////////////////
// http://paulirish.com/2011/requestanimationframe-for-smart-animating/
// http://my.opera.com/emoller/blog/2011/12/20/requestanimationframe-for-smart-er-animating
 
// requestAnimationFrame polyfill by Erik Möller
// fixes from Paul Irish and Tino Zijdel
 
(function() {
    var lastTime = 0;
    var vendors = ['ms', 'moz', 'webkit', 'o'];
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
        window.cancelAnimationFrame = window[vendors[x]+'CancelAnimationFrame']
                                   || window[vendors[x]+'CancelRequestAnimationFrame'];
    }
 
    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function() { callback(currTime + timeToCall); },
              timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };
 
    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        };
}());
////////////////////////////////////////////////////////////////////////////////////////////
