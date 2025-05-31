
#include <iostream>
#include <inttypes.h>
#include <string.h>

#include "execpipeline.h"
#include "pipelinemath.h"
#include "expressionparser.h"

void printStack(const PipelineStack* stack){
	if(stack->errorMask & StackErrorMask::OVERFLOW){
		std::cout<<"PipelineStack Overflow\n";
	}
	if(stack->errorMask & StackErrorMask::POP_ON_EMPTY){
		std::cout<<"PipelineStack pop attempt on empty pipeline\n";
	}
	if(stack->index == NONE_INDEX){
		std::cout<<"Stack EMPTY\n";
		return;
	}
	Index pointedIndex = stack->index;
	std::cout<<"Stack size: "<<pointedIndex+1<<'\n';
	for(Index index = 0; index <= pointedIndex; index++){
		std::cout<<"["<<uint(index)<<"] = "<< stack->entries[index]<<"\n";
	}
}

void printPipeline(const Pipeline* pipeline){
	if(pipeline->errorMask & StackErrorMask::OVERFLOW){
		std::cout<<"Pipeline Overflow\n";
	}
	if(pipeline->errorMask & StackErrorMask::POP_ON_EMPTY){
		std::cout<<"Pipeline pop attempt on empty pipeline\n";
	}
	if(pipeline->index == NONE_INDEX){
		std::cout<<"Pipeline EMPTY\n";
		return;
	}
	Index pointedIndex = pipeline->index;
	std::cout<<"Pipeline size: "<<pointedIndex+1<<'\n';
	for(Index index = 0; index <= pointedIndex; index++){
		PipelineVariant pipelineEntry = pipeline->entries[index];
		if(pipelineEntry.type == PipelineVariantType::CONSTANT) {
			std::cout<<"["<<uint(index)<<"] Constant = "<< pipelineEntry.asConstant <<"\n";
		}
		else if(pipelineEntry.type == PipelineVariantType::VARIABLE_INDEX) {
			std::cout<<"["<<uint(index)<<"] VariableIndex = "<< size_t(pipelineEntry.asVariableIndex) <<"\n";
		}
		else if(pipelineEntry.type == PipelineVariantType::OPERATION) {
			PipelineOperation op = pipelineEntry.asOperation;
			StringSlice opName = getMetaByOperation(op).name;
			std::cout<<"["<<uint(index)<<"] Operation = "<< std::string(opName.str, opName.len) <<"\n";
		}

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
	std::cout<<uint(err.type)<<"\n";
	switch (err.type)
	{
		case ParsingErrorType::INPUT_EMPTY:
			std::cout<<"Empty formula input\n";
			break;
		case ParsingErrorType::UNEXPECTED:
			std::cout<<"Unexpected: '"<<err.unexpected<<"' at column "<<err.at<<'\n';
			break;
		case ParsingErrorType::UNKNOWN_OPERATION:
			std::cout<<"Unknown operation: '"<<err.unexpected<<"' at column "<<err.at<<'\n';
			break;
		case ParsingErrorType::TOO_LITTLE_ARGUMENTS :
			std::cout<<"Too little arguments: '"<<err.unexpected<<"' at column "<<err.at<<'\n';
			break;
		case ParsingErrorType::TOO_MANY_ARGUMENTS:
			std::cout<<"Too many arguments: '"<<err.unexpected<<"' at column "<<err.at<<'\n';
			break;
		case ParsingErrorType::UNKNOWN_VARIABLE:
			std::cout<<"Unknown variable: '"<<err.unexpected<<"' at column "<<err.at<<'\n';
			break;
		case ParsingErrorType::PIPELINE_FULL:
			std::cout<<"Pipeline full, last visited: '"<<err.unexpected<<"' at column "<<err.at<<'\n';
			break;
		default:break;
	}

	

	printPipeline(&pipeline);
	printStack(&stack);

	ValueType result = executePipeline(&pipeline, &stack, MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars), true);
	std::cout<<"Reuslt: "<< result <<'\n';
}

/*

int main(){
	Pipeline pipeline;
	initPipeline(&pipeline);
	

	

	//printStack(&simulatedStack);
	//printPipeline(&pipeline);
	//const char inputFormula[] = "pow2(add(5,add(mul(x,2),23)))";
	// eq "1+2*pow2(3*2)";
	const char inputFormula[] = "add(1,mul(2,pow2(mul(3,2))))";
	PipelineVariable vars[] = {
		{'x', 10},
		{'y', 15},
		{'z', 25}
	};

	pushPipeline(&pipeline, makeStepAsVariableIndex(1));
	pushPipeline(&pipeline, makeStepAsConstant(10));
	pushPipeline(&pipeline, makeStepAsOperation(getOperationByName(MAKE_SLICE_FROM_CONST_STRING("add"))));
	

	PipelineStack simulatedStack;
	initStack(&simulatedStack);
	ValueType result = executePipeline(&pipeline, &simulatedStack, MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars), true);
	std::cout<<"Reuslt: "<< result <<'\n';
	clearPipeline(&pipeline);
	bool successful = compileFromFormattedFormulaRecursively(
		&pipeline,
		MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars),
		MAKE_SLICE_FROM_CONST_STRING(inputFormula)
	);
	if(!successful){
		std::cout<<"Compilation failed!\n";
	}
	printPipeline(&pipeline);
	ValueType result2 = executePipeline(&pipeline, &simulatedStack, MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars), true);
	std::cout<<"Reuslt2: "<< result2 <<'\n';
	printStack(&simulatedStack);

	ValueType result3 = executePipeline(&pipeline, &simulatedStack, MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars), true);
	std::cout<<"Reuslt3: "<< result3 <<'\n';
}*/

	