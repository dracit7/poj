
.PHONY: all 1182 test

PROBLEMS = 1182 1417

all: $(PROBLEMS)

1182: src/1182.c
	@gcc src/1182.c -o bin/1182

1417: src/1417.c
	@gcc src/1417.c -o bin/1417
	
test: $(PROBLEMS)
	@echo [TEST] Testing poj 1182...
	@echo [testcase 1]
	@bin/1182 < test/1182/tc1
	@echo [TEST] Testing poj 1417...
	@echo [testcase 1]
	@bin/1417 < test/1417/tc1