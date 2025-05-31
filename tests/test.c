#include "../include/expressionparser.h"

#include <stdio.h>

void printStack(const PipelineStack* stack){
	if(stack->errorMask & OVERFLOW){
		printf("PipelineStack Overflow\n");
	}
	if(stack->errorMask & POP_ON_EMPTY){
		printf("PipelineStack pop attempt on empty pipeline\n");
	}
	if(stack->index == NONE_INDEX){
		printf("Stack EMPTY\n");
		return;
	}
	Index pointedIndex = stack->index;
	uint8_t stackSize = pointedIndex+1;
	printf("Stack size: %d\n", stackSize);
	for(Index index = 0; index < stackSize; index++){
		printf("[%ld] = %d\n", (size_t)index, stack->entries[index]);
	}
}

void printPipeline(const Pipeline* pipeline){
	if(pipeline->errorMask & OVERFLOW){
		printf("Pipeline Overflow\n");
	}
	if(pipeline->errorMask & POP_ON_EMPTY){
		printf("Pipeline pop attempt on empty pipeline\n");
	}
	if(pipeline->index == NONE_INDEX){
		printf("Pipeline EMPTY\n");
		return;
	}
	Index pointedIndex = pipeline->index;
	printf("Pipeline size: %d\n", pointedIndex+1);
	for(Index index = 0; index <= pointedIndex; index++){
		PipelineVariant pipelineEntry = pipeline->entries[index];
		if(pipelineEntry.type == CONSTANT) {
			printf("[%d] Constant = %d\n", index, pipelineEntry.asConstant);
		}
		else if(pipelineEntry.type == VARIABLE_INDEX) {
			printf("[%d] VariableIndex = %d\n", index, pipelineEntry.asVariableIndex);
		}
		else if(pipelineEntry.type == OPERATION) {
			PipelineOperation op = pipelineEntry.asOperation;
			StringSlice opName = getMetaByOperation(op).name;

			printf("[%d] Operation = ", index);
			for(size_t charIdx = 0; charIdx < opName.len; charIdx++){
				printf("%c", opName.str[charIdx]);
			}
			printf("\n");
		}
	}
}

void printParsingError(ParsingError err){
		switch (err.type)
	{
		case INPUT_EMPTY:
			printf("Empty formula input\n");
			break;
		case UNEXPECTED:
			printf("Unexpected: '%c' at column %d\n",err.unexpected, err.at);
			break;
		case UNKNOWN_OPERATION:
			printf("Unknown operation: '%c' at column %d\n",err.unexpected, err.at);
			break;
		case TOO_LITTLE_ARGUMENTS:
			printf("Too little arguments: '%c' at column %d\n",err.unexpected, err.at);
			break;
		case TOO_MANY_ARGUMENTS:
			printf("Too many arguments: '%c' at column %d\n",err.unexpected, err.at);
			break;
		case UNKNOWN_VARIABLE:
			printf("Unknown variable: '%c' at column %d\n",err.unexpected, err.at);
			break;
		case PIPELINE_FULL:
			printf("Pipeline full, last visited: '%c' at column %d\n",err.unexpected, err.at);
			break;
		case NO_ERROR:
			printf("No error\n");
			break;
	}

}

int main(){

	Pipeline pipeline;
	initPipeline(&pipeline);
	

	//const char inputFormula[] = "1+-2*(-pow2(3*2))";
	const char inputFormula[] = "30*(y+20)";
	PipelineVariable vars[] = {
		{'x', 10},
		{'y', 15},
		{'z', 25}
	};

	PeekableStringSlice peekableSlice = {
		.slice = MAKE_SLICE_FROM_CONST_STRING(inputFormula),
		.cursor = 0
	};

	PipelineStack stack;
	initStack(&stack);

	ParsingError err = compileExpression(
		&pipeline,
		&peekableSlice,
		MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars)
	);
	if(err.type != NO_ERROR){
		printParsingError(err);
	}
	
	printPipeline(&pipeline);
	printStack(&stack);

	ValueType result = executePipeline(&pipeline, &stack, MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars), true);
	printf("Result %d\n", result);
	
	return 0;
}