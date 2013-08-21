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

var terrain = (function () {
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
var createTile = function (details) {
    var label, 
        index, row, col, 
        width, height,
        bgImage, stImage;
        
    label = details.label || '';
    row = details.row || -1;
    col = details.col || -1;
    index = details.index || -1;
    width = details.width || 0;
    height = details.height || 0;
    
    if (details.bgImage !== undefined) {
        bgImage = details.bgImage;
    }
    if (details.stImage !== undefined) {
        stImage = details.stImage;
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

var createMap = function (details) {

};

///////////////////////////////////////////////////////////////////////////////

jQuery(document).ready(function () {
    'use strict';
    var gameClient;
    
    jQuery("#enterBtn").click(function(ev) {
        var username = jQuery("#username").val();
        gameClient.requestLogin(username);
    });

    gameClient = (function () {
        // Declare private variables and methods
        var that, player,
            
            tick, update, animate,
            
            handleInput,
            
            socket = io.connect(),
            
            bgCanvas, stCanvas, mainCanvas, fgCanvas;
            
            bgCanvas = jQuery("#bg").get(0);
            stCanvas = jQuery("#st").get(0);
            mainCanvas = jQuery("#main").get(0);
            fgCanvas = jQuery("#fg").get(0);
            
            bgCanvas.width = document.body.clientWidth;
            bgCanvas.height = document.body.clientHeight;
            
            console.log( "bgCanvas width: " + bgCanvas.width );
            
            stCanvas.width = document.body.clientWidth;
            stCanvas.height = document.body.clientHeight;
            
            mainCanvas.width = document.body.clientWidth;
            mainCanvas.height = document.body.clientHeight;
            
            fgCanvas.width = document.body.clientWidth;
            fgCanvas.height = document.body.clientHeight;

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
                tick();
                
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

        // define privileged functions
        return {
            requestLogin : function (username) {
                socket.emit('requestlogin', username);
            }
        };
        
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
