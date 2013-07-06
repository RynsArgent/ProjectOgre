var
	game_server = module.exports = { games : {}, game_count: 0 },
	verbose 		= true;
	
game_engine = require('./game-engine.js');
tmp_data = require('../resources/tmp.map.2.js');

global.window = global.document = global;

game_server.log = function() {
	if(verbose) console.log.apply(this, arguments);
};

game_server.createGame = function()
{
	var game = {
		id : 'game-' + Math.floor(Math.random() * 100),
		players : {},
		player_count : 0
	};
	
	this.games[ game.id ] = game;
	this.game_count += 1;
	
	game.engine = new game_engine(game.id);
	game.engine.server_init({tileWidth:32,tileHeight:32,input:tmp_data});
	
	this.log( game.id + ' has initialized');
}

// TODO: this doesn't correctly work - need to debug 
// ignore for now since we'll not use it yet
game_server.findGame = function(client)
{
	for( var i in this.games )
	{
		if( this.games[i].players.hasOwnProperty(client.userid) )
		{
			return this.games[i];
		}
	}
}

game_server.onPlayerConnected = function(client)
{
	//var game = this.findGame(client);
	//if( !game )
	//{
	var keys = Object.keys(this.games);
	var game = this.games[ keys[0] ];
		game.players[client.userid] = client;
	//}
	
	client.emit('onconnected', { id: client.userid, gid: game.id, castles: { data: game.Castles, imgsrc: "images/CastleRed.32.png" } });
	client.broadcast.emit('playerjoined', { id: client.userid});
}


/* start a game */
//game_server.createGame();
