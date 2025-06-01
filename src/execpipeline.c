#include "../include/execpipeline.h"



// VARIANT HANDLERS

typedef void (*PipelineVariantHandler)(const PipelineVariant* variant, PipelineStack* stack, const PipelineVariablesSlice* variables);

static void handleConstant(const PipelineVariant* variant, PipelineStack* stack, const PipelineVariablesSlice* _v){
	pushStack(stack, variant->asConstant);
}
static void handleVariableIndex(const PipelineVariant* variant, PipelineStack* stack, const PipelineVariablesSlice* variables){
	PipelineVariable variable = variables->vars[variant->asVariableIndex];
	pushStack(stack, variable.value);
}
static void handleOperation(const PipelineVariant* variant, PipelineStack *stack, const PipelineVariablesSlice* _v){
	PipelineOperation op = variant->asOperation;
	ValueType result = op(stack);
	pushStack(stack, result);
}
void handleNone(const PipelineVariant* variant, PipelineStack* _s, const PipelineVariablesSlice* _v){}

// CONSTANT
// VARIABLE_INDEX
// OPERATION
// NONE

static const PipelineVariantHandler variantHandlers[] = {
	handleConstant,
	handleVariableIndex,
	handleOperation,
	handleNone
};

static void handleConstantUnchecked(const PipelineVariant* variant, PipelineStack* stack, const PipelineVariablesSlice* _v){
	pushStackUnchecked(stack, variant->asConstant);
}
static void handleVariableIndexUnchecked(const PipelineVariant* variant, PipelineStack* stack, const PipelineVariablesSlice* variables){
	PipelineVariable variable = variables->vars[variant->asVariableIndex];
	pushStackUnchecked(stack, variable.value);
}
static void handleOperationUnchecked(const PipelineVariant* variant, PipelineStack *stack, const PipelineVariablesSlice* _v){
	PipelineOperation op = variant->asOperation;
	ValueType result = op(stack);
	pushStackUnchecked(stack, result);
}


static const PipelineVariantHandler variantHandlersUnchecked[] = {
	handleConstantUnchecked,
	handleVariableIndexUnchecked,
	handleOperationUnchecked,
	handleNone
};

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
		++stack->index;
		stack->entries[stack->index] = value;
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

uint8_t capacityOfStack(const PipelineStack *stack)
{
	return PIPELINE_STACK_SIZE;
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

void initPipeline(Pipeline* pipeline){
	clearPipeline(pipeline);
}

void pushPipeline(Pipeline* pipeline, PipelineVariant value){
	if(pipeline->index == NONE_INDEX){
		pipeline->index = 0;
		pipeline->entries[0] = value;
	}
	else if( PIPELINE_SIZE <= (pipeline->index + 1)) {
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

uint8_t capacityOfPipeline(const Pipeline *pipeline)
{
	return PIPELINE_SIZE;
}

ValueType executePipeline(const Pipeline *pipeline, PipelineStack *stack, PipelineVariablesSlice variables, bool clearStackOnExecution){
	if(clearStackOnExecution){
		clearStack(stack);
	}
	if(pipeline->index == NONE_INDEX){
		return MISSING_VALUE;
	}
	uint8_t pipelineLength = pipeline->index + 1;
	for(Index pipelineIdx = 0; pipelineIdx < pipelineLength; pipelineIdx++){
		const PipelineVariant* variant = &pipeline->entries[pipelineIdx];
		const PipelineVariantType variantType = variant->type;
		if(variantType > NONE){
			return MISSING_VALUE;
		}
		const PipelineVariantHandler handler = variantHandlers[variantType];
		handler(variant, stack, &variables);
	}

	return popStack(stack);
}
ValueType executePipelineUnchecked(const Pipeline *pipeline, PipelineStack *stack, PipelineVariablesSlice variables){

	clearStack(stack);
	uint8_t pipelineLength = pipeline->index + 1;
	for(Index pipelineIdx = 0; pipelineIdx < pipelineLength; pipelineIdx++){
		const PipelineVariant* variant = &pipeline->entries[pipelineIdx];
		const PipelineVariantHandler handler = variantHandlersUnchecked[variant->type];
		handler(variant, stack, &variables);
	}

	return popStack(stack);
}
