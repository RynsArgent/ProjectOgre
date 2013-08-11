
var client = function (game) {
    var socket = io.connect();

    // When we connect, we are not 'connected' until we have a server id
    // and are placed in a game by the server. The server sends us a message for that.
    socket.on('connect', function() {
	    // TODO: handle this case - probably should set a state to CONNECTING
	    game.onConnecting();
    });

    // Handle when we connect to the server, showing state and storing id's.
    socket.on('onconnected', game.onConnected());

    // Sent when we are disconnected (network, server down, etc)
    //socket.on('ondisconnected', game_client.onDisconnect());

    // Sent each tick of the server simulation. This is our authoritive update
    socket.on('onserverupdate', function() {

    });

    // On error we just show that we are not connected for now. Can print the data.
    //socket.on('error', game_client.onError());

    // On message from the server, we parse the commands and send it to the handlers
    socket.on('message', function() {

    });

    socket.on('playerjoined', game.onPlayerJoin());

    socket.on('playerleave', game.onPlayerLeave());
}
