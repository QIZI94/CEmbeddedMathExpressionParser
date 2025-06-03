#include "../include/execpipeline.h"

// PIPELINE STACK

void clearStack(PipelineStack* stack){
	stack->index = NONE_INDEX;
	stack->errorMask = NO_ERROR;
}

void initStack(PipelineStack* stack){
	clearStack(stack);
}

void pushStack(PipelineStack* stack, ValueType value){
	if(stack->index == NONE_INDEX){
		stack->index = 0;
		stack->entries[0] = value;
	}
	else if( PIPELINE_STACK_SIZE <= (stack->index + 1)) {
		stack->errorMask |= OVERFLOW;
	}
	else {
		stack->entries[++stack->index] = value;
	}
	
}

ValueType popStack(PipelineStack* stack){
	if(stack->index == NONE_INDEX){
		stack->errorMask |= POP_ON_EMPTY;
		return MISSING_VALUE;
	}
	else if(stack->index == 0){
		stack->index = NONE_INDEX;
		return stack->entries[0];
	}
	
	return stack->entries[stack->index--];
}

void pushStackUnchecked(PipelineStack* stack, ValueType value){
	stack->entries[++stack->index] = value;
}

ValueType popStackUnchecked(PipelineStack* stack){
	return stack->entries[stack->index--];
}

const ValueType* getPtrFromStackIndex(const PipelineStack* stack, Index index){
	if(stack->index == NONE_INDEX){
		return NULL;
	}

	return &stack->entries[stack->index];	
}

uint8_t lengthOfStack(const PipelineStack* stack){
	return stack->index + 1;
}


// PIPELINE VARIANT

PipelineVariant makeStepAsConstant(ValueType value)
{
    PipelineVariant result = {
        .type = CONSTANT,
        {.asConstant = value}
    };
    return result;
    
}

PipelineVariant makeStepAsVariableIndex(Index value){
    PipelineVariant result = {
        .type = VARIABLE_INDEX,
        {.asVariableIndex = value}
    };
    return result;
}

PipelineVariant makeStepAsOperation(PipelineOperation value){
    PipelineVariant result = {
        .type = OPERATION,
        {.asOperation = value}
    };
    return result;
}

PipelineVariant makeNone(){
	PipelineVariant none = {.type = NONE};
	return none;
}

// PIPELINE

void clearPipeline(Pipeline* pipeline){
	pipeline->index = NONE_INDEX;
	pipeline->errorMask = NO_ERROR;
}

Pipeline createPipeline(PipelineVariant storageLink[], uint8_t storageCapacity){
	return (Pipeline){
		.index = NONE_INDEX,
		.capacity = storageCapacity,
		.errorMask = NO_ERROR,
		.entries = storageLink
		
	};
}

void pushPipeline(Pipeline* pipeline, PipelineVariant value){
	if(pipeline->index == NONE_INDEX){
		pipeline->index = 0;
		pipeline->entries[0] = value;
	}
	else if( pipeline->capacity <= (pipeline->index + 1)) {
		pipeline->errorMask |= OVERFLOW;
	}
	else {
		++pipeline->index;
		pipeline->entries[pipeline->index] = value;
	}
	
}

const PipelineVariant* getPtrFromPipelineIndex(const Pipeline* pipeline, Index index){
	if(pipeline->index == NONE_INDEX){
		return NULL;
	}

	return &pipeline->entries[pipeline->index];	
}

uint8_t lengthOfPipeline(const Pipeline *pipeline){
	return pipeline->index + 1;;
}

ValueType executePipeline(const Pipeline *pipeline, PipelineStack *stack, PipelineVariablesSlice variables, bool clearStackOnExecution){
	if(clearStackOnExecution){
		clearStack(stack);
	}
	if(pipeline->index == NONE_INDEX){
		return MISSING_VALUE;
	}
	ValueType right;
	ValueType left;
	
	ValueType* stackStorage = stack->entries;
	Index* stackIndex = &stack->index;
	
	uint8_t pipelineLength = pipeline->index + 1;
	for(Index pipelineIdx = 0; pipelineIdx < pipelineLength; pipelineIdx++){
		const PipelineVariant* variant = &pipeline->entries[pipelineIdx];
		switch (variant->type)
		{
						case OPERATION_NATIVE_ADD:
				{
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left + right;
				}
				break;
			case OPERATION_NATIVE_SUB:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left - right;
				}
				break;
			case OPERATION_NATIVE_MUL:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left * right;
				}
				break;
			case OPERATION_NATIVE_DIV:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left / right;
				}
				break;
			case OPERATION_NATIVE_MOD:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left % right;
				}
				break;
			
			case CONSTANT:
				{
					ValueType constant = variant->asConstant;
					pushStack(stack, constant);
				}
				break;
			case VARIABLE_INDEX:
				{
					VariableIndex variableIndex = variant->asVariableIndex;
					pushStack(stack, variables.vars[variant->asVariableIndex].value);
				}
				
				break;
			case OPERATION:
				{
					PipelineOperation op = variant->asOperation;
					ValueType result = op(stack);
					pushStack(stack, result);
				}
				break;
			case NONE:
				return MISSING_VALUE;
		}

	}

	return popStack(stack);
}
ValueType executePipelineUnchecked(const Pipeline *pipeline, PipelineStack *stack, PipelineVariablesSlice variables){

	clearStack(stack);
	ValueType right;
	ValueType left;
	
	ValueType* stackStorage = stack->entries;
	Index* stackIndex = &stack->index;
	
	uint8_t pipelineLength = pipeline->index + 1;
	for(Index pipelineIdx = 0; pipelineIdx < pipelineLength; pipelineIdx++){
		const PipelineVariant* variant = &pipeline->entries[pipelineIdx];
		switch (variant->type)
		{
			case OPERATION_NATIVE_ADD:
				{
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left + right;
				}
				break;
			case OPERATION_NATIVE_SUB:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left - right;
				}
				break;
			case OPERATION_NATIVE_MUL:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left * right;
				}
				break;
			case OPERATION_NATIVE_DIV:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left / right;
				}
				break;
			case OPERATION_NATIVE_MOD:
				{						
					--(*stackIndex);
					left = stackStorage[(*stackIndex)];
					right = stackStorage[(*stackIndex)] = left % right;
				}
				break;
			case CONSTANT:
				{	
					right = variant->asConstant;
					pushStackUnchecked(stack, right);
				}
				break;
			case VARIABLE_INDEX:
				{
					VariableIndex variableIndex = variant->asVariableIndex;
					right = variables.vars[variant->asVariableIndex].value;
					pushStackUnchecked(stack, right);
				}
				break;
			case OPERATION:
				{
					PipelineOperation op = variant->asOperation;
					ValueType result = op(stack);
					pushStackUnchecked(stack, result);
				}
				break;
			case NONE:
				return MISSING_VALUE;
		}
	}

	return popStackUnchecked(stack);
}
