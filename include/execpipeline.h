#ifndef EXECPIPELINE_H
#define EXECPIPELINE_H

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
//#define NULL nullptr;

#define ARRAY_CONST_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#define PIPELINE_STACK_SIZE 50
#define PIPELINE_SIZE 50

typedef uint8_t Index;


typedef int32_t ValueType;

#define NONE_INDEX UINT8_MAX
#define MISSING_VALUE (ValueType)(1)


// PIPELINE STACK
typedef enum StackErrorMask {
	NO_ERROR = 0x00,
	OVERFLOW = 0x01,
	POP_ON_EMPTY = 0x02
	
} StackErrorMask;


typedef struct {
	ValueType entries[PIPELINE_STACK_SIZE];
	Index index;
	uint8_t errorMask;
} PipelineStack;

extern void clearStack(PipelineStack* stack);
extern void initStack(PipelineStack* stack);
extern void pushStack(PipelineStack* stack, ValueType value);
extern ValueType popStack(PipelineStack* stack);
extern const ValueType* getPtrFromStackIndex(const PipelineStack* stack, Index index);

// PIPELINE VARIANT
typedef enum {
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
#define MAKE_SLICE_FROM_CONST_PIPELINE_VARIABLES(vars) (PipelineVariablesSlice){vars, (sizeof(vars)/sizeof(vars[0])) - 1}
 
// PIPELINE

typedef struct {
	PipelineVariant entries[50];
	Index index;
	uint8_t errorMask;
} Pipeline;

extern void clearPipeline(Pipeline* pipeline);
extern void initPipeline(Pipeline* pipeline);
extern void pushPipeline(Pipeline* pipeline, PipelineVariant value);
extern const PipelineVariant* getPtrFromPipelineIndex(const Pipeline* pipeline, Index index);


extern ValueType executePipeline(const Pipeline* pipeline, PipelineStack* stack, PipelineVariablesSlice variables, bool clearStackOnExecution);


#endif