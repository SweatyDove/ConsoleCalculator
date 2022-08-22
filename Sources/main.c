#include "header.h"

float gl_rgstr[NUM_of_REGISTERS + 1];				

int	FillTheList(struct block_of_list *root, char *from);
struct block_of_list *ManageBlocks(struct block_of_list *point_of_descent);
extern struct block_of_list *AddBlock();

int FindFunctionBlock(struct block_of_list *root);
int CreateFunctionBlock(struct block_of_list *block);

int FindMistakes(struct block_of_list *block);


struct block_of_list storage_base = {NULL, NULL, 0, 0, 0, 0, 0, 0, {0, '\0'}, };
struct block_of_list *storage = &storage_base;



/*	1) Разделить MangeBlocks() на составные части.
*	2) Оптимальный размер функции - с экран монитора
*	3) Выделить тестирование в отдельную функцию
*	4) Сразу выделить память под root (не в динамической памяти то есть)
*	5) После ReleaseDynamicMem(&storage_base), указатель storage продолжает указывать на освобожденный блок.
*/


int main(void)
{

	char line[SIZE_of_LINE] = {'\0'};
	
	struct block_of_list accumulator = {NULL, NULL, 1, 0, 0, 0, 0, 0, {0.0, '\0'}, };
	struct block_of_list *root = &accumulator;
	
	int is_tested = false;
	int is_exit = false;


	while (is_exit == false) {
		
		// Choose the input source:
		switch (is_tested) {
		case false:
			if (GetTestLine(line, SIZE_of_LINE) == false) {
				is_tested = true;
				printf("\n\nEnter yours EXPRESSION (or enter \"exit\" to exit): ");
				ReadString(line, SIZE_of_LINE);
			}
			break;
		case true:
			printf("\n\nEnter yours EXPRESSION (or enter \"exit\" to exit): ");
			ReadString(line, SIZE_of_LINE);
			if (CompareStrings(line, "exit", true)) {
				is_exit = true;
			}
			break;
		default:
			printf("\n\n>TYPE: error\n>SOURCE: main()\n>MESSAGE: is_tested = %d  <- it is unacceptable value", is_tested);
			is_exit = true;
			break;
		}
		
		// Work with double-linked list
		CreateList(root, line);
		if (CheckList(root) != 0) {
			printf("\n\nYou've entered incorrect expression. Try again (or enter \"exit\" to exit)>\t");
			ReleaseDynamicMem(root);
		}
		else {
			SortList(root);
			CalcList(root);

			// Copy the result of calculation into the accumulator
			if (root->next_block->is_number == 1) {
				accumulator.type.number.value = root->next_block->type.number.value;
				SaveToStorage(root->next_block);
				accumulator.next_block = NULL;
			}
			else {
				printf("\n\n>TYPE: error\n>SOURCE: main()\n>MESSAGE: original expression is balanced incorrectly");
				return -1;
			}


			printf("*****************************");
			printf("\nRecieved result:  %-15f", accumulator.type.number.value);
			
			ReleaseDynamicMem(&storage_base);
			storage = &storage_base;
		}


	} 

	printf("\nEnter any key to continue. . .");
	getchar();
	return 0;
}


int SaveToStorage(struct block_of_list *block)
{
	extern struct block_of_list *storage;


	if (storage != NULL) {
		storage->next_block = block;
		block->last_block = storage;
		storage = block;
		storage->next_block = NULL;
	}
	else {
		printf("\n\n>TYPE: error\n>SOURCE: SafeToStorage()\n>MESSAGE: storage == NULL");
		return 1;
	}
	
	return 0;


}





/* ReleaseDynamicMem():	release memory, allocated in the heap.
*/ 
void ReleaseDynamicMem(struct block_of_list *root)
{
	struct block_of_list *p, *q;

	for (p = root->next_block; p != NULL; p = q) {
		q = p->next_block;
		// Below we need to delete reference to the current block (which we are going to free)
		// from the previous one. If we don't do that, after the free(), the last block will
		// point to the random block with trash.
		if (p->last_block != NULL)
			p->last_block->next_block = NULL;
		free(p);								// Òóò âîçíèêàåò îøèáêà, ò.ê. ïûòàåìñÿ îñâîáîäèòü ÍÅÄÈÍÀÌÈ×ÅÑÊÈÉ ýë-ò.â
	}
}



/* FindMistakes():	preliminary error control of the list.
*/
int FindMistakes(struct block_of_list *block)
{
	int in_brackets = 0;

	for (block; block != NULL; block = block->next_block) {
		if (block->is_sign == 1) {
			// Parentheses mistakes
			if(block->type.sign.value == '(')
				in_brackets++;
			else if (block->type.sign.value == ')') {
				in_brackets--;
				if (in_brackets < 0) {
					printf("\n>TYPE: error\n>SOURCE: FindMistakes()\n>MESSAGE: Check the parentheses %p", block);
					return -1;
				}
			}
			// Other problems with "signs"
		}
		// Problems with other types of block

	}
	
	return 0;

}




/* FindFunctionBlock():	find block with the "name of function". 
*/
int FindFunctionBlock(struct block_of_list *root)
{
	struct block_of_list *block;

	for (block = root->next_block; block != NULL; block = block->next_block)
		if (block->is_function == 1)
			CreateFunctionBlock(block);

    return 0;
}


/* CreateFunctionBlock():	if we find function-block we will attach next blocks to the arguments
*							of this function. I mean this: [function: arg_x->[next_block]-[next_block]...] 
*/	
int CreateFunctionBlock(struct block_of_list *block)
{
	struct block_of_list *function_block;
	struct block_of_list *next_block;		
	int in_brackets = 0;					
	int arg = 1;				// Current argument of the function

	function_block = block;

	for (block = block->next_block; block != NULL; block = next_block) {
		next_block = block->next_block;

		if (block->is_number == 1) {
			continue;
		}
		else if (block->is_sign == 1) {
			if (block->type.sign.value == '(') {
				if (in_brackets == 0) {
					in_brackets = 1;

					// Further blocks are clung to the arg_x of the current function
					function_block->type.function.arg_x = next_block;
					function_block->type.function.arg_x->last_block = NULL;
					
					// Before to free current block (with the '(' char), we need to delete reference
					// on it from last block (read somewhere above why)
					block->last_block->next_block = NULL;
					free(block);
				}
				else {
					++in_brackets;
				}
				continue;
			}

			else if (block->type.sign.value == ')') {
				if (in_brackets == 1) {
					in_brackets = 0;
					
					block->last_block->next_block = NULL;
					free(block);
					break;
				}
				else {
					--in_brackets;
				}
				continue;
			}


			// The current function has several args (separated with the ';')
			else if (block->type.sign.value == ';') {
				++arg;

				// free [;]-block
				block->last_block->next_block = NULL;
				free(block);

				if (arg == 2) {
					function_block->type.function.arg_y = next_block;
					function_block->type.function.arg_y->last_block = NULL;
				}
				continue;
			}
			
			else
				continue;

		}

		// Met a new function inside current
		else if (block->is_function == 1) {
			CreateFunctionBlock(block);
			continue;
		}

		else {
			printf("\n>TYPE: error\n>SOURCE: CreateFunctionBlock()\n>MESSAGE: Incorrect type of the block %p", block);
			return -1;
		}	
	}

	function_block->next_block = next_block;
	if (next_block != NULL)
		next_block->last_block = function_block;

	return 0;
}
















/*
int CreateFunctionBlock(struct block_of_list *root)
{
	struct block_of_list *block;
	struct block_of_list *function_block;
	struct block_of_list *q;

	int in_brackets = 0;
	int arg = 1;
	
	static int nest_level = 0;			// Ð ÑÐ ÂµÐ¡ÐÐ ÂµÐ ÑÐ ÂµÐ ÐÐ ÐÐ Â°Ð¡Ð Ð ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ Â°Ð Â»Ð ÑÐ Â·Ð ÑÐ¡ÐÐ¡ÑÐ ÂµÐ¡âÐ¡ÐÐ¡Ð Ð ÑÐ âÐ ï¿œÐ Ñ Ð¡ÐÐ Â°Ð Â· Ð ÑÐ¡ÐÐ Ñ Ð ÐÐ¡â¹Ð Â·Ð ÑÐ ÐÐ Âµ Ð¡ÐÐ¡âÐ ÑÐ â Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ Ñ

	// Ð ÑÐ ÂµÐ¡ÐÐ ÂµÐ ÑÐ ÂµÐ ÐÐ ÐÐ Â°Ð¡Ð Ð ÑÐ ÑÐ ÑÐ Â°Ð Â·Ð¡â¹Ð ÐÐ Â°Ð ÂµÐ¡â Ð¡ÑÐ¡ÐÐ ÑÐ ÐÐ ÂµÐ ÐÐ¡Ð Ð ÐÐ Â»Ð ÑÐ Â¶Ð ÂµÐ ÐÐ ÐÐ ÑÐ¡ÐÐ¡âÐ Ñ Ð¡âÐ ÂµÐ ÑÐ¡ÑÐ¡â°Ð ÂµÐ â Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ Ñ
	++nest_level;

    // Ð ï¿œÐ¡â°Ð ÂµÐ Ñ Ð Â±Ð Â»Ð ÑÐ Ñ Ð¡Ð Ð ÑÐ ÑÐ ÂµÐ ÐÐ ÂµÐ Ñ Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ Ñ Ð ÒÐ Â»Ð¡Ð Ð¡ÐÐ ÑÐ Â·Ð ÒÐ Â°Ð ÐÐ ÑÐ¡Ð Ð Â±Ð Â»Ð ÑÐ ÑÐ Â°-Ð¡Ð-Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ ÂµÐ â (Ð ÐÐ¡ÑÐ Â¶Ð ÐÐ Ñ Ð ÑÐ¡ÐÐ ÑÐ ÐÐ¡ÑÐ ÒÐ ÑÐ¡âÐ ÂµÐ Â»Ð¡ÐÐ ÐÐ Ñ Ð ÐÐ¡â¹Ð¡âŠÐ ÑÐ ÒÐ ÑÐ¡âÐ¡Ð Ð ÑÐ Â·
	// Ð ÐÐ Â»Ð ÑÐ Â¶Ð ÂµÐ ÐÐ ÐÐ¡â¹Ð¡âŠ Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ â. Ð ÑÐ Â°Ð ÑÐ¡ÐÐ ÑÐ ÑÐ ÂµÐ¡Ð Ð ÑÐ Â· pow(). Ð ÑÐ Ñ Ð ÂµÐ¡ÐÐ¡âÐ¡Ð Ð ÑÐ Â°Ð Ñ Ð ÑÐ¡ÐÐ ÑÐ¡â¬Ð Â»Ð Â° Ð Â·Ð Â°Ð ÑÐ¡ÐÐ¡â¹Ð ÐÐ Â°Ð¡ÐÐ¡â°Ð Â°Ð¡Ð Ð¡ÐÐ ÑÐ ÑÐ Â±Ð ÑÐ Â°, Ð¡Ð Ð ÒÐ ÑÐ Â»Ð Â¶Ð ÂµÐ Ð Ð ÐÐ ÂµÐ¡ÐÐ ÐÐ¡ÑÐ¡âÐ¡ÐÐ¡ÐÐ¡Ð
	// Ð Ð sin())
	for (block = root->next_block; block != NULL; block = block->next_block) {
        // Ð ÑÐ¡ÐÐ ÑÐ ÑÐ¡ÑÐ¡ÐÐ ÑÐ Â°Ð ÂµÐ Ñ Ð ÒÐ¡ÐÐ¡ÑÐ ÑÐ ÑÐ Âµ Ð Â±Ð Â»Ð ÑÐ ÑÐ Ñ
		if (block->is_function != 1)
			continue;

		// Ð â¢Ð¡ÐÐ Â»Ð Ñ Ð ÐÐ Â°Ð¡â¬Ð Â»Ð Ñ Ð Â±Ð Â»Ð ÑÐ Ñ Ð¡Ð Ð ÑÐ ÑÐ ÂµÐ ÐÐ ÂµÐ Ñ Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ Ñ, Ð Â·Ð Â°Ð ÑÐ ÑÐ ÑÐ ÑÐ ÐÐ Â°Ð ÂµÐ Ñ Ð ÂµÐ ÑÐ Ñ
		function_block = block;

        // Ð ÂŠÐ ÑÐ ÑÐ Â» Ð ÑÐ¡âÐ ÐÐ ÂµÐ¡â¡Ð Â°Ð ÂµÐ¡â Ð Â·Ð Â° Ð ÐÐ Â°Ð ÑÐ ÑÐ Â»Ð ÐÐ ÂµÐ ÐÐ ÑÐ Âµ Ð Â±Ð Â»Ð ÑÐ ÑÐ Â°-Ð¡Ð-Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ ÂµÐ â
		for (block = block->next_block; ; block = q) {

			// Ð âÐ ÐÐ ÑÐ Â Ð â¢Ð ÑÐ ï¿œÐ âºÐ ÐÐ Ð Ð âÐ âºÐ ÑÐ Ñ Ð Ð Ð Â§Ð ï¿œÐ ÐÐ âºÐ ÑÐ Ñ
			if (block->is_number == 1) {
				q = block->next_block;
				continue;
			}

			// Ð âÐ ÐÐ ÑÐ Â Ð â¢Ð ÑÐ ï¿œÐ âºÐ ÐÐ Ð Ð âÐ âºÐ ÑÐ Ñ Ð ÐÐ Ñ Ð âÐ ÑÐ ÑÐ ÑÐ ÑÐ Ñ
			else if (block->is_sign == 1) {
				// Ð ÐÐ Â»Ð¡ÑÐ¡â¡Ð Â°Ð â Ð ÑÐ¡âÐ ÑÐ¡ÐÐ¡â¹Ð ÐÐ Â°Ð¡ÐÐ¡â°Ð ÂµÐ â Ð¡ÐÐ ÑÐ ÑÐ Â±Ð ÑÐ Ñ
				if (block->type.sign.value == '(') {
					if (in_brackets == 0) {
						in_brackets = 1;
						function_block->type.function.x = block->next_block;
						block->next_block->last_block = function_block->type.function.x;
						q = block->next_block;
						free(block);
					}
					else {
						++in_brackets;
						q = block->next_block;
					}
					continue;
				}

				// Ð âÐ¡ÐÐ¡âÐ¡ÐÐ ÂµÐ¡âÐ ÑÐ Â»Ð Â°Ð¡ÐÐ¡Ð Ð Â·Ð Â°Ð ÑÐ¡ÐÐ¡â¹Ð ÐÐ Â°Ð¡ÐÐ¡â°Ð Â°Ð¡Ð Ð¡âÐ ÑÐ ÑÐ¡ÑÐ¡ÐÐ ÐÐ Â°Ð¡Ð Ð¡ÐÐ ÑÐ ÑÐ Â±Ð ÑÐ Â°
				else if (block->type.sign.value == ')') {
					if (in_brackets == 1) {
						in_brackets = 0;
						block->type.sign.value = '\n';
						q = block->next_block;
						block->next_block = NULL;
						break;
					}
					else {
						--in_brackets;
						q = block->next_block;
					}
					continue;
				}

				// Ð âÐ Â°Ð ÑÐ¡ÐÐ¡â¹Ð ÐÐ Â°Ð¡Ð Ð¡âÐ ÂµÐ ÑÐ¡ÑÐ¡â°Ð ÑÐ â Ð Â°Ð¡ÐÐ ÑÐ¡ÑÐ ÑÐ ÂµÐ ÐÐ¡â Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ Ñ Ð Ñ Ð ÒÐ ÑÐ Â±Ð Â°Ð ÐÐ Â»Ð¡ÐÐ¡Ð Ð¡ÐÐ Â»Ð ÂµÐ ÒÐ¡ÑÐ¡ÐÐ¡â°Ð ÑÐ â (Ð ÑÐ ÑÐ ÑÐ Â° Ð ÑÐ Â°Ð ÑÐ¡ÐÐ ÑÐ ÑÐ¡ÑÐ Ñ 2 Ð Â°Ð¡ÐÐ ÑÐ¡ÑÐ ÑÐ ÂµÐ ÐÐ¡âÐ Â°)
				else if (block->type.sign.value == ';') {
					++arg;
					block->type.sign.value = '\n';
					q = block->next_block;
					block->next_block = NULL;
					if (arg == 2) {
						function_block->type.function.y = q;
						q->last_block = function_block->type.function.y;
					}
					continue;
				}
			}

			// Ð âÐ ÐÐ ÑÐ Â Ð â¢Ð ÑÐ ï¿œÐ âºÐ ÐÐ Ð Ð âÐ âºÐ ÑÐ Ñ Ð Ð Ð Â€Ð ÐÐ ÑÐ ÑÐ ÂŠÐ ï¿œÐ â¢Ð â¢
			else if (block->is_function == 1) {
				CreateFunctionBlock(block->last_block);
			}

			// Ð âÐ ÐÐ ÑÐ Â Ð â¢Ð ÑÐ ï¿œÐ âºÐ ÐÐ Ð Ð âÐ Â Ð ÐÐ âÐ ÑÐ â¢ Ð âÐ âºÐ ÑÐ Ñ
			else {
				printf("\n>TYPE: error\n>SOURCE: CreateFunctionBlock()\n>MESSAGE: Incorrect type of the block %p", block);
				return -1;
			}	
        }

        // Ð ÐÐ ÐÐ¡ÐÐ Â·Ð¡â¹Ð ÐÐ Â°Ð¡Ð Ð¡ÐÐ Â»Ð ÂµÐ ÒÐ¡ÑÐ¡ÐÐ¡â°Ð ÑÐ â Ð Â±Ð Â»Ð ÑÐ Ñ Ð ÐÐ ÂµÐ ÑÐ ÑÐ¡ÐÐ¡ÐÐ ÂµÐ ÒÐ¡ÐÐ¡âÐ ÐÐ ÂµÐ ÐÐ ÐÐ Ñ Ð¡Ð Ð Â±Ð Â»Ð ÑÐ ÑÐ ÑÐ Ñ-Ð¡Ð-Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ ÂµÐ â, Ð Â° Ð ÐÐ Âµ Ð¡Ð Ð ÂµÐ¡â Ð Â°Ð¡ÐÐ ÑÐ¡ÑÐ ÑÐ ÂµÐ ÐÐ¡âÐ Â°Ð ÑÐ Ñ
        function_block->next_block = q;
        q->last_block = function_block;

        // Ð âÐ ÑÐ Â·Ð ÐÐ¡ÐÐ Â°Ð¡â°Ð Â°Ð¡ÐÐ¡ÐÐ¡Ð Ð Ñ Ð¡âÐ ÑÐ ÑÐ¡Ñ Ð ÑÐ ÂµÐ¡ÐÐ¡âÐ¡Ñ, Ð ÑÐ ÒÐ Âµ Ð ÑÐ¡ÐÐ¡âÐ Â°Ð ÐÐ ÑÐ ÐÐ ÑÐ Â»Ð¡ÐÐ¡Ð (Ð¡Ð Ð¡ÑÐ Â¶Ð Âµ Ð ÑÐ ÑÐ¡âÐ ÑÐ ÐÐ¡â¹Ð Ñ Ð Â±Ð Â»Ð ÑÐ ÑÐ ÑÐ Ñ-Ð¡Ð-Ð¡âÐ¡ÑÐ ÐÐ ÑÐ¡â Ð ÑÐ ÂµÐ â)
        block = function_block;
    }

    return 0;
}
*/
