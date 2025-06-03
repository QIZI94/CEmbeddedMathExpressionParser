#ifndef EXECPIPELINE_H
#define EXECPIPELINE_H

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
//#define NULL nullptr;

#define ARRAY_CONST_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

typedef uint8_t Index;


typedef int32_t ValueType;

#define NONE_INDEX UINT8_MAX
#define MISSING_VALUE (ValueType)(1)


// PIPELINE STACK
#ifndef PIPELINE_STACK_SIZE
#define PIPELINE_STACK_SIZE 50
#endif
typedef enum StackErrorMask {
	NO_ERROR = 0x00,
	OVERFLOW = 0x01,
	POP_ON_EMPTY = 0x02
	
} StackErrorMask;


typedef struct {
	Index index;
	uint8_t errorMask;
	ValueType entries[PIPELINE_STACK_SIZE];
} PipelineStack;

extern void clearStack(PipelineStack* stack);
extern void pushStack(PipelineStack* stack, ValueType value);
extern ValueType popStack(PipelineStack* stack);
void pushStackUnchecked(PipelineStack* stack, ValueType value);
ValueType popStackUnchecked(PipelineStack* stack);

extern const ValueType* getPtrFromStackIndex(const PipelineStack* stack, Index index);
extern uint8_t lengthOfStack(const PipelineStack* stack);


// PIPELINE VARIANT
typedef enum {
	OPERATION_NATIVE_ADD,
	OPERATION_NATIVE_SUB,
	OPERATION_NATIVE_MUL,
	OPERATION_NATIVE_DIV,
	OPERATION_NATIVE_MOD,
    CONSTANT,
    VARIABLE_INDEX,
    OPERATION,
	NONE
} PipelineVariantType;

typedef ValueType (*PipelineOperation)(PipelineStack* stack);
typedef ValueType Constant;
typedef Index VariableIndex;

typedef struct {
    PipelineVariantType type;
    union{
        Constant asConstant;
        VariableIndex asVariableIndex;
        PipelineOperation asOperation;
    };
} PipelineVariant;

extern PipelineVariant makeStepAsConstant(ValueType value);
extern PipelineVariant makeStepAsVariableIndex(Index value);
extern PipelineVariant makeStepAsOperation(PipelineOperation value);
extern PipelineVariant makeNone();

// PIPELINE VARIABLE
typedef struct{
	char name;
	ValueType value;
}PipelineVariable;

typedef struct{
	const PipelineVariable* vars;
	int8_t len;
} PipelineVariablesSlice;
#define MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars) (PipelineVariablesSlice){vars, (sizeof(vars)/sizeof(vars[0]))}
 
// PIPELINE

typedef struct {
	Index index;
	uint8_t capacity;
	uint8_t errorMask;
	PipelineVariant* entries;
} Pipeline;

#define CREATE_PIPELINE_FROM_CONST_STORAGE(storage) ((Pipeline){.index = NO_ERROR, .capacity = ARRAY_CONST_SIZE(storage), .errorMask = NO_ERROR, .entries = storage})

Pipeline createPipeline(PipelineVariant storageLink[], uint8_t storageCapacity);

extern void clearPipeline(Pipeline* pipeline);
extern void pushPipeline(Pipeline* pipeline, PipelineVariant value);
extern const PipelineVariant* getPtrFromPipelineIndex(const Pipeline* pipeline, Index index);
extern uint8_t lengthOfPipeline(const Pipeline* pipeline);

extern ValueType executePipeline(const Pipeline* pipeline, PipelineStack* stack, PipelineVariablesSlice variables, bool clearStackOnExecution);
extern ValueType executePipelineUnchecked(const Pipeline *pipeline, PipelineStack *stack, PipelineVariablesSlice variables);

#endif