var Tile = function(label, x, y, w, h, image)
{
	this.Label = label;
	this.X = x;
	this.Y = y;
	this.Width = w;
	this.Height = h;
	this.Image = image;
}

Tile.prototype.get_label = function() { return this.Label; }
Tile.prototype.get_x = function() { return this.X; }
Tile.prototype.get_y = function() { return this.Y; }
Tile.prototype.get_width = function() { return this.Width; }
Tile.prototype.get_height = function() { return this.Height; }
Tile.prototype.get_image = function() { return this.Image; }

var Map = function(width, height, tilePixelWidth)
{
	// dimensions
	this.Grid = { width: width, height: height };
	
	this.TilePixelWidth = tilePixelWidth;
	this.TilePixelHeight = tilePixelWidth;
	
	// map matrix
	this.TileMap = [];
}

Map.prototype.get_pixel_width = function() { return this.Grid.width * this.TilePixelWidth; }
Map.prototype.get_pixel_height = function() { return this.Grid.height * this.TilePixelWidth; }
Map.prototype.get_grid = function() { return this.Grid; }
Map.prototype.get_matrix = function() { return this.TileMap; }
Map.prototype.get_tile = function(x, y) { return this.TileMap[x][y]; }

Map.prototype.set_tile = function(tile, x, y) { this.TileMap[x][y] = tile; }

var Terrain = {
	'p' : {label: 'Plains', image: function(){ var img = new Image(); img.src = 'images/Plains.png'; return img;}},
	'w' : {label: 'Sea', image: function(){ var img = new Image(); img.src = 'images/Sea.png'; return img;}},
	'm' : {label: 'Mountains', image: function(){ var img = new Image(); img.src = 'images/Mountain.png'; return img;}},
	'f' : {label: 'Forests', image: function(){ var img = new Image(); img.src = 'images/Forest.png'; return img;}},
	's' : {label: 'Swamps', image: function(){ var img = new Image(); img.src = 'images/Swamp.png'; return img;}}
};

/*
 *	The input will be a 2 dimensional string consisting of the above
 *	letters representing what type of terrain for that tile.
 */
Map.prototype.initialize_map = function(input)
{
	var rows = input.length;
	var columns = input[0].length;
	
	if( rows != this.Grid.width )
		this.Grid.width = rows;
	if( columns != this.Grid.height )
		this.Grid.height = columns;
		
	for(var i = 0; i < rows; ++i)
	{
		var row = [];
		for(var j = 0; j < columns; ++j)
		{
			var t = new Tile(Terrain[ input[i][j] ].label, i, j, this.TilePixelWidth, this.TilePixelHeight, Terrain[ input[i][j] ].image());
			row.push(t);
		}
		this.TileMap.push(row);
	}
}

Map.prototype.generate_random = function(width,height)
{
	var t = ['p','w','m','f','s'];
	
	if( width )
	{
		this.Grid.width = width;
	}
	if( height )
	{
		this.Grid.height = height;
	}
	
	for( aaa in Terrain )
	{
		console.log( Terrain[aaa].label + " ~ " + Terrain[aaa].image() + " ~ " + Terrain[aaa].image().src);
	}
	
	for(var i = 0; i < this.Grid.width; ++i)
	{
		var row = [];
		for(var j = 0; j < this.Grid.height; ++j)
		{
			var r = rand(t.length);
			var tile = new Tile(Terrain[ t[r] ].label, i, j, this.TilePixelWidth, this.TilePixelHeight, Terrain[ t[r] ].image());
			
			row.push(tile);
		}
		this.TileMap.push(row);
	}
}

var Castle = function(label, x, y, w, h, image)
{
	
}



// Utilities
function rand(ulimit)
{
	return Math.floor(Math.random() * ulimit);
}
