var createSettlement = module.exports = function (data) {
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
        
        owner = data.owner,
        armies = [], units = [];
        
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
        addUnit : function (unit) {
            units.push(unit);
        },
        addArmy : function (army) {
            armies.push(army);
        },
        getArmies : function () {
            return armies;
        },
        getUnits : function () {
            return units;
        }
    };
};
