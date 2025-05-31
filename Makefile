
test:
	gcc -O2 -g  test.c execpipeline.c pipelinemath.c  expressionparser.c -o test && ./test && rm ./test

test_input:
	echo NotImplemented
