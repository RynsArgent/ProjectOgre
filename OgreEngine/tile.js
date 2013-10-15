var createTile = module.exports = function (params) {
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
