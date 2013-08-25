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
        bgImage, stImage;
        
    label = params.label || '';
    row = params.row || -1;
    col = params.col || -1;
    index = params.index || -1;
    width = params.width || 0;
    height = params.height || 0;
    
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

///////////////////////////////////////////////////////////////////////////////

jQuery(document).ready(function () {
    'use strict';
    var gameClient,
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
    
    jQuery("#enterBtn").click(function(ev) {
        var username = jQuery("#username").val();
        gameClient.requestLogin(username);
    });

    gameClient = (function () {
        // Declare private variables and methods
        var that, player,
            
            tick, update, animate, draw,
            
            handleInput, resizeScreen,
            
            socket = io.connect(),
            
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
            
            map;
            
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

        // connection communications //
        // Handle when we connect to the server, showing state and storing id's.
        socket.on('onconnected', function (data) {
            console.log('gameClient : onconnected');
            console.log(data);
            
            player = {
                userid : data.userid,
                userip : data.userip
            };
        });
        
        socket.on('onloggedin', function (data) {
            console.log('gameClient : onloggedin');
            console.log(data);
            
            if (data.status === 'success') {
                player.username = data.username;
                
                // TODO: we need to also grab the map data from the server.
                // but for now we are going to cheat and use a pre-defined
                // map data file
                map = createMap(tmp_data);
                
                //tick();
                draw();
                
                jQuery("div#frontPage").hide("slide", { direction: "left" }, "slow", function() {
                    jQuery("div#gamePage").show("slide", { direction: "right" }, "slow");
                });
            }
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
            /*console.log({
                label : "executing draw",
                scroll : scroll,
                tilepixelwidth : map.getTileWidthInPixels(),
                tilepixelheight : map.getTileHeightInPixels(),
                startRow : startRow,
                startCol : startCol,
                rowCount : rowCount,
                colCount : colCount,
                numOfRows : map.numberOfRows,
                numOfCols : map.numberOfColumns
            });*/
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
                }
            }
        };  // end draw

        // define privileged functions
        return {
            requestLogin : function (username) {
                socket.emit('requestlogin', username);
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
            
            handleKeyboard : function (event) {
                switch (event.keyCode) {
                    case Keys.W:
                    case Keys.w:
                        if (scroll.y - map.getTileHeightInPixels() >= 0) {
                            scroll.y -= map.getTileHeightInPixels();
                        }
                        break;
                    case Keys.S:
                    case Keys.s:
                        if (scroll.y + map.getTileHeightInPixels() <= 
                            map.getHeightInPixels() - gamescreen.height) {
                            
                            scroll.y += map.getTileHeightInPixels();
                        }
                        break;
                    case Keys.A:
                    case Keys.a:
                        if (scroll.x - map.getTileWidthInPixels() >= 0) {
                            scroll.x -= map.getTileWidthInPixels();
                        }
                        break;
                    case Keys.D:
                    case Keys.d:
                        if (scroll.x + map.getTileWidthInPixels() <=
                            map.getWidthInPixels() - gamescreen.width) {
                            
                            scroll.x += map.getTileWidthInPixels();   
                        }
                        break;
                }
                
                jQuery("#scrollx").html(scroll.x);
                jQuery("#scrolly").html(scroll.y);
            }
        };
        
    }()); // end gameClient
    
    jQuery(window).keypress(function (event) {
        gameClient.handleKeyboard(event);
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
