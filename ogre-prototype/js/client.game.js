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
        var tick,
            update,
            animate,
            handleInput,
            that,
            socket = io.connect(),
            player;

        // connection communications //
        // Handle when we connect to the server, showing state and storing id's.
        socket.on('onconnected', function (data) {
            console.log('gameClient : onconnected : ' + data.userid + ', ' + data.userip);
            
            player = {
                userid : data.userid,
                userip : data.userip
            };
        });
        
        socket.on('onloggedin', function (data) {
            console.log(data);
            
            if (data.status === 'success') {
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
