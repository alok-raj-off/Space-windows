
#ifndef LEXTOTOKEN_H
#define LEXTOTOKEN_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>


typedef enum
{
	_EMIT,
	_PLANET,
	_DOCK,
	INT_LIT,
	STRING,
	IDENT,
	EXP,
	EQUALS,
	SEMI,
	OPERATOR,

} Tokentype;

typedef struct
{
	Tokentype type;
	char *value;
} Token;


#include "./libs/vectores.c"
#include "./libs/dynamic_list.c"


TokenVector tokenizer(char *code)
{

	TokenVector tokens;
	vector_init(&tokens);

	char buffer[100];
	int bffidx = 0;
	for (int i = 0; i < strlen(code); i++)
	{
		if (isalpha(code[i]))
		{
			bffidx = 0;
			buffer[bffidx++] = code[i];
			i++;

			// Read the whole word
			while (isalnum(code[i]))
			{
				buffer[bffidx++] = code[i];
				i++;
			}
			i--; // Step back so the outer loop increments correctly

			// 1. Check for 'planet' keyword
			if (strncmp(buffer, "planet", bffidx) == 0 && bffidx == 6)
			{
				vector_push(&tokens, (Token){.type = _PLANET, .value = NULL});
			}
			// 2. Check for 'emit' keyword
			else if (strncmp(buffer, "emit", bffidx) == 0 && bffidx == 4)
			{
				vector_push(&tokens, (Token){.type = _EMIT, .value = NULL});
			}
			else if (strncmp(buffer, "dock", bffidx) == 0 && bffidx == 4)
			{
				vector_push(&tokens, (Token){.type = _DOCK, .value = NULL});
			}

			// 3. If it's not a keyword, it MUST be an identifier
			else
			{
				// Extract the actual variable name from the buffer
				char* ident_name = strndup(buffer, bffidx);

				// Push the token with the ACTUAL name, not NULL
				vector_push(&tokens, (Token){.type = IDENT, .value = ident_name});

				// Add it to your dynamic list safely
				add_to_list(ident_name);
			}

			buffer[0] = '\0';
			continue;
		}
		else if (isdigit(code[i]))
		{
			// printf("--- i stored an value ");
			bffidx = 0;
			buffer[bffidx++] = code[i];
			i++;
			while (isdigit(code[i]))
			{
				buffer[bffidx++] = code[i];
				i++;
			}
			i--;
			vector_push(&tokens, (Token){.type = INT_LIT, .value = strndup(buffer,bffidx)});
			buffer[0] = '\0';
			continue;
		}
		else if (code[i] == ';')
		{
			vector_push(&tokens, (Token){.type = SEMI, .value = NULL});
		}

		else if (isspace(code[i]))
		{
			continue;
		}
		else if (code[i] == '=')
		{
			vector_push(&tokens, (Token){.type = EQUALS, .value = NULL});
			// adding the vriables into list
		}
		// IN lexTotoken.h
		else if (code[i] == '+' || code[i] == '-' || code[i] == '*' || code[i] == '/' || code[i] == '%')
		{
			// Save the actual operator character into the token!
			char op_str[2] = {code[i], '\0'};
			vector_push(&tokens, (Token){.type = OPERATOR, .value = strdup(op_str)});
		}
		// here we gonna storing out strings
		else if (code[i] == '"')
		{
			bffidx = 0;
			i++; // Step past the opening quote

			// Keep reading until we hit the closing quote OR the end of the file
			while (code[i] != '"' && code[i] != '\0')
			{
				buffer[bffidx++] = code[i];
				i++; // Move to the next character
			}

			// Push the string token (Make sure you add STRING_LIT to your enum in lexTotoken.h!)
			vector_push(&tokens, (Token){.type = STRING, .value = strndup(buffer, bffidx)});

			buffer[0] = '\0'; // Clear the buffer for the next token
			continue;
		}

		else
		{
			printf(" you messed up ");
		}
	}
	return tokens;
}

#endif
