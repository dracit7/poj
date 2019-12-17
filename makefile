
.PHONY: all 1182 1417 3321 1990 3294 3415 1470 1986 2777 test

PROBLEMS = 1182 1417 3321 1990 3294 3415 1470 1986 2777

all: $(PROBLEMS)

1182: src/1182.c
	@gcc src/1182.c -o bin/1182

1417: src/1417.c
	@gcc src/1417.c -o bin/1417

3321: src/3321.cc
	@g++ src/3321.cc -o bin/3321
	
1990: src/1990.c
	@gcc src/1990.c -o bin/1990

3294: src/3294.c
	@gcc src/3294.c -o bin/3294

3415: src/3415.c
	@gcc src/3415.c -o bin/3415

1470: src/1470.cc
	@g++ src/1470.cc -o bin/1470

1986: src/1986.cc
	@g++ src/1986.cc -o bin/1986

2777: src/2777.c
	@gcc src/2777.c -o bin/2777

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
	@echo [TEST] Testing poj 1990...
	@echo [testcase 1]
	@bin/1990 < test/1990/tc1
	@echo [TEST] Testing poj 3294...
	@echo [testcase 1]
	@bin/3294 < test/3294/tc1
	@echo [TEST] Testing poj 3415...
	@echo [testcase 1]
	@bin/3415 < test/3415/tc1
	@echo [TEST] Testing poj 1470...
	@echo [testcase 1]
	@bin/1470 < test/1470/tc1
	@echo [TEST] Testing poj 1986...
	@echo [testcase 1]
	@bin/1986 < test/1986/tc1
	@echo [TEST] Testing poj 2777...
	@echo [testcase 1]
	@bin/2777 < test/2777/tc1