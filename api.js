
var modeler = require('modeler-redis')
 	, client = require('redis').createClient()
	, idgen = require('idgen')
	, bcrypt = require('bcrypt')
;

// entity model
var user = modeler({
	client: client // must pass a node_redis client here
	,name: 'user' // keys will be prefixed by the collection name
	,prefix: 'OGRE_' // optional root prefix for keys
});

module.exports.api = {
	createUser: function(data, cb) {
		bcrypt.hash(data.pass, 5, function( err, cPass) {
			var me = user.create({
				id:data.email
				,pass:cPass
				,alias:data.alias
			});

			user.save(me, function(err){
				cb(me);
			});
		});
	}

	, getUser: function(data, cb) {
		user.load(data.email, function(err, me){
			bcrypt.compare(data.pass, me.pass, function(err, doesMatch){
				if (doesMatch){
					cb(me);
				}else{
					cb('bad pw');
				}
			 });
		});
	}
};


