#define REG_BOUND 256
#define STAR 257
#define QUES 258
#define PLUS 259
#define PIPE 260
#define LEFT_PAREN 261
#define RIGHT_PAREN 262
#define EMPTY 263

typedef enum
{
	START,
	START_,
	JUX,
	JUX_,
	QUANT,
	LEAF,
	EPSILON
} NodeType_t;

struct node
{
	NodeType_t type;
	int character;
	struct node *left;
	struct node *right;
};

typedef struct node Node_t;

typedef enum
{
	BEGIN,
	CHAR,
	SPLIT,
	JMP,
	MATCH
} Opcode_t;

typedef struct
{
	Opcode_t opcode;
	char character;
	int left;
	int right;
} Instruct_t;

typedef struct
{
	Instruct_t *array;
} InstructArray_t;

struct list
{
	Instruct_t instruct;
	int index;
	struct list *next;
};

typedef struct list InstructList_t;

InstructArray_t *compile(char regex[]); //defines the function to be called for compiling

