var createArmy = module.exports = function (data) {
    var kingdom = data.kingdom,
        units = [],
        tilePosition,
        pixelPosition,
        icon,
        deployed = false;
        
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
        },
        
        deploy : function (pos) {
            tilePosition = pos;
            deployed = true;
        },
        
        isDeployed : function () {
            return deployed;
        }
    };
};
