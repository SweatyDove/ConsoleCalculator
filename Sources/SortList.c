
#include "header.h"

extern float gl_rgstr[];


/* SortList():	this function gets mathematical expression in the form of the list of blocks,
*				each of which contains a sign, or a number, or a function;
*				then it sorts this list in the order of execution and return the result
*				as the stack of these blocks (LIFO).
*
*	Comments:	1) Добавить функцию на перестановку texas->california или florida->texas
*				2) НУЖЕН ЦИКЛ в том случае, когда нужно пропустить вагоны из Техаса, т.к. у них больший приоритет чем у вагона из Флориды
*/				
int SortList(struct block_of_list *root)
{	

	struct block_of_list *florida			=	root->next_block;
	struct block_of_list *fork				=	NULL;		// It is a temporary place, where every element from Florida comes to
	struct block_of_list *hangar			=	NULL;		// Temporary hangar
					

	struct block_of_list california_depo 	= 	{NULL, NULL, 0, 0, 0, 0, 0, 0, {0.0, '\0'}, };
	struct block_of_list texas_depo 		=	{NULL, NULL, 0, 0, 0, 0, 0, 0, {0.0, '\0'}, };
	//struct block_of_list thiefed_depo		=	{NULL, NULL, 0, 0, 0, 0, 0, 0, {0.0, '\0'}, };

	struct block_of_list *california 		=	&california_depo;
	struct block_of_list *texas				=	&texas_depo;
	//struct block_of_list* thiefed			=	&thiefed_depo;



	while (florida != NULL) {

		// The wagon (aka block_of_list) from Florida have to choose further direction (California or Texas)
		fork = florida;
		florida = florida->next_block;
		fork->next_block = NULL;
		fork->last_block = NULL;

		// Block with a number is going directly to California
		if (fork->is_number == 1) {
			california->next_block = fork;
			fork->last_block = california;
			california = california->next_block;

			fork = NULL;
		}

		// Met the block with arithmetic sign
		else if (fork->is_sign == 1 && fork->is_special == 0) {

			// Texas is not empty and has more or equal priority
			while ((texas != &texas_depo) && (fork->type.sign.priority <= texas->type.sign.priority)) {
				hangar = texas;
				texas = texas->last_block;

				texas->next_block = NULL;
				hangar->last_block = NULL;
				hangar->next_block = NULL;

				california->next_block = hangar;
				hangar->last_block = california;
				california = california->next_block;

				hangar = NULL;
			}

			// The wagon on the fork is more priority
			texas->next_block = fork;
			fork->last_block = texas;
			texas = texas->next_block;

			fork = NULL;
		}

		// Met the block with non-arithmetic (special) sign
		else if (fork->is_sign == 1 && fork->is_special == 1) {
			if (fork->type.sign.value == '(') {
				texas->next_block = fork;
				fork->last_block = texas;
				texas = texas->next_block;

				fork = NULL;
			}
			else if (fork->type.sign.value == ')') {
				// Arithmetic signs go to California
				while (texas->type.sign.value != '(') { 
					hangar = texas;
					texas = texas->last_block;
					hangar->last_block = NULL;
					hangar->next_block = NULL;

					california->next_block = hangar;
					hangar->last_block = california;
					california = california->next_block;

					hangar = NULL;
				}

				// Blocks with parenthesis have been thiefed (and then we will use free() function)
				hangar = texas;
				texas = texas->last_block;

				SaveToStorage(hangar);
				hangar = NULL;

				SaveToStorage(fork);
				fork = NULL;

			}
			// Do not need '\n' sign by this moment
			else if (fork->type.sign.value == '\n') {
				SaveToStorage(fork);
				fork = NULL;
			}
			else {
				printf("\n\n>TYPE: error\n>SOURCE: SortList()\n>MESSAGE: Met an unaccounted special sign \'%c\'", fork->type.sign.value);
				return -2;
			}
		}

		else {
			printf("\n\n>TYPE: error\n>SOURCE: SortList()\n>MESSAGE: Unknown content of the current block %p", fork);
			return -1;
		}

	}
	// Remaning signs in Texas go to California
	while (texas != &texas_depo) {
		hangar = texas;
		texas = texas->last_block;
		hangar->last_block = NULL;
		hangar->next_block = NULL;

		california->next_block = hangar;
		hangar->last_block = california;
		california = california->next_block;
		
		hangar = NULL;
	}

	if (california != &california_depo) {
		california = california_depo.next_block;
		california->last_block = root;
		root->next_block = california;
	}
	else {
		printf("\n\n>TYPE: warning\n>SOURCE: SortList()\n>MESSAGE: The list is empty");
	}

	return 0;
}

/*	ManageBlocks(): ф-ция управляет расстановкой приоритетов операций, сортировкой операций и т.д. Если встречаются скобки, то происходит "спуск" в них,
 *	путём рекурсивного вызова данной функции. Возвращает указатель на блок с результатом */
/*struct block_of_list *ManageBlocks(struct block_of_list *root)
{
	int i;
	char *name;

	struct block_of_list *p				= NULL;
	struct block_of_list *result_block	= NULL;		// Блок с результатом текущих вычислений


	static struct block_of_list *operations[MAX_NUM_OF_OPERATIONS];		// Массив указателей на блоки, содержащие знаки арифметических операций
	static int opi;														// Итератор массива operation[]

	int left_edge = opi;												// Левая граница сортировки на текущем "этаже" (изначально равна 0)


	result_block = root->next_block;	// Случай, когда в выражении лишь одно число

/***************************************************************************************/
/*******    Set operation priorities    ****************************************/
/***************************************************************************************/
/*	for (p = root->next_block; p != NULL; p = p->next_block) {
		if (p->is_sign == 1) {
			// if we met '(', then calls ManageBlocks() recursively
			if (p->type.sign.value == '(') {
				p->next_block = ManageBlocks(p);	// Получим конструкцию "(" -> "Результат" -> ")"
				p = UniteBlocks(p->next_block);		//	и сведём её в один блок с результатом
				result_block = p;
				continue;
			}
			else if (p->type.sign.value == ')' || p->type.sign.value == '\n' ) {
				break;
			}
			else {
				if (FindInTable(p->type.sign.value))
					operations[opi++] = p;
				else
					printf("\nerror: from ManageBlocks(): incorrect sign encountered \"%c\"\n", p->type.sign.value);
			}
			
		}
		// 1.2.0
		// Расчет функций
		else if (p->is_function == 1) {
			for (i = 0; *(name = func[i].name) != '\0'; ++i)
				if (CompareStrings(p->type.function.name, name, 1) == 1) {
					p->type.number.value = ExecuteFunction(func[i].code, p);
					p->is_function = 0;
					p->is_number = 1;
					result_block = p;
					break;
				}
			if (name == '\0') {
				printf("\n>TYPE: error\n>SOURCE: ManageBlocks()\n>MESSAGE: Unknown name of the function: \"%s\"", p->type.function.name);
				return NULL;
			}
		}
		
	}



	//#### 2.0.0 #### СОРТИРОВКА ОПЕРАЦИЙ ПО ИХ ПРИОРИТЕТУ (НА ТЕКУЩЕМ "ЭТАЖЕ")
	Sort(operations, left_edge, opi - 1);

	//#### 3.0.0 #### ВЫЧИСЛЕНИЯ
	for (i = left_edge; opi > left_edge; --opi, ++i) {
		p = operations[i];
		operations[i] = NULL;

		//## 3.1.0 ## Блок со знаком операции заменяется блоком с результатом этой операции
		if (p->type.sign.value == '=') {
			if (p->last_block->is_variable == 1) {		// Запись в переменную
				gl_rgstr[p->last_block->type.number.variable - 'a' + 1] = p->next_block->type.number.value;
				result_block = root;				// Ничего не предпринимаем, так как выполняли операцию над переменной
			}
			else if (p->last_block == root)	{	// Запись в аккумулятор (просто убираем блок с символом '=').
				root->next_block = p->next_block;
				p->next_block->last_block = root;
				free(p);
				result_block = root->next_block;
			}

		}
		else {
			p->type.number.value = GetResult(p);
			p->is_number = 1;
			p->is_sign = 0;

			p = UniteBlocks(p);
			result_block = p;
		}
	}

	return result_block;
}
*/

// Функция возвращает "1", если в таблице операций встретился знак "sign". Возвращает "0" в противном случае.
/*int FindInTable(char sign) 
{
	int i;
	char s;		// Знак операции из таблицы

	for (i = 0; (s = table_of_priorities[i].sign) != '\0'; ++i)
		if (s == sign)	
			return 1;
	return 0;
}
*/


// Сортировка  операций по их приоритету и порядку в выражении
void Sort(struct block_of_list *operation[], int left_edge, int right_edge)
{
	int pattern, i;
	struct block_of_list *temp;

	for (pattern = left_edge; pattern < right_edge; ++pattern)
		for (i = pattern + 1; i <= right_edge; ++i) {
			// sorting by priority (here "0" is a highest priority)
			if (GetPriority(operation[pattern]->type.sign.value) > GetPriority(operation[i]->type.sign.value)) {
				temp = operation[pattern];
				operation[pattern] = operation[i];
				operation[i] = temp;
			}
			// Если приоритет один и тот же, то сортируем по порядку следования операции в выражении
			else if (GetPriority(operation[pattern]->type.sign.value) == GetPriority(operation[i]->type.sign.value))
				if (operation[pattern]->type.sign.order > operation[i]->type.sign.order) {
					temp = operation[pattern];
					operation[pattern] = operation[i];
					operation[i] = temp;
				}
		}

	return;
}



/* ExecuteFunction():	get the block with the function and the function's code;
*						return the result of the function execution.
*/ 
float ExecuteFunction(int code, struct block_of_list *p)
{
	struct block_of_list *x				= NULL;
	struct block_of_list *y				= NULL;
	float result;

	switch(code) {
	/*case POW:
		x = ManageBlocks(p->type.function.arg_x);
		y = ManageBlocks(p->type.function.arg_y);
		result = pow(x->type.number.value, y->type.number.value);
		break;
		*/
	case EXP:
		break;
	case SQRT:
		break;
	case SIN:
		break;
	default:
		break;
	}
		
	ReleaseDynamicMem(p->type.function.arg_x);
	ReleaseDynamicMem(p->type.function.arg_y);

	return result;
}