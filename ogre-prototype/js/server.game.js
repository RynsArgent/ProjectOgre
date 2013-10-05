var mapdata = require('../resources/tmp.map.1.js');

var game = function () {
    var terrain_dicitonary,
    
        createTile, createMap, createPlayer, createSettlement,
        
        players = [],
        map,
        settlements = [],
        owned_settlements = [];
        
    tmp_random_names = ['Abasyn','Adelhyde','Arngrym','Alorika','Anaheim','Belorne','Bastok','Barduch','Boernisia','Balnor','Camlin','Coerthas','Canthor','Corinth',
                        'Dagohba','Dalborym','Durahan','Dawnstar','Danburen','Dusken','Duskdale','Den of Thieves','Dan','Esterheim','Edelweiss','Elsinore','Eclidius',
                        'Estheim','Eastvale','Eden','Fallbrook','Faust','Foghaven','Fogvale','Fafhalla','Felheim','Fairhaven','Fairbrook','Gridania',"San'doria",'Limsa Lominsa',
                        "Ul'dah",'Windhurst','Figaro','Garden Grove','Vanaheim','Asgard','Midgard','Alfheim','Niflheim','Muspelm','Musplheim','Jotunheim','Ishgard','Isengarde',
                        'Daltigoth','Deregoth','Guntz','Mysidia','Eblan','Goldshire','Darkshire','Neverwinter','Dragon Gate','Lion Gate','Wolf Gate',"Val'hal",'Stormwind',
                        'Nordrassil','Yggdrassil','Teldrassil'];
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
            bgImage, stImage,
            objects = [],
            settlement;
            
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
            },
            getObjects : function () {
                return objects;
            },
            addObject : function (obj) {
                objects.push(obj);
            },
            setSettlement : function (st) {
                settlement = st;
            },
            getSettlement : function () {
                return settlement;
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
                            bgImage : terrain_dictionary.getImagePath(k)
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
                console.log(row + ' , ' + col);
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
    createSettlement = function (data) {
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
            
            owner = null;
            
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
                Name : cityname,
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
    /////////////////////////////////////////////////////////
    createPlayer = function (data) {
        var id = data.id, 
            playername = data.playername, 
            ip = [];
        
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
    
    
    return {
        // This is where a game instance is initialized
        // the data passed in would probably be the map
        // data file or the location to the map.
        initialize : function (params) {
            map = createMap(params.mapdata);
            this.seedSettlements(Math.floor(Math.random() * tmp_random_names.length));
        },
        
        seedSettlements : function (seed) {
            var i, node, t, cityname,
                usedTiles = [], 
                usedNames = [], 
                rndNames = [];
                t = {row:Math.floor(Math.random() * (map.numberOfRows - 4)), col:Math.floor(Math.random() * (map.numberOfColumns - 4))};

            for (i = 0; i < tmp_random_names.length; i += 1) {
                rndNames.push(tmp_random_names[i]);
            }
            
            console.log('seeding settlements');
            for (i = 0; i < seed; i += 1) {
                do {
                    t = {row:Math.floor(Math.random() * (map.numberOfRows - 4)), col:Math.floor(Math.random() * (map.numberOfColumns - 4))};
                } while (usedTiles.indexOf(t) >= 0);
                
                cityname = rndNames[ Math.floor(Math.random() * rndNames.length) ];
                rndNames.splice(rndNames.indexOf(cityname), 1);
                usedNames.push(cityname);
                
                node = createSettlement({
                    imgsrc : 'images/CastleRed.png',
                    imagekey : 'CastleRed',
                    size : 'medium',
                    cityname : cityname,
                    containingTiles : [t, {row:t.row+1, col:t.col}, {row:t.row, col:t.col+1}, {row:t.row+1, col:t.col+1}]
                });
                
                map.getTileAt(t.row, t.col).setSettlement(node);
                map.getTileAt(t.row+1, t.col).setSettlement(node);
                map.getTileAt(t.row, t.col+1).setSettlement(node);
                map.getTileAt(t.row+1, t.col+1).setSettlement(node);
                
                usedTiles.push(t);
                usedTiles.push({row:t.row+1, col:t.col});
                usedTiles.push({row:t.row, col:t.col+1});
                usedTiles.push({row:t.row+1, col:t.col+1});
                
                settlements.push(node);
            }
            console.log(' Settlements established : ' + settlements.length);
        },
        
        addPlayer : function (player) {
            // We need to invoke createPlayer here
            // before we push the resulting created player onto the
            // array of players. We could probably assign 
            // a random settlement here too?
            var i, settlement;
            
            if (players.indexOf(player) < 0) {
                //players.push(player);
                players.push(createPlayer({
                    id : player.userid,
                    playername : player.username
                }));
                
                i = Math.floor(Math.random() * settlements.length);
                settlement = settlements[i];
                settlement.setOwner(player.username);
                settlements.splice(i, 1);
                owned_settlements.push(settlement);
                
            }
        },
        
        removePlayer : function (player) {
            var idx;// = players.indexOf(player);
            
            for (idx = 0; idx < players.length; idx += 1) {
                if (players[idx].getPlayerId() === player.userid) {
                    players.splice(idx, 1);
                }
            }
        },
        
        getSettlements : function () {
            return settlements;
        },
        
        getOwnedSettlements : function () {
            return owned_settlements;
        },
        
        // these require building up an object to pass
        // to the client
        getSettlementsForClient : function () {
            var ss = [], i;
            for (i = 0; i < settlements.length; i += 1) {
                ss.push({
                    Name : settlements[i].getName(),
                    owner : settlements[i].ownedBy(),
                    size : settlements[i].getSize(),
                    imagesource : settlements[i].getImageSource(),
                    imagekey : settlements[i].getImageKey(),
                    Population : settlements[i].getPopulation(),
                    Happiness : settlements[i].getHappiness(),
                    Income : settlements[i].getIncome(),
                    Harvest : settlements[i].getHarvest(),
                    Gold : settlements[i].getGold(),
                    Food : settlements[i].getFood(),
                    Order : settlements[i].getOrder(),
                    containingTiles : settlements[i].containedInTiles()
                });
            }
            
            return ss;
        },
        
        getOwnedSettlementsForClient : function () {
            var ss = [], i;
            for (i = 0; i < owned_settlements.length; i += 1) {
                ss.push({
                    Name : owned_settlements[i].getName(),
                    owner : owned_settlements[i].ownedBy(),
                    size : owned_settlements[i].getSize(),
                    imagesource : owned_settlements[i].getImageSource(),
                    imagekey : owned_settlements[i].getImageKey(),
                    Population : owned_settlements[i].getPopulation(),
                    Happiness : owned_settlements[i].getHappiness(),
                    Income : owned_settlements[i].getIncome(),
                    Harvest : owned_settlements[i].getHarvest(),
                    Gold : owned_settlements[i].getGold(),
                    Food : owned_settlements[i].getFood(),
                    Order : owned_settlements[i].getOrder(),
                    containingTiles : owned_settlements[i].containedInTiles()
                });
            }
            
            return ss;
        }
    };
};  // end game


var game_server = module.exports = (function () {
    // Declare private variables and methods
    var onPlayerConnect,
        onPlayerDisconnect,
        tick,
        update,
        games = [],
        players = {};
    
    // initialize a game
    
    games[0] = game();
    games[0].initialize({mapdata:mapdata});
        
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
            var i;
            
            for (i = 0; i < players.length; i += 1) {
                if (players[i] === client) {
                    players.splice(i, 1);
                    
                    games.removePlayer(client);
                    return;
                }
            }
        },

        onRequestLogin : function (client, callback) {
            if (players[client.userid] !== undefined) {
                players[client.userid] = client;
            }
            
            games[0].addPlayer(client);
            
            callback({
                status : "success", 
                msg : client.userid + " logged in as " + client.username + " from " + client.handshake.address.address,
                username : client.username,
                settlements : games[0].getSettlementsForClient(),
                owned_settlements : games[0].getOwnedSettlementsForClient()
            });
        }
    };
}());
