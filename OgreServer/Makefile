

TESTS = test/*.js

tests:
	mocha --timeout 5000 --reporter nyan $(TESTS)

cont:
	mocha -w --reporter min --timeout 5000 --check-leaks $(TESTS)


