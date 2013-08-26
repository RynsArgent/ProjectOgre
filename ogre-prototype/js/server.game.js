var game = function () {
    var terrain_dicitonary,
    
        createTile, createMap, createPlayer,
        
        players = [];
        
    terrain_dictionary = (function () {
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
            getImagePath : function (key) {
                return dict[key].image;
            },
            
            getLabelFor : function (key) {
                return dict[key].label;
            }
        };
    }());
    /////////////////////////////////////////////////////////
    createTile = function (params) {
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
    /////////////////////////////////////////////////////////
    createMap = function (data) {
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
    /////////////////////////////////////////////////////////
    createPlayer = function (data) {
        var id, name, ip;
        
        ip = [];
        
        return {
            getPlayerID : function () {
                return id;
            },
            getPlayerName : function () {
                return name;
            }
        };
    };
};


var game_server = module.exports = (function () {
    // Declare private variables and methods
    var onPlayerConnect,
        onPlayerDisconnect,
        tick,
        update,
        players = {};
        
    // define privileged functions
    return {
        onPlayerConnect : function (client, callback) {
            console.log( "\t :: game_server.onPlayerConnect :: " + client.userid );
            console.log( "\t\t" + client.handshake.address.address + " : " + client.handshake.address.port );
            var address = client.handshake.address;
            
            if (players[client.userid] !== undefined) {
                players[client.userid] = client;
            }
            callback({userid : client.userid, userip : address.address});
        },

        onPlayerDisconnect : function (client, data) {
            console.log( "\t :: game_server.onPlayerDisconnect :: " + client.userid );
        },

        onRequestLogin : function (client, callback) {
            if (players[client.userid] !== undefined) {
                players[client.userid] = client;
            }
            callback({
                status : "success", 
                msg : client.userid + " logged in as " + client.username + " from " + client.handshake.address.address,
                username : client.username
            });
        }
    };
}());
