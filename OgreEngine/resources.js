var resources = module.exports = (function () {
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
    };
}());
