var createMap = module.exports = function (data) {
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

