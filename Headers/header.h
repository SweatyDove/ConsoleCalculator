// Add BLAHBLAhhhhhh


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>


#define MAX_NUM_OF_OPERATIONS		10
#define SIZE_of_LINE				100
#define MAX_WORD					20
#define SIZE_of_STACK				10
#define NUM_of_REGISTERS			26

#define NO_STACK			01
#define NO_REGISTER			02
#define NO_FUNCTION			04


//#define OUTPUT(c)		((c >= 101 && c <= 126) ? 1 : 0)
//#define INPUT(c)		((c >= 1 && c <= 26))


#define mcr__IsSign(c)						(c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' || c == '(' || c == ')' || c == '\n' || c == ';')
#define mcr__IsSpecial(c)					(c == '(' || c == ')' || c == '\n' || c == ';')
#define mcr__IsDigit(c)						(c >= '0' && c <= '9') ? 1 : 0
#define mcr__IsLetter(c)					(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
//#define PrintRegister(c)					printf(#c " = %10f", rgstr[c - 'A'])
//#define paste(front, back)				front ## back
#define mcr__IsWhiteSpace(c)				(c == ' ' || c == '\t') ? 1 : 0
#define mcr__ToLow(c)						((c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c)
#define mcr__LowerCase(c)					(c >= 'a' && c <= 'z')
#define mcr__UpperCase(c)					(c >= 'A' && c <= 'Z')


enum stack_command {PUSH = 31, POP, SWAP, TOP, PRINT_STACK, CLEAR};
enum other_command {PRINT_VAR = 51, HELP};
enum function {SIN = 71, COS, EXP, SQRT, POW, LAST_FUNCTION};
enum boolean {false, true};



#define mcr__IsCommand(c)	(c >= PUSH && c <= HELP)
#define mcr__IsFunction(c)	(c >= SIN && c <= LAST_FUNCTION)




struct block_of_list {
	struct block_of_list* last_block;		// pointer to the previous block
	struct block_of_list* next_block;		// pointer to the next block 

	// Blocks can have several active bits;
	// e.g. in the case of the block with a "variable" (is_number == 1 and is_variable == 1).
	unsigned int is_number		:	1;
	unsigned int is_sign		:	1;
	unsigned int is_special		:	1;
	unsigned int is_variable	:	1;
	unsigned int is_command		:	1;
	unsigned int is_function	:	1;
	unsigned int				:	2;

	// I've used "union" 'cause a block has ONLY one type
	union {
		struct {
			float value;
			char variable;
		} number;

		struct {
			char value;
			char priority;
			short order;
		} sign;

		struct {
			int code;
			struct block_of_list *arg_x;
			struct block_of_list *arg_y;
		} function;

		struct {
			int code;
		} command;

	} type;										// May be replace type -> "a" or "the" or "is" etc
};



/************************************************/
/************** ?????????? ??????? **************/
/************************************************/

struct block_of_list	*AddBlock(void);
float					ReadNumber(char *line);
int						ReadName(char *line);


struct block_of_list	*UniteBlocks(struct block_of_list *middle_block);
int						FindInTable(char current_sign);
void					Sort(struct block_of_list *operations[], int left_edge, int right_edge);
int						GetPriority(char sign_of_operation);
int					GetResult(struct block_of_list *block_with_sign);
void					ReleaseDynamicMem(struct block_of_list *root);

int itob(int number, int base_of_number, int base_of_result);


void	CopyString(char *to, char *from, int n);
int		ReadString(char *line, int limit);
int		CompareStrings(char *str1, char *str2, int ignore_register);

void ExecuteCommand(int code_of_the_command);
float ExecuteFunction(int code_of_the_function, struct block_of_list *p);
int  SearchCommand(char *input_line);

int CreateList(struct block_of_list *to_root, char *from_line);
int SortList(struct block_of_list *root);
int CalcList(struct block_of_list *root);
int CheckList(struct block_of_list *root);


int SaveToStorage(struct block_of_list *block);

enum boolean GetTestLine (char *line, int limit);



