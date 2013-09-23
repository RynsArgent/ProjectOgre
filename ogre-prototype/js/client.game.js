$.noConflict();

/////////////////// UTILITIES /////////////////////////////////////////////////
// Create object function using another object as its prototype
if (typeof Object.create !== 'function') {
    Object.create = function (o) {
        var F = function () {};
        F.prototype = o;
        return new F;
    };
}
///////////////////////////////////////////////////////////////////////////////
var resources = (function () {
    var dict = {
        'CastleRed' : {label: 'CasteRed', image: 'images/CastleRed.png', width: 32, height: 32}
    }
    
    return {
        preloadAll : function () {
            var keys = Object.keys(dict),
                i,
                img;
            for(i = 0; i < keys.length; i += 1) {
                img = new Image();
                img.src = dict[ keys[i] ].image;
                dict[ keys[i] ].imageObject = img;
            }
        },
        
        getImageObject : function (key) {
            var img;
            if (dict[key].imageObject === undefined) {
                img = new Image();
                img.src = dict[key].image;
                dict[key].imageObject = img;
            }
            return dict[key].imageObject;
        },
        
        getLabelFor : function (key) {
            return dict[key].label;
        }
    }
}());

var terrain_dictionary = (function () {
    var dict = {
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
    
    return {
        preloadAll : function () {
            var keys = Object.keys(dict),
                i,
                img;
            for(i = 0; i < keys.length; i += 1) {
                img = new Image();
                img.src = dict[ keys[i] ].image;
                dict[ keys[i] ].imageObject = img;
            }
        },
        
        getImageObject : function (key) {
            var img;
            if (dict[key].imageObject === undefined) {
                img = new Image();
                img.src = dict[key].image;
                dict[key].imageObject = img;
            }
            return dict[key].imageObject;
        },
        
        getLabelFor : function (key) {
            return dict[key].label;
        }
    };
}());
/*        col
        0    1   2   3   4
    0  [ 0][ 1][ 2][ 3][ 4]
row 1  [ 5][ 6][ 7][ 8][ 9]
    2  [10][11][12][13][14]
    
    rows: 3     columns: 5
    
    index = row * columns + col
*/
var createTile = function (params) {
    var label, 
        index, row, col, 
        width, height,
        bgImage, stImage,
        settlement;
        
    label = params.label || '';
    row = params.row;
    col = params.col;
    index = params.index;
    width = params.width;
    height = params.height;
    
    if (params.bgImage !== undefined) {
        bgImage = params.bgImage;
    }
    if (params.stImage !== undefined) {
        stImage = params.stImage;
    }
    
    return {
        getLabel : function () {
            return label;
        },
        getRow : function () {
            return row;
        },
        getColumn : function () {
            return col;
        },
        getTilePosition : function () {
            return {row:row,col:col};
        },
        getIndex : function () {
            return index;
        },
        getWidth : function () {
            return width;
        },
        getHeight : function () {
            return height;
        },
        getBackgroundImage : function () {
            return bgImage;
        },
        getStationaryImage : function () {
            return stImage;
        },
        setSettlement : function (st) {
            settlement = st;
        },
        getSettlement : function () {
            return settlement;
        }
    };
};

var createMap = function (data) {//(rows, columns, tileWidthInPixels, tileHeightInPixels) {
    var rows = data.rows || data.height,
        columns = data.columns || data.width,
        tileWidthInPixels = data.tilePixelWidth,
        tileHeightInPixels = data.tilePixelHeight,
        tilemap = [],
        keys = data.keys;

        console.log("createMap call: keys = " + keys);        
        (function () {
            var i, j, k, row;
            console.log("createMap : generating map");
            // generate the map from data
            for (i = 0; i < rows; i += 1) {
                row = [];
                for (j = 0; j < columns; j += 1) {
                    k = data.matrix[i][j];
                    
                    row.push(createTile({
                        label : terrain_dictionary.getLabelFor(k),
                        row : i,
                        col : j,
                        index : i * columns + j,    // index = row * columns + col
                        width : tileWidthInPixels,
                        height : tileHeightInPixels,
                        bgImage : terrain_dictionary.getImageObject(k)
                    }));
                }
                tilemap.push(row);
            }
        }());
    
    return {
        getWidthInPixels : function () {
            return columns * tileWidthInPixels;
        },
        getHeightInPixels : function () {
            return rows * tileHeightInPixels;
        },
        getTileWidthInPixels : function () {
            return tileWidthInPixels;
        },
        getTileHeightInPixels : function () {
            return tileHeightInPixels;
        },
        numberOfRows : rows,
        numberOfColumns : columns,
        getTileMap : function () {
            return tilemap;
        },
        getTileAt : function (row, col) {
            return tilemap[row][col];
        },
        setTileAt : function (row, col, tile) {
            tilemap[row][col] = tile;
        },
        
        generate : function (data) {
            var i, j, key, tile, row;
            
            // danger : clears the tilemap if it has existing data
            tilemap = [];
            
            if (data.columsn !== undefined) {
                columns = data.columns;
            }
            
            if (data.rows !== undefined) {
                rows = data.rows;
            }
            
            if (data.keys !== undefined) {
                keys = data.keys;
            }
            
            // index = row * columns + col
            for (i = 0; i < rows; i += 1) {
                row = [];
                for (j = 0; j < columns; j += 1) {
                    k = data.matrix[i][j];
                    
                    row.push(createTile({
                        label : terrain_dictionary.getLabelFor(k),
                        row : i,
                        col : j,
                        index : i * columns + j,
                        width : tileWidthInPixels,
                        height : tileHeightInPixels,
                        bgImage : terrain_dictionary.getImageObject(k)
                    }));
                }
                tilemap.push(row);
            }
        }
    };
};

var createSettlement = function (data) {
     var imgsrc = data.imgsrc,
        imgkey = data.imagekey,
        size = data.size,   // size of settlement is determined by tiles
                            // small : 1 tile, medium : 2x2 tiles, large : 3x3 tiles
        containingTiles = data.containingTiles, // an array of the tile locations in which contains this settlement
        
        name = data.name || '',
        
        // attributes
        population = data.population || Math.floor(Math.random() * 100),
        happiness = data.happiness || Math.floor(Math.random() * 100),
        income = data.income || Math.floor(Math.random() * 100),
        harvest = data.harvest || Math.floor(Math.random() * 100),
        gold = data.gold || Math.floor(Math.random() * 100),
        food = data.food || Math.floor(Math.random() * 100),
        order = data.order || Math.floor(Math.random() * 100),
        
        owner = data.owner;
        
        return {
            ownedBy : function () {
                return owner;
            },
            
            getSize : function () {
                return size;
            },
            
            containedInTiles : function () {
                return containingTiles;
            },
            
            getImageSource : function () {
                return imgsrc;
            },
            
            containingTiles : containingTiles,
            Name : name,
            owner : owner,
            size : size,
            imagesource : imgsrc,
            imagekey : imgkey,
            
            Population : population,
            Happiness : happiness,
            Income : income,
            Harvest : harvest,
            Gold : gold,
            Food : food,
            Order : order
        };
};

var createClient = function () {
    // Declare private variables and methods
    var that, player,
        
        tick, update, animate, draw,
        
        handleInput, resizeScreen,
        
        //socket = io.connect(),
        
        bgCanvas = jQuery("#bg").get(0),
        stCanvas = jQuery("#st").get(0),
        mainCanvas = jQuery("#main").get(0),
        fgCanvas = jQuery("#fg").get(0),
        
        bgContext = bgCanvas.getContext('2d'),
        stContext = stCanvas.getContext('2d'),
        mainContext = mainCanvas.getContext('2d'),
        fgContext = fgCanvas.getContext('2d'),
        
        gamescreen = {},
        
        scroll = {x: 0, y: 0},
        
        map, settlements = [], owned_settlements = [],
        
        Keys = {
	        UP : 38,
	        DOWN : 40,
	        LEFT : 37,
	        RIGHT : 39,
	        W : 87,
	        S : 83,
	        A : 65,
	        D : 68,
	        w : 119,
	        s : 115,
	        a : 97,
	        d : 100
        };
        
    resizeScreen = function (dimensions) {
        gamescreen.width = dimensions.width;
        gamescreen.height = dimensions.height;
        
        bgCanvas.width = gamescreen.width;
        bgCanvas.height = gamescreen.height;
        
        stCanvas.width = gamescreen.width;
        stCanvas.height = gamescreen.height;
        
        mainCanvas.width = gamescreen.width;
        mainCanvas.height = gamescreen.height;
        
        fgCanvas.width = gamescreen.width;
        fgCanvas.height = gamescreen.height;
    };
        

    resizeScreen({
        width : document.body.clientWidth,
        height : document.body.clientHeight
    });

    ///////////////////////////////
    
    tick = function () {
        requestAnimationFrame(tick);
        
        console.log( 'gameClient: called tick' );
    };
    
    draw = function () {
        requestAnimationFrame(draw);
        
        bgContext.fillStyle = '#ffffff';
        bgContext.fillRect(0, 0, bgCanvas.width, bgCanvas.height);
        
        stContext.clearRect(0, 0, stCanvas.width, stCanvas.height);
        
        var startRow = Math.floor(scroll.x / map.getTileWidthInPixels()), 
            startCol = Math.floor(scroll.y / map.getTileHeightInPixels()), 
            
            rowCount = startRow + Math.floor(bgCanvas.width / map.getTileWidthInPixels()) + 1, 
            colCount = startCol + Math.floor(bgCanvas.height / map.getTileHeightInPixels()) + 1,

            row, col,
            tilePositionX, tilePositionY,
            tile, img;
            
        if ((startRow + rowCount) > map.numberOfRows) {
            rowCount = map.numberOfRows;
        }
        
        if ((startCol + colCount) > map.numberOfColumns) {
            colCount = map.numberOfColumns;
        }

        for (row = startRow; row < rowCount && row < map.numberOfRows; row += 1) {
            for (col = startCol; col < colCount && col < map.numberOfColumns; col += 1) {
                tilePositionX = map.getTileWidthInPixels() * row;
                tilePositionY = map.getTileHeightInPixels() * col;
                
                tilePositionX -= scroll.x;
                tilePositionY -= scroll.y;
                
                tile = map.getTileAt(row, col);
                img = tile.getBackgroundImage();
                
                bgContext.drawImage(img,
                    tilePositionX, 
                    tilePositionY,
                    map.getTileWidthInPixels(),
                    map.getTileHeightInPixels());
                    
                if (map.getTileAt(row,col).getSettlement() !== undefined) {
                    if (map.getTileAt(row,col).getSettlement().containingTiles[0].row === row &&
                        map.getTileAt(row,col).getSettlement().containingTiles[0].col === col) {
                        img = resources.getImageObject(map.getTileAt(row,col).getSettlement().imagekey);
                        
                        stContext.drawImage(img,
                            tilePositionX,
                            tilePositionY,
                            64, 64);
                    }
                }
            }
        }
    };  // end draw

    // define privileged functions
    return {
        onconnected : function (data, callback) {
            player = {
                userid : data.userid,
                userip : data.userip
            };
            
            if (callback !== undefined) {
                callback();
            }
        },
        
        onloggedin : function (data, callback) {
            console.log('gameClient : onloggedin');
            console.log(data);
            var i, j, settlement;
            
            if (data.status === 'success') {
                player.username = data.username;
                
                // TODO: we need to also grab the map data from the server.
                // but for now we are going to cheat and use a pre-defined
                // map data file
                map = createMap(tmp_data);
                
                for (i = 0; i < data.settlements.length; i += 1) {
                    settlement = createSettlement({
                        imgsrc : data.settlements[i].imagesource,
                        imagekey : data.settlements[i].imagekey,
                        containingTiles : data.settlements[i].containingTiles,
                        
                        name : data.settlements[i].Name,
                        owner : data.settlements[i].owner,
                        size : data.settlements[i].size,
                        
                        population : data.settlements[i].Population,
                        happiness : data.settlements[i].Happiness,
                        income : data.settlements[i].Income,
                        harvest : data.settlements[i].Harvest,
                        gold : data.settlements[i].Gold,
                        food : data.settlements[i].Food,
                        order : data.settlements[i].Order
                        
                    });
                    
                    for (j = 0; j < 4; j += 1) {
                        map.getTileAt(settlement.containingTiles[j].row, settlement.containingTiles[j].col).setSettlement(settlement);
                    }
                    settlements.push(settlement);
                }
                
                for (i = 0; i < data.owned_settlements.length; i += 1) {
                     settlement = createSettlement({
                        imgsrc : data.settlements[i].imagesource,
                        imagekey : data.settlements[i].imagekey,
                        containingTiles : data.settlements[i].containingTiles,
                        
                        name : data.settlements[i].Name,
                        owner : data.settlements[i].owner,
                        size : data.settlements[i].size,
                        
                        population : data.settlements[i].Population,
                        happiness : data.settlements[i].Happiness,
                        income : data.settlements[i].Income,
                        harvest : data.settlements[i].Harvest,
                        gold : data.settlements[i].Gold,
                        food : data.settlements[i].Food,
                        order : data.settlements[i].Order
                        
                    });
                    
                    if (settlement.owner !== '') {
                        console.log(settlement.Name + ' - ' + settlement.owner + ' @ ');
                        console.log(settlement.containingTiles[0]);
                    }
                    
                    for (j = 0; j < 4; j += 1) {
                        map.getTileAt(settlement.containingTiles[j].row, settlement.containingTiles[j].col).setSettlement(settlement);
                    }
                    owned_settlements.push(settlement);
                }
                
                //tick();
                draw();
                
                if (callback !== undefined) {
                    callback(data);
                }
            }
        },
        
        screenDimension : function () {
            return gamescreen;
        },
        
        scroll : function () {
            return scroll;
        },
        
        screenResize : function (dimensions) {
            resizeScreen(dimensions);
        },
        
        handleObjectSelected : function (obj) {
            jQuery('#stl_owner').html(obj.owner);
            jQuery('#stl_size').html(obj.size);
            jQuery('#stl_population').html(obj.Population);
            jQuery('#stl_order').html(obj.Order);
            jQuery('#stl_happiness').html(obj.Happiness);
            jQuery('#stl_food').html(obj.Food);
            jQuery('#stl_gold').html(obj.Gold);
            jQuery('#stl_harvest').html(obj.Harvest);
            jQuery('#stl_income').html(obj.Income);
            
            jQuery('#settlement_details')
                .dialog({
                    title : obj.Name
                });
        },
        
        handleKeyboard : function (event) {
            switch (event.keyCode) {
                case Keys.W:
                case Keys.w:
                case Keys.UP:
                    if (scroll.y - map.getTileHeightInPixels() >= 0) {
                        scroll.y -= map.getTileHeightInPixels();
                    }
                    break;
                case Keys.S:
                case Keys.s:
                case Keys.DOWN:
                    if (scroll.y + map.getTileHeightInPixels() <= 
                        map.getHeightInPixels() - gamescreen.height) {
                        
                        scroll.y += map.getTileHeightInPixels();
                    }
                    break;
                case Keys.A:
                case Keys.a:
                case Keys.LEFT:
                    if (scroll.x - map.getTileWidthInPixels() >= 0) {
                        scroll.x -= map.getTileWidthInPixels();
                    }
                    break;
                case Keys.D:
                case Keys.d:
                case Keys.RIGHT:
                    if (scroll.x + map.getTileWidthInPixels() <=
                        map.getWidthInPixels() - gamescreen.width) {
                        
                        scroll.x += map.getTileWidthInPixels();   
                    }
                    break;
            }
            
            jQuery("#scrollx").html(scroll.x);
            jQuery("#scrolly").html(scroll.y);
        },
        
        handleMouseClick : function (event) {
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
	        var absrow = Math.floor(scroll.x / map.getTileWidthInPixels()),
	            relrow = absrow + Math.floor(event.clientX / map.getTileWidthInPixels()),
	        
	            abscol = Math.floor(scroll.y / map.getTileHeightInPixels()),
	            relcol = abscol + Math.floor(event.clientY / map.getTileHeightInPixels()),
	            
	            cellAt = map.getTileAt(relrow, relcol);
	        
	        console.log(relrow + ',' + relcol);
	        
	        var sb = cellAt.getLabel() + '@[' +
	                cellAt.getRow() + ',' +
	                cellAt.getColumn() + '] : ';
	        
	        if (cellAt.getSettlement() !== undefined) {
	            var keys = Object.keys(cellAt.getSettlement());
	            
	            for(var k in keys) {
	                sb += keys[k] + ' ~ ' + cellAt.getSettlement()[keys[k]] + ' _ ';
	            }
	            
	            var settlement = cellAt.getSettlement();
	            
	            this.handleObjectSelected(settlement);
	        }
	        
	        jQuery("#log").prepend(
	            '<li>' + sb + '</li>'
	        );
        }
    };
    
}; // end gameClient

///////////////////////////////////////////////////////////////////////////////

jQuery(document).ready(function () {
    'use strict';
    var socket = io.connect(),
        gameClient = createClient();
    
    jQuery("#enterBtn").click(function(ev) {
        var username = jQuery("#username").val();
        socket.emit('requestlogin', username);
    });
    
    // connection communications //
    // Handle when we connect to the server, showing state and storing id's.
    socket.on('onconnected', function (data) {
        gameClient.onconnected(data);
    });
    socket.on('onloggedin', function (data) {
        gameClient.onloggedin(data, function () {
            jQuery("div#frontPage").hide("slide", { direction: "left" }, "slow", function() {
                jQuery("div#gamePage").show("slide", { direction: "right" }, "slow");
            });
        });
    });
    
    ////////////////////////////////////////////////////////////////////////////
    
    jQuery(window).keydown(function (event) {
        gameClient.handleKeyboard(event);
    });
    
    jQuery("#viewport_container").click(function (event) {
        console.log('click');
        gameClient.handleMouseClick(event);
    });
    
    jQuery(window).resize(function () {
        gameClient.screenResize({
            width: document.body.clientWidth,
            height: document.body.clientHeight
        });
    });
});

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
