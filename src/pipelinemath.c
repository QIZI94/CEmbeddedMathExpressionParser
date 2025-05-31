#include "../include/pipelinemath.h"

#include <ctype.h>

// operations 
ValueType opAdd(PipelineStack* stack){
	ValueType right = popStack(stack);
	ValueType left = popStack(stack);

	return left + right;
}

ValueType opSub(PipelineStack* stack){
	ValueType right = popStack(stack);
	ValueType left = popStack(stack);

	return left - right;
}

ValueType opMul(PipelineStack* stack){
	ValueType right = popStack(stack);
	ValueType left = popStack(stack);

	return left * right;
}

ValueType opDiv(PipelineStack* stack){
	ValueType right = popStack(stack);
	ValueType left = popStack(stack);
	return left / right;
}

ValueType opMod(PipelineStack* stack){
	ValueType right = popStack(stack);
	ValueType left = popStack(stack);
	return left % right;
}

ValueType opPow2(PipelineStack* stack){
	ValueType powered = popStack(stack);
	return powered * powered;
}



// operation map

const OperationMapEntry operationMap [] = {
	{opAdd,	{MAKE_SLICE_FROM_CONST_STRING("add"), 2}},
	{opSub, {MAKE_SLICE_FROM_CONST_STRING("sub"), 2}},
	{opMul, {MAKE_SLICE_FROM_CONST_STRING("mul"), 2}},
	{opDiv, {MAKE_SLICE_FROM_CONST_STRING("div"), 2}},
	{opMod, {MAKE_SLICE_FROM_CONST_STRING("mod"), 2}},
	{opPow2, {MAKE_SLICE_FROM_CONST_STRING("pow2"), 0}}
};

PipelineOperation getOperationByName(StringSlice name){

	for(size_t idx = 0;  idx < ARRAY_CONST_SIZE(operationMap); idx++){
		const OperationMapEntry* entry = &operationMap[idx];
		if(isSliceEqual(entry->meta.name, name)){
			return entry->op;
		}
	}
	return NULL;
}

PipelineOperationMeta getMetaByOperation(PipelineOperation op)
{
	for(size_t idx = 0;  idx < ARRAY_CONST_SIZE(operationMap); idx++){
		const OperationMapEntry* entry = &operationMap[idx];
		if(entry->op == op){
			return entry->meta;
		}
	}
	return (PipelineOperationMeta){.name = MAKE_EMPTY_SLICE, .argCount = 0};
}



StringSlice makeSliceFromString(const char* str){
	StringSlice slice = {
		.str = str,
		.len = strlen(str)
	};
	return slice;
}



bool startWith(StringSlice inputSlice, StringSlice reference){
	if(inputSlice.len < reference.len){
		return false;
	}
	
	for(size_t idx = 0; idx < reference.len; idx++){
		if(inputSlice.str[idx] != reference.str[idx]){
			return false;
		}
	}

	return true;
}


bool sliceToInt(StringSlice input, int32_t* output){
	int32_t result = 0;
	int32_t sign = 1;
	size_t idx = 0;

	if(input.str[idx] == '-'){
		sign = -1;
		idx++;
	}
	else if(input.str[idx] == '+'){
		sign = 1;
		idx++;
	}
	for(; idx < input.len; idx++){
		result *= 10;
		char ch = input.str[idx];

		
		if(isdigit(ch)){
			result += ch - '0';
		}
		else {
			return false;
		}
	}
	*output = result * sign;
	return true;
}

bool isSliceEqual(StringSlice left, StringSlice right){
	if(left.len != right.len){
		return false;
	}
	else if(left.str == right.str){
		return true;
	}
	for(size_t idx = 0; idx < left.len; idx++){
		if(left.str[idx] != right.str[idx]){
			return false;
		}
	}
	return true;

}

size_t findInSlice(StringSlice input, StringSlice searched){

	for(size_t inputIdx = 0; inputIdx < input.len; inputIdx++){
		if(inputIdx + searched.len > input.len){
			return input.len;
		}

		StringSlice inputSliceAtIndex = {.str = &input.str[inputIdx], .len = searched.len};
		if(isSliceEqual(inputSliceAtIndex, searched)){
			return inputIdx;
		}
	}

	return input.len;
}


