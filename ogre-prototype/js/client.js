var client = function () {
    var socket = io.connect();

    // When we connect, we are not 'connected' until we have a server id
    // and are placed in a game by the server. The server sends us a message for that.
	this.socket.on('connect', function() {
		// TODO: handle this case - probably should set a state to CONNECTING
	});

	// Handle when we connect to the server, showing state and storing id's.
    this.socket.on('onconnected', this.onConnected.bind(this));

	// Sent when we are disconnected (network, server down, etc)
	this.socket.on('ondisconnected', this.onDisconnect.bind(this));

	// Sent each tick of the server simulation. This is our authoritive update
    this.socket.on('onserverupdate', function() {

    });

    // On error we just show that we are not connected for now. Can print the data.
    this.socket.on('error', this.onDisconnect.bind(this));

    // On message from the server, we parse the commands and send it to the handlers
    this.socket.on('message', function() {

    });

    this.socket.on('playerjoined', this.onPlayerJoin.bind(this));

    this.socket.on('playerleave', this.onPlayerLeave.bind(this));
};
