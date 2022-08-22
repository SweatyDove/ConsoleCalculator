#include "header.h"


/*  1) Пока что я предполагаю, что все операции выполняются над 2-мя операндами
*
*/

int CalcList(struct block_of_list *root)
{
    struct block_of_list	*block	=	NULL;
	struct block_of_list	*left	=   NULL;
    struct block_of_list    *right	=   NULL;

	float a = 0.0;
	float b = 0.0;
	float result = 0.0;
	char operator = '\0';



    for (block = root->next_block; block != NULL; block = block->next_block) {
		// Looking for the block with arithmetic sign
        if (block->is_sign == 1) {
			// Set the values of left and right arguments
			if ((left != NULL) && (right != NULL)) {
			    a = left->type.number.value;
    			b = right->type.number.value;
			}
			else {
				printf("\n\n>TYPE: error\n>SOURCE: CalcList()\n>MESSAGE: initial expression is malformed:");
				return 1;
			}

			operator = block->type.sign.value;
			switch(operator) {
			case '*':
				result = a * b;
				break;
			case '/':
				result = a / b;
				break;
			case '%':
				result = (int)a - ((int)a / (int)b) * (int)b;
				break;
			case '+':
				result = a + b;
				break;
			case '-':
				result = a - b;
				break;
			default:
				printf("\n\n>TYPE: error\n>SOURCE: CalcList()\n>MESSAGE: incorrect operator: %c", operator);
				return 2;
				break;
			}

			block->last_block = left->last_block;
			if (left->last_block != NULL) {
				left->last_block->next_block = block;
			}			
			block->is_sign = 0;
			block->is_number = 1;
			block->type.number.value = result;

			SaveToStorage(left);
			SaveToStorage(right);
		}
		// Do nothing
		else {
			;
		}
		right = block;
		left = block->last_block;
	}
    
    return 0;
}


/* GetResult():	returns the result of the arithmetic operation
*
*	Comment:	1)	При входе в эту функцию почему-то меняется адрес у root (Может потому-что root - статичный объект, а не динамический)?
*					И по этой причине в какой-то момент его адрес изменяется.
*/
/* int GetResult(struct block_of_list *block)
{
    struct block_of_list    *left    =   NULL;
    struct block_of_list    *right   =   NULL;

	float a = 0.0;
	float b = 0.0;
	float result = 0.0;
    char operator = block->type.sign.value;

	right = block->last_block;
    left = right->last_block;					// Здесь left почему-то указывает НЕ на root в последнем 

    a = left->type.number.value;
    b = right->type.number.value;
    

	switch(operator) {
	case '*':
		result = a * b;
		break;
	case '/':
		result = a / b;
		break;
	case '%':
		result = (int)a - ((int)a / (int)b) * (int)b;
		break;
	case '+':
		result = a + b;
		break;
	case '-':
		result = a - b;
		break;
	default:
		printf("\n>TYPE: error\n>SOURCE: GetResult()\n>MESSAGE: incorrect operator: %c", operator);
		return 1;
		break;
	}

    block->last_block = left->last_block;
	if (left->last_block != NULL) 
		left->last_block->next_block = block;
    
    block->is_sign = 0;
    block->is_number = 1;
    block->type.number.value = result;


	SafeToStorage(left);
	SafeToStorage(right);

	return 0;
} */


/* UniteBlocks():	unite three blocks into one
struct block_of_list* UniteBlocks(struct block_of_list* block)
{
	struct block_of_list* left = NULL;
    struct block_of_list* right = NULL;

	right = block->last_block;
	left = right->last_block;

	block->last_block = left->last_block;
	if (left->last_block != NULL) 
		left->last_block->next_block = block;
	free(lb);

	return block;
	
}
*/