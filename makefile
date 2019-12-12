
.PHONY: all 1182 test

PROBLEMS = 1182

all: $(PROBLEMS)

1182: src/1182.c
	@gcc src/1182.c -o bin/1182
	
test: $(PROBLEMS)
	@echo [TEST] Testing poj 1182...
	@echo [testcase 1]
	@bin/1182 < test/1182/tc1