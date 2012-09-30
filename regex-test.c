#include<stdio.h>
#include"regex.h"

int main(int argv, char *argc[])
{
	printf("Compilation begining...\n");
	InstructArray_t *container = compile(argc[1]);
	Instruct_t *array = container->array;
	int length = container->length;
	int index = 0;
	while(index < length)
	{
		Instruct_t instruct = array[index];
		switch(instruct.opcode)
		{
			case START:
				printf("%d START\n", index);
				break;
			case CHAR:
				printf("%d CHAR\t%c\n", index, instruct.character);
				break;
			case SPLIT:
				printf("%d SPLIT\t%d\t%d\n", index, instruct.left, instruct.right);
				break;
			case JMP:
				printf("%d JMP\t%d\n", index, instruct.left);
				break;
			case MATCH:
				printf("%d MATCH\n", index);
				break;
		}
		index++;
	}
	printf("Compilation complete.\n");
}

