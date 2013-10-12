/*
 *  Kevin Phung
 */

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
        'CastleRed' : {label: 'CasteRed', image: 'images/CastleRed.png', width: 32, height: 32},
        'ico_fighter' : {label: 'FighterIcon', image: 'images/fighter_32.png', width: 32, height: 32}
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

var createMap = function (data) {
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
        
        cityname = data.cityname || '',
        
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
            containedInTiles : function () {
                return containingTiles;
            },
            getImageSource : function () {
                return imgsrc;
            },
            getImageKey : function () {
                return imgkey;
            },
            getName : function () {
                return cityname;
            },
            ownedBy : function () {
                return owner;
            },
            getSize : function () {
                return size;
            },
            getPopulation : function () {
                return population;
            },
            getHappiness : function () {
                return happiness;
            },
            getOrder : function () {
                return order;
            },
            getFood : function () {
                return food;
            },
            getGold : function () {
                return gold;
            },
            getIncome : function () {
                return income;
            },
            getHarvest : function () {
                return harvest;
            },
            
            setOwner : function (player) {
                owner = player;
                console.log('set owner = player :: ' + owner + ' ; ' + player);
            },
            setGold : function (n) {
                gold = n;
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

var createArmy = function (data) {
    var kingdom = data.kingdom,
        units = [],
        tilePosition,
        pixelPosition,
        icon;
        
    return {
        belongsTo : function () {
            return kingdom;
        },
        
        getTilePosition : function () {
            return tilePosition;
        },
        
        getIcon : function () {
            return icon;
        },
        
        setIcon : function (ico) {
            icon = ico;
        },
        
        setTilePosition : function (pos) {
            tilePosition = pos;
        }
    }
};

var createKingdom = function (data) {
    var player = data.player,
        color = data.color || '',
        holdfasts = [],
        armies = [],
        
        stats = {
            gold : 0,
            food : 0,
            nholdfasts : holdfasts.length,
            armies : armies.length
        };
        
        return {
            getPlayer : function () {
                return player;
            },
            
            changeGoldBy : function (amount) {
                stats.gold = Math.min(0, stats.gold + amount);
            },
            
            changeFoodBy : function (amount) {
                stats.food = Math.min(0, stats.food + amount);
            },
            
            addArmy : function (army) {
                armies.push(army);
            },

            removeArmy : function (idx) {
                var army = armies[idx];
                armies.splice(armies.indexOf(army), 1);
                return army;
            },

            armySize : function () {
                return armies.length;
            },
            
            getArmy : function (idx) {
                return armies[idx];
            },
            
            hasHowManyHoldfasts : function () {
                return holdfasts.length;
            },
            
            getHoldfast : function (idx) {
                return holdfasts[idx];
            },
            
            addHoldfast : function (holdfast) {
                holdfasts.push(holdfast);
            },
            
            removeHoldfast : function (idx) {
                var holdfast = holdfasts[idx];
                holdfasts.splice(holdfasts.indexOf(holdfast), 1);
                return holdfast;
            }
            
        }
};

var createClient = function () {
    // Declare private variables and methods
    var that, player, kingdoms = [], unitObjects = [],
        
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
        mouse = {x: 0, y: 0, down: false},
        
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
        
        
        var n_tiles_wide, n_tiles_high, column_sections, row_sections, sections = [], i, j, section;
        
        if (map !== null && map !== undefined) {
            n_tiles_wide = Math.floor(gamescreen.width / map.getTileWidthInPixels());
            n_tiles_high = Math.floor(gamescreen.height / map.getTileHeightInPixels());
            
            // map.numberOfRows map.numberOfColumns
            
            column_sections = Math.ceil(map.numberOfColumns / n_tiles_wide);
            row_sections = Math.ceil(map.numberOfRows / n_tiles_high);
            
            for (i = 0; i < row_sections; i += 1) {
                for (j = 0; j < column_sections; j += 1) {
                    section = {
                        mincol : j * n_tiles_wide,
                        maxcol : ((j * n_tiles_wide) + n_tiles_wide),
                        minrow : i * n_tiles_high,
                        maxrow : (i * n_tiles_high) + n_tiles_high
                    }
                
                    if (section.maxcol > map.numberOfColumns) {
                        section.maxcol = map.numberOfColumns;
                        section.mincol = map.numberOfColumns - n_tiles_wide;
                    }
                    
                    if (section.maxrow > map.numberOfRows) {
                        section.maxrow = map.numberOfRows;
                        section.minrow = map.numberOfRows - n_tiles_high;
                    }
                    sections.push(section);
                }
            }
            
            gamescreen.mapsections = sections;
        }
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
        
        var startRow = Math.floor(scroll.y / map.getTileHeightInPixels()), 
            startCol = Math.floor(scroll.x / map.getTileWidthInPixels()), 
            
            rowCount = startRow + Math.floor(bgCanvas.height / map.getTileHeightInPixels()) + 1, 
            colCount = startCol + Math.floor(bgCanvas.width / map.getTileWidthInPixels()) + 1,

            row, col,
            tilePositionX, tilePositionY,
            tile, img, i;
            
        if ((startRow + rowCount) > map.numberOfRows) {
            rowCount = map.numberOfRows;
        }
        
        if ((startCol + colCount) > map.numberOfColumns) {
            colCount = map.numberOfColumns;
        }

        for (row = startRow; row < rowCount && row < map.numberOfRows; row += 1) {
            for (col = startCol; col < colCount && col < map.numberOfColumns; col += 1) {
                tilePositionY = map.getTileHeightInPixels() * row;
                tilePositionX = map.getTileWidthInPixels() * col;
                
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

                for (i = 0; i < unitObjects.length; i += 1) {
                    if (unitObjects[i].getTilePosition().row === row && 
                        unitObjects[i].getTilePosition().col === col) {
                        mainContext.drawImage(
                            unitObjects[i].getIcon(),
                            tilePositionX,
                            tilePositionY,
                            map.getTileWidthInPixels(),
                            map.getTileHeightInPixels());
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
            var i, j, settlement, mycastle, mykingdom;
            
            if (data.status === 'success') {
                player.username = data.username;
                mykingdom = createKingdom({
                    player : player
                });
                kingdoms.push(mykingdom);
                
                // TODO: we need to also grab the map data from the server.
                // but for now we are going to cheat and use a pre-defined
                // map data file
                map = createMap(tmp_data);
                if (gamescreen.sections === undefined || gamescreen === null) {
                    resizeScreen({
                        width : document.body.clientWidth,
                        height : document.body.clientHeight
                    });
                }
                
                for (i = 0; i < data.settlements.length; i += 1) {
                    settlement = createSettlement({
                        imgsrc : data.settlements[i].imagesource,
                        imagekey : data.settlements[i].imagekey,
                        containingTiles : data.settlements[i].containingTiles,
                        
                        cityname : data.settlements[i].Name,
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
                     console.log(data.owned_settlements[i]);
                     settlement = createSettlement({
                        imgsrc : data.owned_settlements[i].imagesource,
                        imagekey : data.owned_settlements[i].imagekey,
                        containingTiles : data.owned_settlements[i].containingTiles,
                        
                        cityname : data.owned_settlements[i].Name,
                        owner : data.owned_settlements[i].owner,
                        size : data.owned_settlements[i].size,
                        
                        population : data.owned_settlements[i].Population,
                        happiness : data.owned_settlements[i].Happiness,
                        income : data.owned_settlements[i].Income,
                        harvest : data.owned_settlements[i].Harvest,
                        gold : data.owned_settlements[i].Gold,
                        food : data.owned_settlements[i].Food,
                        order : data.owned_settlements[i].Order
                        
                    });
                    
                    if (settlement.owner !== '') {
                        console.log(settlement.Name + ' - ' + settlement.owner + ' @ ');
                        console.log(settlement.containingTiles[0]);
                    }
                    
                    for (j = 0; j < 4; j += 1) {
                        map.getTileAt(settlement.containingTiles[j].row, settlement.containingTiles[j].col).setSettlement(settlement);
                    }
                    owned_settlements.push(settlement);
                    
                    // is this mine?
                    if (settlement.ownedBy() === player.username) {
                        mycastle = settlement;
                        mykingdom.addHoldfast(mycastle);
                        // we want to set up the scroll values so that the user sees their castle immediately
                        // -- this involves the scroll.x and scroll.y -- look at the keyboard handler to see how panning works
                        // what we want to do is to center the home castle on the screen
                        // but we also gotta check for the edge cases, we don't want their viewport way outside the
                        // map boundaries
                        (function () {
                            var my_x = mycastle.containedInTiles()[0].col,
                                my_y = mycastle.containedInTiles()[0].row,
                                i, section;
                            
                            jQuery('#log').prepend('<li>Your castle [' + mycastle.getName() + '] is at (' + my_y + ',' + my_x + ')</li>');
                            
                            for (i = 0; i < gamescreen.mapsections.length; i += 1) {
                                section = gamescreen.mapsections[i];
                                if (my_x >= section.mincol && my_x < section.maxcol &&
                                    my_y >= section.minrow && my_y < section.maxrow) {
                                    scroll.x = section.mincol * map.getTileWidthInPixels();
                                    scroll.y = section.minrow * map.getTileHeightInPixels();
                                    return;
                                }
                            }
                        }());
                    }
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
            var div;
            jQuery('#stl_owner').html(obj.ownedBy());
            jQuery('#stl_size').html(obj.getSize());
            jQuery('#stl_population').html(obj.getPopulation());
            jQuery('#stl_order').html(obj.getOrder());
            jQuery('#stl_happiness').html(obj.getHappiness());
            jQuery('#stl_food').html(obj.getFood());
            jQuery('#stl_gold').html(obj.getGold());
            jQuery('#stl_harvest').html(obj.getHarvest());
            jQuery('#stl_income').html(obj.getIncome());
            console.log(obj.ownedBy() + ' ~ ' + player.username);
            if (obj.ownedBy() === player.username) {
                jQuery('#menu_top').menu({
                    select : function (event, ui) {
                        var context = ui.item.find('a:first').html().toLowerCase();
                        
                        if (context === 'army' || context === 'diplomacy' || context === 'city') {
                            jQuery('#menu_top').hide({
                                effect : 'slide',
                                complete : function () {
                                    jQuery('#menu_'+context).show({
                                        effect : 'slide'
                                    });
                                }
                            });
                            
                            jQuery('#settlement_details').hide();
                            jQuery('#' + context + '_details').show();
                        }
                        else if (context === 'details') {
                            jQuery('#menu_selected_content').children().hide();
                            jQuery('#settlement_details').show();
                        }
                    }
                });
                jQuery('#menu_army').menu({
                    select : function (event, ui) {
                        var context = ui.item.find('a:first').html();
                        
                        if (context === 'Back') {
                            jQuery('#menu_army').hide({
                                effect : 'slide',
                                complete : function () {
                                    jQuery('#menu_top').show('slide');
                                }
                            });
                        }
                        
                        if (context === 'Recruit') {
                            (function () {
                                var army, i, kingdom;
                                    for (i = 0; i < kingdoms.length; i += 1) {
                                        if (kingdoms[i].getPlayer() === player) {
                                            kingdom = kingdoms[i];
                                            break;
                                        }
                                    }
                                
                                army = createArmy({
                                    kingdom : kingdom
                                });
                                
                                army.setIcon(resources.getImageObject('ico_fighter'));
                                army.setTilePosition(obj.containedInTiles()[0]);
                                
                                unitObjects.push(army);
                            }());
                        }
                    }
                });
                jQuery('#menu_diplomacy').menu({
                    select : function (event, ui) {
                        var context = ui.item.find('a:first').html();
                        
                        if (context === 'Back') {
                            jQuery('#menu_diplomacy').hide({
                                effect : 'slide',
                                complete : function () {
                                    jQuery('#menu_top').show('slide');
                                }
                            });
                        }
                    }
                });
                jQuery('#menu_city').menu({
                    select : function (event, ui) {
                        var context = ui.item.find('a:first').html();
                        
                        if (context === 'Back') {
                            jQuery('#menu_city').hide({
                                effect : 'slide',
                                complete : function () {
                                    jQuery('#menu_top').show('slide');
                                }
                            });
                        }
                    }
                });
                
                jQuery('#menu')
                    .dialog({
                        title : obj.getName(),
                        width : 600,
                        resizable : false
                    });
            }
            else {
                jQuery('#infoscreen').children().remove();
                jQuery('#settlement_details').clone().show().appendTo('#infoscreen');
                
                jQuery('#infoscreen').dialog({
                    title : obj.getName()
                });
            }
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
	        var absrow = Math.floor(scroll.y / map.getTileHeightInPixels()),
	            relrow = absrow + Math.floor(event.clientY / map.getTileHeightInPixels()),
	        
	            abscol = Math.floor(scroll.x / map.getTileWidthInPixels()),
	            relcol = abscol + Math.floor(event.clientX / map.getTileWidthInPixels()),
	            
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
        },
        
        handleMouseMove : function (event) {
            var absrow = Math.floor(scroll.y / map.getTileHeightInPixels()),
	            relrow = absrow + Math.floor(event.clientY / map.getTileHeightInPixels()),
	        
	            abscol = Math.floor(scroll.x / map.getTileWidthInPixels()),
	            relcol = abscol + Math.floor(event.clientX / map.getTileWidthInPixels());
	            
	            jQuery('#mousex').html(relcol);
	            jQuery('#mousey').html(relrow);
	            
	            if (mouse.down) {
	            
	                if (Math.abs(mouse.y-relrow) > Math.abs(mouse.x-relcol)) {
	                    if (mouse.y < relrow) {
	                        if (scroll.y - map.getTileHeightInPixels() >= 0) {
                                scroll.y -= map.getTileHeightInPixels();
                            }
	                    }
	                    
	                    if (mouse.y > relrow) {
	                        if (scroll.y + map.getTileHeightInPixels() <= 
                                map.getHeightInPixels() - gamescreen.height) {
                                
                                scroll.y += map.getTileHeightInPixels();
                            }
	                    }
                    } 
                    else if (Math.abs(mouse.y-relrow) < Math.abs(mouse.x-relcol)) {
	                    if (mouse.x < relcol) {
	                        if (scroll.x - map.getTileWidthInPixels() >= 0) {
                                scroll.x -= map.getTileWidthInPixels();
                            }
	                    }
	                    
	                    if (mouse.x > relcol) {
	                        if (scroll.x + map.getTileWidthInPixels() <=
                                map.getWidthInPixels() - gamescreen.width) {
                                
                                scroll.x += map.getTileWidthInPixels();   
                            }
	                    }
                    }
	                
	                mouse.x = relcol;
	                mouse.y = relrow;
	            }
        },
        
        handleMouseDown : function (event) {
            mouse.down = true;
        },
        
        handleMouseUp : function (event) {
            mouse.down = false;
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
            jQuery("#info_player").html(data.username);
        
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
        var clientWidth = document.body.clientWidth,
            clientHeight = document.body.clientHeight;
            
        gameClient.screenResize({
            width: document.body.clientWidth,
            height: document.body.clientHeight
        });
        
        jQuery('#tools_container').css({
            height : clientHeight * 0.2
        });
        
        jQuery('#log_container').css({
            height : clientHeight - parseInt(jQuery('#info_panel').css('height')) - parseInt(jQuery('#tools_container').css('height'))
        });
    });
    
    jQuery('#gamePage').mousemove(function (event) {
        gameClient.handleMouseMove(event);
    });
    
    jQuery('#gamePage').mousedown(function (event) {
        gameClient.handleMouseDown(event);
    });
    
    jQuery('#gamePage').mouseup(function (event) {
        gameClient.handleMouseUp(event);
    });
    
    (function() {
        var clientWidth = document.body.clientWidth,
            clientHeight = document.body.clientHeight;
            
        gameClient.screenResize({
            width: document.body.clientWidth,
            height: document.body.clientHeight
        });
        
        jQuery('#tools_container').css({
            height : clientHeight * 0.2
        });
        
        jQuery('#log_container').css({
            height : clientHeight - parseInt(jQuery('#info_panel').css('height')) - parseInt(jQuery('#tools_container').css('height'))
        });
    }());
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
