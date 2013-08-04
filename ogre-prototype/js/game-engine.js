var Terrain = {
	'p' : {label: 'Plains', image: function(){ var img = new Image(); img.src = 'images/Plains.png'; return img;}},
	'w' : {label: 'Sea', image: function(){ var img = new Image(); img.src = 'images/Seas.png'; return img;}},
	'm' : {label: 'Mountains', image: function(){ var img = new Image(); img.src = 'images/Mountains.png'; return img;}},
	'f' : {label: 'Forests', image: function(){ var img = new Image(); img.src = 'images/Forests.png'; return img;}},
	's' : {label: 'Swamps', image: function(){ var img = new Image(); img.src = 'images/Swamps.png'; return img;}},
	
	'df' : {label: 'dforest', image: function(){ var img = new Image(); img.src = 'images/ForestDeserts.png'; return img;}},
	'dm' : {label: 'dmountain', image: function(){ var img = new Image(); img.src = 'images/MountainDeserts.png'; return img;}},
	'dn' : {label: 'desert', image: function(){ var img = new Image(); img.src = 'images/Deserts.png'; return img;}},
	'pf' : {label: 'pforest', image: function(){ var img = new Image(); img.src = 'images/ForestPlains.png'; return img;}},
	'pm' : {label: 'pmountain', image: function(){ var img = new Image(); img.src = 'images/MountainPlains.png'; return img;}},
	'pn' : {label: 'plains', image: function(){ var img = new Image(); img.src = 'images/Plains.png'; return img;}},
	'sf' : {label: 'sforest', image: function(){ var img = new Image(); img.src = 'images/ForestSnows.png'; return img;}},
	'sm' : {label: 'smountain', image: function(){ var img = new Image(); img.src = 'images/MountainSnows.png'; return img;}},
	'sn' : {label: 'snow', image: function(){ var img = new Image(); img.src = 'images/Snows.png'; return img;}},
	'wb' : {label: 'beach', image: function(){ var img = new Image(); img.src = 'images/Beaches.png'; return img;}},
	'wo' : {label: 'ocean', image: function(){ var img = new Image(); img.src = 'images/Oceans.png'; return img;}},
	'ws' : {label: 'sea', image: function(){ var img = new Image(); img.src = 'images/Seas.png'; return img;}},
};

var TerrainSrv = {
	'p' : {label: 'Plains', image: 'images/Plains.png'},
	'w' : {label: 'Sea', image: 'images/Seas.png'},
	'm' : {label: 'Mountains', image: 'images/Mountains.png'},
	'f' : {label: 'Forests', image: 'images/Forests.png'},
	's' : {label: 'Swamps', image: 'images/Swamps.png'},
	
	'df' : {label: 'dforest', image: 'images/ForestDeserts.png'},
	'dm' : {label: 'dmountain', image: 'images/MountainDeserts.png'},
	'dn' : {label: 'desert', image: 'images/Deserts.png'},
	'pf' : {label: 'pforest', image: 'images/ForestPlains.png'},
	'pm' : {label: 'pmountain', image: 'images/MountainPlains.png'},
	'pn' : {label: 'plains', image: 'images/Plains.png'},
	'sf' : {label: 'sforest', image: 'images/ForestSnows.png'},
	'sm' : {label: 'smountain', image: 'images/MountainSnows.png'},
	'sn' : {label: 'snow', image: 'images/Snows.png'},
	'wb' : {label: 'beach', image: 'images/Beaches.png'},
	'wo' : {label: 'ocean', image: 'images/Oceans.png'},
	'ws' : {label: 'sea', image: 'images/Seas.png'},
};

var Tile = function(label, x, y, w, h, image)
{
	this.Label = label;
	this.X = x;
	this.Y = y;
	this.Width = w;
	this.Height = h;
	this.Image = image;
}

var Castle = function(data)
{
	this.Label = data.label;
	
	if( data.x )	this.X = data.x;
	if( data.y )	this.Y = data.y;
	
	// array of coordinates of each tile that this castle occupies
	//if( data.tiles )	this.Tiles = data.tiles;
	if( data.tile_location )	this.Tile = data.tile_location;
	
	if( data.imageSrc )	this.ImageSrc = data.imageSrc || '';
	if( data.image )	this.Image = data.image || new Image();
	
	this.Owner = '';
}

var Map = function(width, height, tilePixelWidth, tilePixelHeight)
{
	// Dimensions
	this.GridWidth = width;
	this.GridHeight = height;
	this.TilePixelWidth = tilePixelWidth;
	this.TilePixelHeight = tilePixelHeight;
	
	// 2-d matrix of the map
	this.TileMap = [];
}

Map.prototype.get_pixel_width = function() { return this.GridWidth * this.TilePixelWidth; }
Map.prototype.get_pixel_height = function() { return this.GridHeight * this.TilePixelHeight; }
Map.prototype.get_map_matrix = function() { return this.TileMap; }
Map.prototype.get_tile = function(x, y) { return this.TileMap[x][y]; }
Map.prototype.set_tile = function(tile, x, y) { this.TileMap[x][y] = tile; }

Map.prototype.server_generate = function(data)
{
	this.GridWidth = data.width;
	this.GridHeight = data.height;
	this.keys = data.keys;
	
	for( var i = 0; i < this.GridWidth; ++i )
	{
		var row = []
		for( var j = 0; j < this.GridHeight; ++j )
		{
			var k = data.matrix[i][j];
			
			var tile = new Tile(TerrainSrv[k].label, i, j, this.TilePixelWidth, this.TilePixelHeight, TerrainSrv[k].image);
			row.push(tile);
		}
		this.TileMap.push(row);
	}
}

Map.prototype.generate = function(data)
{
	this.GridWidth = data.width;
	this.GridHeight = data.height;
	this.keys = data.keys;
	
	for( var i = 0; i < this.GridWidth; ++i )
	{
		var row = []
		for( var j = 0; j < this.GridHeight; ++j )
		{
			var k = data.matrix[i][j];
			
			var tile = new Tile(Terrain[k].label, i, j, this.TilePixelWidth, this.TilePixelHeight, Terrain[k].image());
			row.push(tile);
		}
		this.TileMap.push(row);
	}
}

Map.prototype.generate_random = function(width,height)
{
	var t = ['p','w','m','f','s'];

	if( width )
	{
		this.GridWidth = width;
	}
	if( height )
	{
		this.GridHeight = height;
	}

	for( aaa in Terrain )
	{
		console.log( Terrain[aaa].label + " ~ " + Terrain[aaa].image() + " ~ " + Terrain[aaa].image().src);
	}

	for(var i = 0; i < this.GridWidth; ++i)
	{
		var row = [];
		for(var j = 0; j < this.GridHeight; ++j)
		{
			var r = rand(t.length);
			var tile = new Tile(Terrain[ t[r] ].label, i, j, this.TilePixelWidth, this.TilePixelHeight, Terrain[ t[r] ].image());
		
			row.push(tile);
		}
		this.TileMap.push(row);
	}
}

// TODO : build the core_engine
var core_engine = function(gid)
{
	this.game_id = gid;
}

core_engine.prototype.server_init = function(parameters)
{
	console.log( '\t :: core_engine.server_init' );
	var tileWidth = parameters.tileWidth || 32;
	var tileHeight = parameters.tileHeight || 32;
	var mapWidth = parameters.width || 1280 / tileWidth;
	var mapHeight = parameters.height || 800 / tileHeight;
	var numOfCastles = parameters.numberOfCastles || 100;
	
	this.map = new Map(mapWidth, mapHeight, tileWidth, tileHeight);
	this.map.server_generate(parameters.input);
	this.server_generate_castles({numberOfCastles: numOfCastles, imageSrc:"images/CastleRed.32.png"});
}

core_engine.prototype.client_init = function(parameters)
{
	var tileWidth = parameters.tileWidth || 32;
	var tileHeight = parameters.tileHeight || 32;
	var mapWidth = parameters.width || 1280 / tileWidth;
	var mapHeight = parameters.height || 800 / tileHeight;
	
	// the client should eventually get map data from the server
	// for the time-being both client and server draw from the
	// same map input file so we'll leave that step out
	this.map = new Map(mapWidth, mapHeight, tileWidth, tileHeight);
	this.map.generate(parameters.input);
	
	console.log( ' map generated and initialized ' + this.map );
	console.log( ' from client_init parameters.data: ' + parameters.data.length );
	this.init_castles({castles:parameters.data, imgsrc:parameters.imgsrc});
}

core_engine.prototype.initialize = function(parameters)
{
	var tileWidth = parameters.tileWidth || 32;
	var tileHeight = parameters.tileHeight || 32;
	var mapWidth = parameters.width || 1280 / tileWidth;
	var mapHeight = parameters.height || 800 / tileHeight;
	
	this.map = new Map(mapWidth, mapHeight, tileWidth, tileHeight);
	
	if( parameters.input )
	{
		this.map.generate(parameters.input);
	}
	else
	{
		this.map.generate_random();
	}
	this.generate_castles({numberOfCastles: 100, imageSrc:"images/CastleRed.32.png"});
}

core_engine.prototype.init_castles = function(parameters)
{
	this.castles = [];
	var img = new Image();
	img.src = parameters.imgsrc;
	
	console.log( ' -parameters.castles.length = ' + parameters.castles.length);
	
	for(var i = 0; i < parameters.castles.length; ++i)
	{
		var c = parameters.castles[i];
		var castle = new Castle({label:c.Label, tile_location:c.Tile, image:img});
		this.castles.push(castle);
		//console.log( ' core_engine.init_castles: c.X ' + c.X + ' c.Y ' + c.Y + ' c ' + parameters.castles[i]);
		//var keys = Object.keys( parameters.castles[i] );
		/*for( k in keys )
		{
			console.log( ' k: ' + keys[k] );
		}*/
		this.map.get_tile(c.Tile.x, c.Tile.y).Castle = castle;
	}
}

core_engine.prototype.server_generate_castles = function(parameters)
{
	console.log( '\t :: core_engine.server_generate_castles' );
	parameters = parameters || {};
	var num_of_castles = parameters.numberOfCastles || rand(10);
	var image_src = parameters.imageSrc || "images/CastleRed.32.png";
	this.castles = [];

	for(var i = 0; i < num_of_castles; ++i)
	{
		// need label
		// pixel_x and pixel_y - it's better to just find the grid coordinates
		// image
		// randomly choose a starting coordinate, make sure to stay away from
		// the edges
		
		var row = rand(this.map.GridWidth);
		var col = rand(this.map.GridHeight);

		if( this.map.get_tile(row,col).Label.toLowerCase() == 'ocean' || this.map.get_tile(row,col).Label.toLowerCase() == 'sea' )
				continue;
		
		var castle = new Castle({label:'Castle-' + i, tile_location:{x:row, y:col}, imageSrc:image_src});
		this.castles.push(castle);
		
		// insert castle to tile
		this.map.get_tile(row,col).Castle = castle;
	}
	//console.log( '\t :: core_engine.server_generate_castles completed :: this.castles ' + this.castles + ' length: ' + this.castles.length );
}

core_engine.prototype.generate_castles = function(parameters)
{
	parameters = parameters || {};
	var num_of_castles = parameters.numberOfCastles || rand(10);
	var image_src = parameters.imageSrc || "images/CastleRed.32.png";
	this.castles = [];
	
	var img = new Image();
	img.src = image_src;
	console.log( 'IMAGE: ' + img + ' :: ' + img.src );
	for(var i = 0; i < num_of_castles; ++i)
	{
		// need label
		// pixel_x and pixel_y - it's better to just find the grid coordinates
		// image
		// randomly choose a starting coordinate, make sure to stay away from
		// the edges
		
		var row = rand(this.map.GridWidth);
		var col = rand(this.map.GridHeight);

		if( this.map.get_tile(row,col).Label.toLowerCase() == 'ocean' || this.map.get_tile(row,col).Label.toLowerCase() == 'sea' )
				continue;
		
		var castle = new Castle({label:'Castle-' + i, tile_location:{x:row, y:col}, image:img});
		this.castles.push(castle);
		
		// insert castle to tile
		this.map.get_tile(row,col).Castle = castle;
	}
}

// Main update loop
/*
game_core.prototype.update = function(t) {
	// work out the delta time
	this.dt = this.last_frame_time ? ( (t - this.last_frame_time) / 1000.0 ).fixed() : 0.016;
	
	// update the game specifics
	if( !this.server )
	{
		this.client_update();
	}
	else
	{
		this.server_update();
	}
	
	// schedule next update
	this.updateid = window.requestAnimationFrame( this.update.bind(this), this.viewport );
}*/

var game_engine = module.exports = core_engine;

// Utilities
function rand(ulimit)
{
	return Math.floor(Math.random() * ulimit);
}

// Helper functions

// (4.22208334636).fixed(n) will return fixed point value to n places, default n = 3
Number.prototype.fixed = function(n) { n = n || 3; return parseFloat(this.toFixed(n)); };