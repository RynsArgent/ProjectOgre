
var	  bcrypt = require('bcrypt');

module.exports.api = function(modeler, client)
{
	// entity model
	this.user = modeler({
		client: client // must pass a node_redis client here
		,name: 'user' // keys will be prefixed by the collection name
		,prefix: 'OGRE_' // optional root prefix for keys
	});	
};

module.exports.api.prototype.createUser = function(data, cb) 
{
	var self = this;
	bcrypt.hash(data.pass, 5, function( err, cPass) {
		var me = self.user.create({
			id:data.email
			,pass:cPass
			,alias:data.alias
		});

		self.user.save(me, function(err){
			cb(me);
		});
	});
};

module.exports.api.prototype.getUser= function(data, cb) 
{
	this.user.load(data.email, function(err, me){
		bcrypt.compare(data.pass, me.pass, function(err, doesMatch){
			if (doesMatch){
				cb(me);
			}else{
				cb('bad pw');
			}
		 });
	});
};
