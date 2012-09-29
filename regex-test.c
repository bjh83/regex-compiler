#include<stdio.h>
#include"regex.h"

int main()
{
	printf("Compilation begining...\n");
	InstructArray_t *container = compile("aaa");
	Instruct_t *array = container->array;
	FILE *file = fopen("test.txt", "w");
	int length = sizeof(array) / sizeof(Instruct_t);
	int index = 0;
	fprintf(file, "Size of file:%d\nOPCODES:\n", length);
	while(index < length)
	{
		Instruct_t instruct = array[index];
		switch(instruct.opcode)
		{
			case START:
				fprintf(file, "START\n");
				break;
			case CHAR:
				fprintf(file, "CHAR\t%c\n", instruct.character);
				break;
			case SPLIT:
				fprintf(file, "SPLIT\t%d\t%d\n", instruct.left, instruct.right);
				break;
			case JMP:
				fprintf(file, "JMP\t%d\n", instruct.left);
			case MATCH:
				fprintf(file, "MATCH\n");
		}
		index++;
	}
	printf("Compilation complete.\n");
}

