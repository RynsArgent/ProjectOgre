
/*
var	modeler = require('modeler-redis')
,	client = require('redis').createClient();
*/
//var io = require('socket.io-client');


/*
var	modeler = require('modeler-redis')
,	client = require('redis').createClient();
*/

var modeler = require('modeler');
var client = null;

var api = new (require('../api').api)(modeler, client);

require('chai').should();

describe('api tests /', function(){

	it('testCreateUser', function(done)
	{
		api.should.have.property('createUser');
		api.createUser.should.be.a('Function');

		api.createUser({
			email:'tesract@gmail.com', 
			pass:'wanda', 
			alias:'tesract'
		}, function(data) {
			
			data.should.be.a('Object');
			
			data.should.have.property('id');
			data.id.should.equal('tesract@gmail.com');
			
			data.should.have.property('pass');
			//data.pass.should.equal('wanda');
			
			data.should.have.property('alias');
			data.alias.should.equal('tesract');
			
			api.getUser({email:'tesract@gmail.com', pass:'wanda'}, function(data){
				data.should.be.a('Object');
				
				data.should.have.property('id');
				data.id.should.equal('tesract@gmail.com');
				
				data.should.have.property('pass');
				//data.pass.should.equal();
				
				data.should.have.property('alias');
				data.alias.should.equal('tesract');

console.log(data);				

				done();
			});			
		});
	});
});

