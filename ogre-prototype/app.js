var
	gameport	= process.env.PORT || 4004,
	
	io				=	require('socket.io'),
	express		=	require('express'),
	
	verbose		=	true,
	app				=	express.createServer();
	
/* Express Server Set Up */

// The express server handles passing our content to the browser,
// as well as routing users to their intended destination.

// NOTE: this is bare bones and will serve any file the user requests from the
// root of the web server.

// Tell the server to listen for incoming connections
app.listen(gameport);

// Log something so we know that it succeeded.
console.log('\t :: Express :: Listening on port ' + gameport);

// By default, we forward the / path to index.html automatically
app.get('/', function( req, res ) {
	res.sendfile('index.html', { root:__dirname });
});

// This handler will listen for requests on /*, any file from the root of our server.
// See expressjs documentation for more info on routing
app.get('/*', function( req, res, next ) {
	// This is the current file they have requested
	var file = req.params[0];
	
	// For debugging, we can track what files are requested.
	if(verbose) console.log('\t :: Express :: file requested : ' + file);
	
	// Send the requesting client the file.
	res.sendfile(file, { root:__dirname });
});

/* Socket.IO server set up */

// Express and socket.io can work together to serve the socket.io client files
// This way, when the client requests '/socket.io/' files, socket.io determines what they need

// Create a socket.io instance using express server
var sio = io.listen(app);

// Configure the socket.io connection settings
// See http://socket.io/
sio.configure(function() {
	sio.set('log level', 0);
	sio.set('authorization', function(handshakeData, callback) {
		callback(null, true);	// error first callback style
	});
});

/* Temporary game server code */
game_server = require('./js/server-engine.js');
console.log( 'Game server initialized: ' + game_server.game_count + ' game running' );
/* -------------------------- */

sio.sockets.on('connection', function(client) {
	// assign a userid to new client
	client.userid = 'Player-' + Math.floor(Math.random() * 100);

	// Tell the player they connected
	client.emit('onconnected', { id: client.userid });
	client.broadcast.emit('playerjoined', { id: client.userid});
	// call game_server
	//game_server.onPlayerConnected( client );
	console.log('\t :: socket.io :: player ' + client.userid + ' connected');

	
	
	// Handle some messages that clients send
	client.on('message', function(msg) {
		console.log('\t :: socket.io :: player ' + client.userid + ' sent message [' + msg + ']');
	});
	
	client.on('mouseclick', function(data) {
		console.log('\t :: socket.io :: player ' + client.userid + ' clicked ' + data.x + ',' + data.y);
		client.broadcast.emit('onothersclick', {pid: client.userid, x:data.x, y:data.y});
	});
	
	// handle disconnect
	client.on('disconnect', function() {
		console.log('\t :: socket.io :: player ' + client.userid + ' disconnected');
	});
});
