#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H
#include <inttypes.h>
#include <stdbool.h>

#include "../include/execpipeline.h"
#include "../include/pipelinemath.h"



#define PARSING_ERROR(err_type, err_at, err_unexp) ((ParsingError){.at=(uint16_t)err_at, .unexpected=err_unexp, .type=err_type})
#define NO_PARSING_ERROR ((ParsingError){.at=0, .unexpected='\0', .type=NOERROR})
typedef enum {
	NOERROR,
	INPUT_EMPTY,
	UNEXPECTED,
	UNKNOWN_OPERATION,
	TOO_MANY_ARGUMENTS,
	TOO_LITTLE_ARGUMENTS,
	UNKNOWN_VARIABLE,
	PIPELINE_FULL
} ParsingErrorType;

typedef struct {
	uint16_t at;
	char unexpected;
	ParsingErrorType type;
	
} ParsingError;




typedef struct {
	StringSlice slice;
	size_t cursor;
} PeekableStringSlice;


extern ParsingError parseConstantVariableOperationToken(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables);
extern ParsingError parseMulDivToken(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables);
extern ParsingError parseAddSubToken(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables);
extern ParsingError compileExpression(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables);



#endif