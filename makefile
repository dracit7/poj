
.PHONY: all 1182 1417 3321 test

PROBLEMS = 1182 1417 3321

all: $(PROBLEMS)

1182: src/1182.c
	@gcc src/1182.c -o bin/1182

1417: src/1417.c
	@gcc src/1417.c -o bin/1417

3321: src/3321.cc
	@g++ src/3321.cc -o bin/3321
	
test: $(PROBLEMS)
	@echo [TEST] Testing poj 1182...
	@echo [testcase 1]
	@bin/1182 < test/1182/tc1
	@echo [TEST] Testing poj 1417...
	@echo [testcase 1]
	@bin/1417 < test/1417/tc1
	@echo [TEST] Testing poj 3321...
	@echo [testcase 1]
	@bin/3321 < test/3321/tc1