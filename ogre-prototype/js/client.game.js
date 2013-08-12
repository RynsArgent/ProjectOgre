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
            
            bgCanvas = jQuery("#bg");
            stCanvas = jQuery("#st");
            mainCanvas = jQuery("#main");
            fgCanvas = jQuery("#fg");
            
            jQuery("#viewport_container").width = document.body.clientWidth;
            jQuery("#viewport_container").height = document.body.clientHeight;
            
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
