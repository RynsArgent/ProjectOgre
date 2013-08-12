
var game_server = module.exports = (function () {
    // Declare private variables and methods
    var onPlayerConnect,
        onPlayerDisconnect,
        tick,
        update,
        players = {};
        
    // define privileged functions
    return {
        onPlayerConnect : function (client, callback) {
            console.log( "\t :: game_server.onPlayerConnect :: " + client.userid );
            console.log( "\t\t" + client.handshake.address.address + " : " + client.handshake.address.port );
            var address = client.handshake.address;
            
            if (players[client.userid] !== undefined) {
                players[client.userid] = client;
            }
            callback({userid : client.userid, userip : address.address});
        },

        onPlayerDisconnect : function (client, data) {
            console.log( "\t :: game_server.onPlayerDisconnect :: " + client.userid );
        },

        onRequestLogin : function (client, callback) {
            if (players[client.userid] !== undefined) {
                players[client.userid] = client;
            }
            callback({status : "success", msg : client.userid + " logged in as " + client.username + " from " + client.handshake.address.address});
        }
    };
}());
