var
	game_server = module.exports = { games : {}, game_count: 0 },
	UUID				= require('node-uuid');
	verbose			=	true;
	
// Since we are sharing code with the browser
// include some values to handle that
global.window = global.document = global;

// Import shared game library code
require('./game.core.js');

// Simple wrapper for logging
game_server.log = function() {
	if(verbose) console.log.apply(this, arguments);
};

game_server.fake_latency = 0;
game_server.local_time = 0;
game_server._dt = new Date().getTime();
game_server._dte = new Date().getTime();

// a local queue of messages we delay if faking latency
game_server.messages = [];

setInterval( function() {
	game_server._dt = new Date().getTime() - game_server._dte;
	game_server._dte = new Date().getTime();
	game_server.local_time += game_server._dt/1000.0;
}, 4);

game_server.onMessage = function(client, message) {
	// leave out fake latency - for now
	game_server._onMessage(client, message);
};

game_server._onMessage = function(client, message) {
	
};	// game_server.onMessage

game_server.onInput = function(client, parts) {
	
};	// game_server.onInput

game_server.createGame = function(player) {

};	// game_server.createGame

game_server.endGame = function(gameid, userid) {

};	// game_server.endGame

game_server.startGame = function(game) {

};	// game_server.startGame

game_server.findGame = function(player) {

};	// game_server.findGame
