
var game_server = module.exports = (function () {
    // Declare private variables and methods
    var onPlayerConnect,
        onPlayerDisconnect,
        tick,
        update,
        players = [];

    // define privileged functions
    return {
        onPlayerConnect : function (client, data) {
            console.log( " game_server.onPlayerConnect :: " + client.userid );
        },

        onPlayerDisconnect : function (client, data) {
            console.log( "game_server.onPlayerDisconnect :: " + client.userid );
        },

        onRequestLogin : function (username, callback) {
            var i, player;
            
            // Make sure the username isn't already in use
            for(i = 0; i < players.length; i += 1) {
                if(players[i].username === username) {
                    callback({ result : "error", msg : "This name is already in use" });
                    return;
                }
            }
            
            player = (function (username) {
                var client;
                
                return {
                    username : function () {
                        return username;
                    },

                    setUsername : function (name) {
                        username = name;
                    },
                    
                    client : function () {
                        return client;
                    },
                    
                    setClient : function (cli) {
                        client = cli;
                    }
                };
            }());
            
            players.push(player);
            callback({ result : "success", msg : "Connected as " + username });
        }
    };
}());
