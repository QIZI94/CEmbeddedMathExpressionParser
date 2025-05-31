#ifndef PIPELINEMATH_H
#define PIPELINEMATH_H

#include "execpipeline.h"
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	const char* str;
	size_t len;
} StringSlice;

typedef struct {
	StringSlice name;
	size_t argCount;
} PipelineOperationMeta;

typedef struct {
	PipelineOperation op;
	PipelineOperationMeta meta;
} OperationMapEntry;


#define MAKE_SLICE_FROM_CONST_STRING(str) ((StringSlice){str, (sizeof(str)/sizeof(str[0])) - 1})
#define MAKE_EMPTY_SLICE ((StringSlice){.str = NULL, .len = 0})
//#define SLICE_TO_STR(slice) (std::string(slice.str, slice.len))

extern StringSlice makeSliceFromString(const char* str);
extern bool sliceToInt(StringSlice input, int32_t* output);
extern bool isSliceEqual(StringSlice left, StringSlice right);
extern size_t findInSlice(StringSlice input, StringSlice searched);


PipelineOperation getOperationByName(StringSlice name);
PipelineOperationMeta getMetaByOperation(PipelineOperation op);


#endif