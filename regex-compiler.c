#include<stdio.h>
#include<stdlib.h>
#include"regex.h"

int lex(char old_string[], int new_string[])
{
	printf("Lexing beginning...\n");
	int index = 0;
	int new_index = 0;
	int length = sizeof(old_string) / sizeof(char);
	int escaped = 0;
	while(index < length)
	{
		switch(old_string[index])
		{
			case '\\':
				if(escaped)
				{
					new_string[new_index] = '\\';
					escaped = 0;
				}
				else
				{
					escaped = 1;
					new_index--; //So that new index is not incremented
				}
				break;
			case '*':
				if(escaped)
				{
					new_string[new_index] = '*';
					escaped = 0;
				}
				else
					new_string[new_index] = STAR;
				break;
			case '+':
				if(escaped)
				{
					new_string[new_index] = '+';
					escaped = 0;
				}
				else
					new_string[new_index] = PLUS;
			case '?':
				if(escaped)
				{
					new_string[new_index] = '?';
					escaped = 0;
				}
				else
					new_string[new_index] = QUES;
				break;
			case '|':
				if(escaped)
				{
					new_string[new_index] = '|';
					escaped = 0;
				}
				else
					new_string[new_index] = PIPE;
				break;
			case '(':
				if(escaped)
				{
					new_string[new_index] = '(';
					escaped = 0;
				}
				else
					new_string[new_index] = LEFT_PAREN;
				break;
			case ')':
				if(escaped)
				{
					new_string[new_index] = ')';
					escaped = 0;
				}
				else
					new_string[new_index] = RIGHT_PAREN;
				break;
			default:
				new_string[new_index] = old_string[index];
				break;
		}
		if(escaped && old_string[index] != '\\')
			return 0; //ERROR: Something was illeagally escaped
		index++;
		new_index++;
	}
	new_string[new_index] = EMPTY;
	printf("Lexing complete...\nLength of regex:\t%d\n", new_index + 1);
	return 1;
}

int parse(int **string, Node_t *node) //returns 1 on success
{
	NodeType_t type = node->type;
	if(**string == EMPTY)
	{
		//This automatically closes the leaves when all input is consumed
		node->type = EPSILON;
		return 1;
	}
	switch(type)
	{
		case START:
		{
			//Nothing can go wrong here
			Node_t *left = malloc(sizeof(Node_t));
			left->type = JUX;
			node->left = left;
			if(!parse(string, left))
			{
				return 0;
			}
			Node_t *right = malloc(sizeof(Node_t));
			right->type = START_;
			node->right = right;
			if(!parse(string, right))
			{
				return 0;
			}
			return 1;
		}
		case START_:
		{
			if(**string == PIPE)
			{
				//branches for a pipe
				(*string)++; //Consume the pipe
				Node_t *left = malloc(sizeof(Node_t));
				left->type = JUX;
				node->left = left;
				if(!parse(string, left))
				{
					return 0;
				}
				Node_t *right = malloc(sizeof(Node_t));
				right->type = START_;
				node->right = right;
				if(!parse(string, right))
				{
					return 0;
				}
				return 1;
			}
			node->type = EPSILON; //if no pipe was found
			return;
		}
		case JUX:
		{
			Node_t *left = malloc(sizeof(Node_t));
			left->type = QUANT;
			node->left = left;
			if(!parse(string, left))
			{
				return 0;
			}
			Node_t *right = malloc(sizeof(Node_t));
			right->type = JUX_;
			node->right = right;
			if(!parse(string, right))
			{
				return 0;
			}
			return 1;
		}
		case JUX_:
		{
			if(**string < REG_BOUND || **string == LEFT_PAREN) //We cannot find a RIGHT_PAREN here
			{
				//do not consume the input since we have only determined
				//that it is either a character or a group
				Node_t *left = malloc(sizeof(Node_t));
				left->type = QUANT;
				node->left = left;
				if(!parse(string, left))
				{
					return 0;
				}
				Node_t *right = malloc(sizeof(Node_t));
				right->type = JUX_;
				node->right = right;
				if(!parse(string, right))
				{
					return 0;
				}
				return 1;
			}
			node->type = EPSILON; //No more juxtoposition
			return 1;
		}
		case QUANT:
		{
			Node_t *left = malloc(sizeof(Node_t));
			left->type = LEAF;
			node->left = left;
			if(!parse(string, left))
			{
				return 0;
			}
			if(**string == STAR)
			{
				node->character = STAR;
				(*string)++; //Consume the STAR
			} else if(**string == PLUS) {
				node->character = PLUS;
				(*string)++; //Consume the PLUS
			} else if(**string == QUES) {
				node->character = QUES;
				(*string)++; //Consume the QUES
			} else {
				node->character = EMPTY;
			}
			return 1;
		}
		case LEAF:
		{
			if(**string == LEFT_PAREN)
			{
				(*string)++; //Consume the left paren
				Node_t *left = malloc(sizeof(Node_t));
				left->type = START;
				node->left = left;
				if(!parse(string, left))
				{
					return 0;
				}
				if(**string == RIGHT_PAREN)
				{
					(*string)++; //Consume the right paren
				} else {
					return 0; //ERROR: No right paren was found
				}
				return 1;
			} else if(**string < REG_BOUND) {
				node->character = **string;
				(*string)++; //Consumes a regular character
				return 1;
			} else {
				//ERROR: Unrecognized character
				//This is where most errors are expected to be caught
				return 0;
			}
		}
	}
}

InstructList_t *generate(Node_t *node, InstructList_t *list)
{
	switch(node->type)
	{
		case START:
			list = generate(node->left, list);
			return generate(node->right, list);
		case START_: //We know this is an or otherwise it would be an EPSILON
		{
			InstructList_t *save = malloc(sizeof(InstructList_t));
			save->index = list->index + 1; //XXX: I SHOULD REALLY LOOK THIS OVER AS I WAS VERY SLEEPY WHEN I WROTE IT!!!!!!!!!!!!!!!!!
			save->instruct.opcode = SPLIT;
			save->instruct.left = save->index + 1;
			list->next = save;
			list = generate(node->left, save);
			save->instruct.right = list->index + 1;
			return generate(node->right, list);
		}
		case JUX:
			list = generate(node->left, list);
			return generate(node->right, list);
		case JUX_:
			list = generate(node->left, list);
			return generate(node->right, list);
		case QUANT:
		{
			switch(node->character)
			{
				case STAR:
				{
					InstructList_t *save = malloc(sizeof(InstructList_t));
					save->index = list->index + 1;
					save->instruct.opcode = SPLIT;
					save->instruct.left = save->index + 1;
					list->next = save;
					list = generate(node->left, save);
					list->next = malloc(sizeof(InstructList_t));
					list->next->index = list->index + 1;
					list = list->next;
					list->instruct.opcode = JMP;
					list->instruct.left = save->index;
					save->instruct.right = list->index + 1;
					return list;
				}
				case PLUS:
				{
					InstructList_t *save = list;
					list = generate(node->left, list);
					list->next = malloc(sizeof(InstructList_t));
					list->next->index = list->index;
					list = list->next;
					list->instruct.opcode = SPLIT;
					list->instruct.left = save->index + 1;
					list->instruct.right = list->index + 1;
					return list;
				}
				case QUES:
				{
					InstructList_t *save = malloc(sizeof(InstructList_t));
					save->index = list->index + 1;
					save->instruct.opcode = SPLIT;
					save->instruct.left = save->index + 1;
					list->next = save;
					list = generate(node->left, save);
					save->instruct.right = list->index + 1;
					return list;
				}
				case EMPTY:
					return generate(node->left, list);
			}
		}
		case LEAF:
		{
			list->next = malloc(sizeof(InstructList_t));
			list->next->index = list->index + 1;
			list = list->next;
			list->instruct.opcode = CHAR;
			list->instruct.character = (char) node->character;
			return list;
		}
		case EPSILON:
			return list;
	}
}

void free_tree(Node_t *node)
{
	if(node->left != NULL)
	{
		free_tree(node->left);
	}
	if(node->right != NULL)
	{
		free_tree(node->right);
	}
	free(node);
}

InstructArray_t *compile(char regex[])
{
	int length = sizeof(regex) / sizeof(char);
	length++;
	int *lexed_string = (int *) malloc(sizeof(int) * length);
	if(!lex(regex, lexed_string))
	{
		printf("Lexing failed...\n");
		return NULL;
	}
	Node_t *root = malloc(sizeof(Node_t));
	root->type = START;
	int *free_lexed_string = lexed_string;
	if(!parse(&lexed_string, root))
	{
		printf("Parsing failed...\n");
		return NULL;
	}
	free(free_lexed_string);
	InstructList_t *instruct_list_head = malloc(sizeof(InstructList_t));
	instruct_list_head->index = 0;
	instruct_list_head->instruct.opcode = BEGIN;
	InstructList_t *instruct_list_tail = generate(root, instruct_list_head);
	free_tree(root);
	instruct_list_tail->next = malloc(sizeof(InstructList_t));
	instruct_list_tail->next->index = instruct_list_tail->index + 1;
	instruct_list_tail = instruct_list_tail->next;
	instruct_list_tail->instruct.opcode = MATCH;
	length = instruct_list_tail->index + 1;
	printf("Length of list:\t%d\n", length);
	Instruct_t *array = (Instruct_t *) malloc(sizeof(Instruct_t) * length);
	int index = 0;
	while(index < length)
	{
		array[index] = instruct_list_head->instruct;
		InstructList_t *to_delete = instruct_list_head;
		instruct_list_head = instruct_list_head->next;
		free(to_delete);
		index++;
	}
	InstructArray_t *ret_val = malloc(sizeof(InstructArray_t));
	ret_val->array = array;
	return ret_val;
}

