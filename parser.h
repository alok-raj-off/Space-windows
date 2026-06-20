#ifndef PARSER_H
#define PARSER_H

#include "lexTotoken.h"
#include "./libs/dynamic_list.c"

typedef enum{
	NODE_EXP,
	NODE_EMIT,
	NODE_VAR_TYPE,
	NODE_VARIABLEL,
	NODE_IDENTIFIER,
	NODE_INT_LIT,
	NODE_OPERATOR,
	NODE_DOCK,   
	NODE_INT_LIT_DOCK,
	NODE_STRING_DOCK,
	NODE_DOCK_INDETFIER,
} NodeType;

typedef struct ATSnode{
	NodeType type;
	char *name;
	char *value;
	struct ATSnode *left;
	struct ATSnode *right;
} ATSnode;

typedef struct
{
	char* relative_var[100];
	int var_type[100];         // 0 for Integer, 1 for String
	int stack_pointer;
	int stack_size;
} Assembly_pointers;

Assembly_pointers assembpoiners;

int push(char * varname, int type)
{
	assembpoiners.stack_pointer++;
	assembpoiners.relative_var[assembpoiners.stack_pointer - 1] = strdup(varname);
	assembpoiners.var_type[assembpoiners.stack_pointer - 1] = type; 
	return 0;
}

ATSnode* createNode(NodeType type, char* value){
	ATSnode* node = malloc(sizeof(ATSnode));
	node->type = type;
	if(value != NULL){
		node->value = strdup(value);
	}else{
		node->value = NULL;
	}
	node->left = NULL;
	node->right = NULL;
	return node;
}

// ==========================================
// AST RECURSIVE PARSER (Unchanged!)
// ==========================================
ATSnode* parse_factor(TokenVector* tkn, int* idx) {
	if (*idx < tkn->size) {
		if (tkn->items[*idx].type == INT_LIT) {
			ATSnode* node = createNode(NODE_INT_LIT, tkn->items[*idx].value);
			(*idx)++;
			return node;
		}
		if (tkn->items[*idx].type == IDENT) {
			ATSnode* node = createNode(NODE_IDENTIFIER, tkn->items[*idx].value);
			(*idx)++;
			return node;
		}
		if (tkn->items[*idx].type == STRING) {
			ATSnode* node = createNode(NODE_STRING_DOCK, tkn->items[*idx].value);
			(*idx)++;
			return node;
		}
	}
	return NULL;
}

ATSnode* parse_term(TokenVector* tkn, int* idx) {
	ATSnode* left = parse_factor(tkn, idx);
	while (*idx < tkn->size && tkn->items[*idx].type == OPERATOR &&
		   (strcmp(tkn->items[*idx].value, "*") == 0 ||
			strcmp(tkn->items[*idx].value, "/") == 0 ||
			strcmp(tkn->items[*idx].value, "%") == 0)) 
	{
		char* op = tkn->items[*idx].value;
		(*idx)++; 
		ATSnode* right = parse_factor(tkn, idx); 
		ATSnode* new_node = createNode(NODE_OPERATOR, op);
		new_node->left = left;
		new_node->right = right;
		left = new_node; 
	}
	return left;
}

ATSnode* parse_expr(TokenVector* tkn, int* idx) {
	ATSnode* left = parse_term(tkn, idx); 
	while (*idx < tkn->size && tkn->items[*idx].type == OPERATOR &&
		   (strcmp(tkn->items[*idx].value, "+") == 0 ||
			strcmp(tkn->items[*idx].value, "-") == 0)) 
	{
		char* op = tkn->items[*idx].value;
		(*idx)++; 
		ATSnode* right = parse_term(tkn, idx); 
		ATSnode* new_node = createNode(NODE_OPERATOR, op);
		new_node->left = left;
		new_node->right = right;
		left = new_node; 
	}
	return left;
}

// ==========================================
// TRANSLATOR: AST to C Code String
// ==========================================
// This replaces your old assembly generation. It recursively walks the math tree and builds a standard C equation.
char* exprToC(ATSnode* node) {
	if (!node) return strdup("");
	char buffer[1024];

	if (node->type == NODE_INT_LIT || node->type == NODE_IDENTIFIER || 
		node->type == NODE_INT_LIT_DOCK || node->type == NODE_DOCK_INDETFIER) {
		return strdup(node->value);
	}
	if (node->type == NODE_STRING_DOCK) {
		snprintf(buffer, sizeof(buffer), "\"%s\"", node->value);
		return strdup(buffer);
	}
	if (node->type == NODE_OPERATOR) {
		char* left = exprToC(node->left);
		char* right = exprToC(node->right);
		snprintf(buffer, sizeof(buffer), "(%s %s %s)", left, node->value, right);
		free(left); free(right);
		return strdup(buffer);
	}
	return strdup("");
}

// ==========================================
// MAIN COMPILER LOOP
// ==========================================
char* tokeToC(TokenVector tkn, Assembly_pointers* assembly)
{
	// Setup standard C boilerplate instead of Assembly sections
	char* final_c = calloc(8192, sizeof(char));
	strcpy(final_c, "#include <stdio.h>\n#include <stdlib.h>\n\nint main() {\n");

	int i = 0;
	while (i < tkn.size) {
		Token defToken = tkn.items[i];

		if (defToken.type == _PLANET) {
			ATSnode* node_for_variable = createNode(NODE_VARIABLEL, NULL);
			if (i + 1 < tkn.size && tkn.items[i + 1].type == IDENT) {
				node_for_variable->left = createNode(NODE_IDENTIFIER, tkn.items[i + 1].value);
			}

			if (i + 2 < tkn.size && tkn.items[i + 2].type == EQUALS) {
				i += 3; 
				node_for_variable->right = parse_expr(&tkn, &i);

				if (i < tkn.size && tkn.items[i].type == SEMI) {
					int is_string = 0;
					if (node_for_variable->right->type == NODE_STRING_DOCK) is_string = 1;
					push(node_for_variable->left->value, is_string);

					char* exprStr = exprToC(node_for_variable->right);
					char stmt[1024];
					
					// If it's a string use char*, otherwise use a large integer type
					if (is_string) {
						snprintf(stmt, sizeof(stmt), "    char* %s = %s;\n", node_for_variable->left->value, exprStr);
					} else {
						snprintf(stmt, sizeof(stmt), "    long long %s = %s;\n", node_for_variable->left->value, exprStr);
					}
					
					strcat(final_c, stmt);
					free(exprStr);
					i++; 
				}
			}
		}
		else if (defToken.type == _DOCK)
		{
			ATSnode* node_for_DOCK = createNode(NODE_DOCK, NULL);
			if (i + 1 < tkn.size && tkn.items[i + 1].type == INT_LIT) {
				node_for_DOCK->left = createNode(NODE_INT_LIT_DOCK, tkn.items[i + 1].value);
			}
			else if(i + 1 < tkn.size && tkn.items[i + 1].type == IDENT) {
				node_for_DOCK->left = createNode(NODE_DOCK_INDETFIER, tkn.items[i + 1].value);
			}
			else {
				node_for_DOCK->left = createNode(NODE_STRING_DOCK, tkn.items[i + 1].value);
			}

			// Figure out the type so C knows whether to print text (%s) or numbers (%lld)
			char* exprStr = exprToC(node_for_DOCK->left);
			int is_string = 0;
			if (node_for_DOCK->left->type == NODE_STRING_DOCK) is_string = 1;
			if (node_for_DOCK->left->type == NODE_DOCK_INDETFIER) {
				for (int j = 0; j < assembly->stack_pointer; j++) {
					if (strcmp(node_for_DOCK->left->value, assembly->relative_var[j]) == 0) {
						is_string = assembly->var_type[j];
						break;
					}
				}
			}

			char stmt[1024];
			if (is_string) {
				snprintf(stmt, sizeof(stmt), "    printf(\"%%s\\n\", %s);\n", exprStr);
			} else {
				snprintf(stmt, sizeof(stmt), "    printf(\"%%lld\\n\", %s);\n", exprStr);
			}
			
			strcat(final_c, stmt);
			free(exprStr);
			i += 3;
		}
		else if (defToken.type == _EMIT) {
			i++; 
			ATSnode* mathTree = parse_expr(&tkn, &i);
			if (i < tkn.size && tkn.items[i].type == SEMI) {
				char* exprStr = exprToC(mathTree);
				char stmt[1024];
				snprintf(stmt, sizeof(stmt), "    return %s;\n", exprStr);
				strcat(final_c, stmt);
				free(exprStr);
				i++; 
			}
		}
		else {
			i++; 
		}
	}

	strcat(final_c, "}\n");
	return final_c;
}

#endif
