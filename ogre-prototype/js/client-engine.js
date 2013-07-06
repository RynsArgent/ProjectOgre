
////////////////////////////////////////////////////////////////////////////////////////////
// http://paulirish.com/2011/requestanimationframe-for-smart-animating/
// http://my.opera.com/emoller/blog/2011/12/20/requestanimationframe-for-smart-er-animating
 
// requestAnimationFrame polyfill by Erik Möller
// fixes from Paul Irish and Tino Zijdel
 
(function() {
    var lastTime = 0;
    var vendors = ['ms', 'moz', 'webkit', 'o'];
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
        window.cancelAnimationFrame = window[vendors[x]+'CancelAnimationFrame']
                                   || window[vendors[x]+'CancelRequestAnimationFrame'];
    }
 
    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function() { callback(currTime + timeToCall); },
              timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };
 
    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        };
}());
////////////////////////////////////////////////////////////////////////////////////////////

window.onload = function() {
	var screen = {
		width : 1280,
		height : 800
	};
	
	Scroll = {
		x: 0,
		y: 0
	}
	
	var Keys = {
		UP : 38,
		DOWN : 40,
		LEFT : 37,
		RIGHT : 39,
		W : 87,
		S : 83,
		A : 65,
		D : 68
	}
	
	var canvas = document.getElementById('viewport');
	canvas.width = screen.width;
	canvas.height = screen.height;
	
	console.log( 'screen [' + screen.width + ', ' + screen.height + ']' );
	console.log( 'clientWidth: ' + document.body.clientWidth + ' : clientHeight: ' + document.body.clientHeight );
	
	var logs = document.getElementById('log_container');
	logs.style.width = (document.body.clientWidth - screen.width - 10) + 'px';
	logs.style.height = screen.height + 'px';
	
	var tools = document.getElementById('tools_container');
	tools.style.width = '100%';
	tools.style.height = '300px';
	
	var ctx = canvas.getContext('2d');
	
	
	var game_client = function()
	{
		this.players = [];
	
		this.logger = function(msg) {
			var logElem = document.getElementById('log');
			
			var li = document.createElement('li');
			li.appendChild( document.createTextNode( msg ) );
			logElem.insertBefore( li, logElem.firstChild );
		}
		
		this.on_viewportclicked = function(e, data)
		{
			this.socket.emit('mouseclick', {x:e.clientX, y:e.clientY});
			console.log( 'on_viewportclicked ' + e );
			//this.socket.emit("{'player':'" + this.myid + "',action:'mouseclick','x':" 
			//	+ e.clientX + ",'y':" + e.clientY + "}");
		}
	
		this.client_onconnected = function(data)
		{
			this.players.push(data.id);
			this.logger( 'You have connected as ' + data.id );
			
			this.myid = data.id;
		}
		
		this.client_ondisconnected = function(data)
		{
		
		}
		
		this.client_onserverupdate_recieved = function(data)
		{
		
		}
		
		this.client_onnetmessage = function(data)
		{
			this.logger(data);			
		}
		
		this.client_onplayerjoin = function(data)
		{
			this.players.push(data.id);
			this.logger( data.id + ' has connected' );
		}
		
		this.client_onothersclick = function(data)
		{
			this.logger( data.pid + ' clicked on map @ {' + data.x + ',' + data.y + '}' );
		}
		
		this.socket = io.connect();
		// When we connect, we are not 'connected' until we have a server id
    // and are placed in a game by the server. The server sends us a message for that.
		this.socket.on('connect', function() {
			// TODO: handle this case - probably should set a state to CONNECTING
		});
		// Handle when we connect to the server, showing state and storing id's.
    this.socket.on('onconnected', this.client_onconnected.bind(this));
		// Sent when we are disconnected (network, server down, etc)
		this.socket.on('ondisconnected', this.client_ondisconnected.bind(this));
		// Sent each tick of the server simulation. This is our authoritive update
    this.socket.on('onserverupdate', this.client_onserverupdate_recieved.bind(this));
    // On error we just show that we are not connected for now. Can print the data.
    this.socket.on('error', this.client_ondisconnected.bind(this));
    // On message from the server, we parse the commands and send it to the handlers
    this.socket.on('message', this.client_onnetmessage.bind(this));
    
    this.socket.on('playerjoined', this.client_onplayerjoin.bind(this));
    
    this.socket.on('onothersclick', this.client_onothersclick.bind(this));
	}
	
	var client = new game_client();
	
	var game = new core_engine(324);
	game.initialize({width:100,height:100,input:tmp_data});
	
	screen.row_lo = 0;
	screen.row_hi = screen.width / game.map.TilePixelWidth;
	screen.col_lo = 0;
	screen.col_hi = screen.height / game.map.TilePixelHeight;
	
	screen.identity = { 
		rlo: 0, 
		rhi: screen.width / game.map.TilePixelWidth,
		clo: 0,
		chi: screen.height / game.map.TilePixelHeight
	};
	
	window.addEventListener('keydown', handleKeyDown, false);
	canvas.addEventListener('click', handleCanvasClick, false);
	draw();
	
	function handleCanvasClick(e)
	{
		// position of the mouse.x and mouse.y is relative to the left and top of the canvas
		// element. - At current 0,0. 
		// To calculate what cell within the matrix is clicked. We need to convert the
		// coordinate system into matrix coordinates.
		// 
		// mouse.x range from 0 to screen.width
		// mouse.y range from 0 to screen.height
		// 
		// absrow = mouse.x / game.map.TilePixelWidth : gives us the row coordinate
		// abscol = mouse.y / game.map.TilePixelHeight : gives us the col coordinate
		//
		// we need to offset for scrolling
		// relrow = (Scroll.x / game.map.TilePixelWidth) + absrow;
		// relcol = (Scroll.y / game.map.TilePixelHeight) + abscol;
		absrow = Math.floor(Scroll.x / game.map.TilePixelWidth);
		relrow = absrow + Math.floor(e.clientX / game.map.TilePixelWidth);
		
		abscol = Math.floor(Scroll.y / game.map.TilePixelHeight);
		relcol = abscol + Math.floor(e.clientY / game.map.TilePixelHeight);
		
		var cellAt = game.map.get_tile(relrow, relcol);
		
		client.logger('you clicked on map @ {' + e.clientX + ',' + e.clientY 
			+ '} \n cell @ {' + relrow + ',' + relcol + '} the cell contains a ' 
			+ cellAt.Label + ( cellAt.hasOwnProperty('Castle') ? ' and a Castle' : '' ));
		
		client.on_viewportclicked(e, {});
	}
	
	// TODO: input should be extracted to a handler
	function handleKeyDown(e)
	{
		switch(e.keyCode) 
		{
			case Keys.W:
				Scroll.y -= ((Scroll.y - game.map.TilePixelHeight) >= 0) ? game.map.TilePixelHeight : 0;
				break;
			case Keys.S:
				Scroll.y += ((Scroll.y + game.map.TilePixelHeight) > game.map.get_pixel_height() - screen.height) ? 0 : game.map.TilePixelHeight;
				break;
			case Keys.A:
				Scroll.x -= ((Scroll.x - game.map.TilePixelWidth) >= 0) ? game.map.TilePixelWidth : 0;
				break;
			case Keys.D:
				Scroll.x += ((Scroll.x + game.map.TilePixelWidth) > game.map.get_pixel_width() - screen.width) ? 0 : game.map.TilePixelWidth;
				break;
		}
		document.getElementById('scrollx').innerHTML = Scroll.x;
		document.getElementById('scrolly').innerHTML = Scroll.y;
	}
	
	function draw()
	{
		requestAnimationFrame(draw);
		ctx.fillStyle = '#ffffff';
		ctx.fillRect(0, 0, canvas.width, canvas.height);
		
		var startRow = Math.floor(Scroll.x / game.map.TilePixelWidth);
		var startCol = Math.floor(Scroll.y / game.map.TilePixelHeight);
		var rowCount = startRow + Math.floor(canvas.width / game.map.TilePixelWidth) + 1;
		var colCount = startCol + Math.floor(canvas.height / game.map.TilePixelHeight) + 1;
		
		rowCount = ((startRow + rowCount) > game.map.GridWidth) ? game.map.GridWidth : rowCount;
		colCount = ((startCol + colCount) > game.map.GridHeight) ? game.map.GridHeight : colCount;
		
		for(var row = startRow; row < rowCount && row < game.map.GridWidth; row++)
		{
			for(var col = startCol; col < colCount && col < game.map.GridHeight; col++)
			{
				var tilePositionX = game.map.TilePixelWidth * row;
				var tilePositionY = game.map.TilePixelHeight * col;
				
				tilePositionX -= Scroll.x;
				tilePositionY -= Scroll.y;
				
				var tile = game.map.get_tile(row, col);
				//var img = tile.Image;
				
				ctx.drawImage(tile.Image, tilePositionX, tilePositionY, game.map.TilePixelWidth, game.map.TilePixelHeight);
				ctx.drawImage(tile.Image, tilePositionX, tilePositionY, game.map.TilePixelWidth, game.map.TilePixelHeight);
				if( tile.hasOwnProperty('Castle') )
				{
					var castle = tile.Castle;
					//console.log( ' huh !? ' + castle.Image + ' ~ ' + castle.Image.src );
					ctx.drawImage(castle.Image, tilePositionX, tilePositionY, game.map.TilePixelWidth, game.map.TilePixelHeight);
				}
			}
		}
		
		/*
		for(var i in game.castles)
		{
			var castle = game.castles[i];
			
			if( castle.Tile.x >= startRow && castle.Tile.x < startRow + rowCount 
					&& castle.Tile.y >= startCol && castle.Tile.y < startCol + colCount )
			{
				var tpx = game.map.TilePixelWidth * castle.Tile.x;
				var tpy = game.map.TilePixelHeight * castle.Tile.y;
				
				tpx -=  Scroll.x;
				tpy -= Scroll.y;
				
				console.log( ' tpx: ' + tpx + '  tpy: ' + tpy + ' ' + game.map.TilePixelWidth + ' ' + game.map.TilePixelHeight + ' tile.x ' + castle.Tile.x + ' tile.y ' + castle.Tile.y + '  scrollx ' + Scroll.x + '   scrolly ' + Scroll.y );
				//var cimg = castle.Image;
				
				ctx.drawImage(castle.Image, tpx, tpy, game.map.TilePixelWidth, game.map.TilePixelHeight);
			}
		}*/
		
		//setTimeout(draw, 1);	// should probably go into update function? hrm
	}
}
