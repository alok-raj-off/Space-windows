#ifndef VECTORES_c
#define VECTORES_c

typedef struct
{
	Token *items; // The dynamic array of Tokens
	int size;	  // Keeps track of how many tokens are inside
	int capacity; // Keeps track of the total allocated memory slots
} TokenVector;

// this is sliocing the things




void vector_init(TokenVector *v)
{
	v->size = 0;
	v->capacity = 4;
	v->items = malloc(v->capacity * sizeof(Token));
}

void vector_push(TokenVector *v, Token item)
{
	if (v->size == v->capacity)
	{
		v->capacity *= 2;
		v->items = realloc(v->items, v->capacity * sizeof(Token));
	}
	v->items[v->size++] = item;
}


#endif
