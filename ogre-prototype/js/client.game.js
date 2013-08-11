$.noConflict();

jQuery(document).ready(function () {
    'use strict';
    var gameClient;
    
    jQuery("#enterBtn").click(function(ev) {
        var username = jQuery("#username").val();
        
        var request = jQuery.ajax({
            url: "login",
            type: "post",
            data: {
                "username" : username
            },
            success : function(data) {
                gameClient = (function () {
                    // Declare private variables and methods
                    var tick,
                        update,
                        animate,
                        handleInput,
                        that,
                        connection;

                    // define privileged functions
                    that = {
                        onConnect : function (data) {
                            console.log( "game_client.onConnect " );
                        },

                        onConnecting : function (data) {
                            console.log( "game_client.onConnecting " );
                        },

                        onConnected : function (data) {
                            console.log( "game_client.onConnected " );
                        },

                        onDisconnect : function (data) {
                            console.log( "game_client.onDisconnect " );
                        },

                        onPlayerJoin : function (data) {
                        
                        }, 

                        onPlayerLeave : function (data) {
                        
                        },

                        requestMoveUnit : function (data) {
                        
                        },

                        onError : function (data) {
                            console.log( "game_client.onError " );
                        }
                    };
                    
                     connection = client(that);
                    
                    return that;
                }());
                console.log(data);
                
                jQuery("div#frontPage").hide("slide", { direction: "left" }, "slow", function() {
                    jQuery("div#gamePage").show("slide", { direction: "right" }, "slow");
                });
            }
        });
        
        
        //console.log("logging in with username = " + username);
    });

    /*
    gameClient = function () {
        // Declare private variables and methods
        var tick,
            update,
            animate,
            handleInput,
            connection = client();

        // define privileged functions
        return {
            onConnect : function (data) {
                console.log( "game_client.onConnect " );
            },

            onConnecting : function (data) {
                console.log( "game_client.onConnecting " );
            },

            onConnected : function (data) {
                console.log( "game_client.onConnected " );
            },

            onDisconnect : function (data) {
                console.log( "game_client.onDisconnect " );
            },

            onPlayerJoin : function (data) {
            
            }, 

            onPlayerLeave : function (data) {
            
            },

            requestMoveUnit : function (data) {
            
            },

            onError : function (data) {
                console.log( "game_client.onError " );
            }
        };
    };
    */
});
