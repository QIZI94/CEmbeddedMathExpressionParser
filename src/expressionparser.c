

#include <ctype.h>

#include "../include/expressionparser.h"

// helper static functions

static bool isalphanumeric(char ch){
	return isalpha(ch) || isdigit(ch);
}

static bool isdigitforsign(char ch){
	return isdigit(ch) || ch == '+' || ch == '-';
}

static char peekToken(PeekableStringSlice* input){
	StringSlice slice = input->slice;
	if(input->cursor >= slice.len){
		return '\0';
	}

	while (isspace(slice.str[input->cursor])) input->cursor++;
    return slice.str[input->cursor];
}

static char consumeToken(PeekableStringSlice* input) {
	StringSlice slice = input->slice;
    return slice.str[input->cursor++];
}

static bool matchToken(PeekableStringSlice* input, char expected) {
    if (peekToken(input) == expected) {
        consumeToken(input);
        return true;
    }
    return false;
}


// extern functions
ParsingError parseConstantVariableOperationToken(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables) {
    if (isdigitforsign(peekToken(peekableSlice))) {
		StringSlice constantSlice = {.str = &peekableSlice->slice.str[peekableSlice->cursor], .len = 0};
        while (isdigit(peekToken(peekableSlice))) {
            consumeToken(peekableSlice);
			constantSlice.len++;
        }
		
		ValueType constantValue;
		if(!sliceToInt(constantSlice, &constantValue)){
			// never will be reached due to previous isdigit check
		}
		pushPipeline(pipeline, makeStepAsConstant(constantValue));
		if(pipeline->errorMask & OVERFLOW){
			return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
		}
        
        return NO_PARSING_ERROR;
		
    } else if (isalpha(peekToken(peekableSlice))) {
		StringSlice operationOrVariableSlice = {.str = &peekableSlice->slice.str[peekableSlice->cursor], .len = 0};
        // Handle pow()

        while (isalphanumeric(peekToken(peekableSlice))) {
            consumeToken(peekableSlice);
			operationOrVariableSlice.len++;
        }
		// maybe variable
		if(operationOrVariableSlice.len == 1){
			for(size_t varIdx = 0; varIdx < variables.len; varIdx++){
				char varName = variables.vars[varIdx].name;
				if(operationOrVariableSlice.str[0] == varName){
					pushPipeline(pipeline, makeStepAsVariableIndex(varIdx));
					return NO_PARSING_ERROR;
				}
			}
			return PARSING_ERROR(UNKNOWN_VARIABLE, peekableSlice->cursor-1, operationOrVariableSlice.str[0]);
		}
	
        PipelineOperation op = getOperationByName(operationOrVariableSlice);

        if (op != NULL) {


            if(!matchToken(peekableSlice, '(')){
				return PARSING_ERROR(UNEXPECTED, peekableSlice->cursor, peekToken(peekableSlice));
			}
			size_t argCount = 0;
            ParsingError err = compileExpression(pipeline, peekableSlice, variables);
			if(err.type != NOERROR){
				
				if(peekToken(peekableSlice) != ')'){
					return err;
				}
				else{
					// zero arguments
				}
			}
			else {
				argCount++;
			}

			size_t opArgCount = getMetaByOperation(op).argCount;
			
            while(matchToken(peekableSlice, ',')){
				if(argCount >= opArgCount){
					return PARSING_ERROR(TOO_MANY_ARGUMENTS, peekableSlice->cursor, peekToken(peekableSlice));
				}
				err = compileExpression(pipeline, peekableSlice, variables);
				if(err.type != NOERROR){
					return err;
				}
				argCount++;
			}

			if(argCount != opArgCount){
				return PARSING_ERROR(TOO_LITTLE_ARGUMENTS, peekableSlice->cursor, peekToken(peekableSlice)); 
			}
            //int right = compileExpression(pipeline, peekableSlice, variables);
			if(!matchToken(peekableSlice, ')')){
				return PARSING_ERROR(UNEXPECTED, peekableSlice->cursor, peekToken(peekableSlice));
			}

			pushPipeline(pipeline, makeStepAsOperation(op));
			if(pipeline->errorMask & OVERFLOW){
				return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
			}

            return NO_PARSING_ERROR;
        } else {
			size_t beginOfOperationName = peekableSlice->cursor - operationOrVariableSlice.len;
			return PARSING_ERROR(UNKNOWN_OPERATION, beginOfOperationName, peekableSlice->slice.str[beginOfOperationName]);
        }
    } else if (matchToken(peekableSlice, '(')) {
        ParsingError err = compileExpression(pipeline, peekableSlice, variables);
		if(err.type != NOERROR){
			return err;
		}
        if(!matchToken(peekableSlice, ')')){
			return PARSING_ERROR(UNEXPECTED, peekableSlice->cursor, peekToken(peekableSlice));
		}
        return NO_PARSING_ERROR;
    } else {
        return PARSING_ERROR(UNEXPECTED, peekableSlice->cursor, peekToken(peekableSlice));
    }
}


ParsingError parseMulDivToken(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables) {
    ParsingError err = parseConstantVariableOperationToken(pipeline, peekableSlice, variables);
    while (1) {
        if (matchToken(peekableSlice, '*')) {
            ParsingError err = parseConstantVariableOperationToken(pipeline, peekableSlice, variables);
			if(err.type != NOERROR){
				return err;
			}
			pushPipeline(pipeline, (PipelineVariant){.type = OPERATION_NATIVE_MUL});
			if(pipeline->errorMask & OVERFLOW){
				return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
			}

            //left = make_op("*", left, right);
        } else if (matchToken(peekableSlice, '/')) {
            ParsingError err = parseConstantVariableOperationToken(pipeline, peekableSlice, variables);
			if(err.type != NOERROR){
				return err;
			}
			pushPipeline(pipeline, (PipelineVariant){.type = OPERATION_NATIVE_DIV});
			if(pipeline->errorMask & OVERFLOW){
				return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
			}
        } else if (matchToken(peekableSlice, '%')) {
            ParsingError err = parseConstantVariableOperationToken(pipeline, peekableSlice, variables);
			if(err.type != NOERROR){
				return err;
			}
			pushPipeline(pipeline, (PipelineVariant){.type = OPERATION_NATIVE_MOD});
			if(pipeline->errorMask & OVERFLOW){
				return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
			}
        } else {
            break;
        }
    }
    return err;
}


ParsingError parseAddSubToken(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables){
	ParsingError err = parseMulDivToken(pipeline, peekableSlice, variables);
    while (1) {
        if (matchToken(peekableSlice, '+')) {
            ParsingError err = parseMulDivToken(pipeline, peekableSlice, variables);
			if(err.type != NOERROR){
				return err;
			}
			pushPipeline(pipeline, (PipelineVariant){.type = OPERATION_NATIVE_ADD});
			if(pipeline->errorMask & OVERFLOW){
				return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
			}
        } else if (matchToken(peekableSlice, '-')) {
            ParsingError err = parseMulDivToken(pipeline, peekableSlice, variables);
			if(err.type != NOERROR){
				return err;
			}
			pushPipeline(pipeline, (PipelineVariant){.type = OPERATION_NATIVE_SUB});
			if(pipeline->errorMask & OVERFLOW){
				return PARSING_ERROR(PIPELINE_FULL, peekableSlice->cursor, peekToken(peekableSlice));
			}
        } else {
            break;
        }
    }
    return err;

}

ParsingError compileExpression(Pipeline* pipeline, PeekableStringSlice* peekableSlice, PipelineVariablesSlice variables){
	if(peekableSlice->slice.len == 0 || peekableSlice->slice.str == NULL){
		return PARSING_ERROR(INPUT_EMPTY, 0, ' ');
	}
	return parseAddSubToken(pipeline, peekableSlice, variables);
}

void validateStackSizeWithPipeline(PipelineStack *stack, const Pipeline *pipeline){
	clearStack(stack);
	uint8_t pipelineLength = pipeline->index + 1;
	for(Index pipelineIdx = 0; pipelineIdx < pipelineLength; pipelineIdx++){
		const PipelineVariant* variant = &pipeline->entries[pipelineIdx];
		switch (variant->type)
		{
			case CONSTANT:
				{
					ValueType constant = variant->asConstant;
					pushStack(stack, constant);
				}
				break;
			case VARIABLE_INDEX:
				{
					VariableIndex variableIndex = variant->asVariableIndex;
					pushStack(stack, 1);
				}
				
				break;
			case OPERATION:
				{
					size_t argCount = getMetaByOperation(variant->asOperation).argCount;
					for(size_t argPop = 0; argPop < argCount; argPop++){
						popStack(stack);
						if(pipeline->errorMask != NO_ERROR || stack->errorMask != NO_ERROR){
							return;
						}
					}
					pushStack(stack, 1);
				}
				break;
			case NONE:
				return;

			if(pipeline->errorMask != NO_ERROR || stack->errorMask != NO_ERROR){
				return;
			}

			
		}
	}
	popStack(stack);
}