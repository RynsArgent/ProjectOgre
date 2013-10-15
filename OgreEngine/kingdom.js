var createKingdom = module.exports = function (data) {
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
        
    };
};
