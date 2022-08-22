#include "header.h"

#define SIZE_of_COMMAND	20
char command_word[SIZE_of_COMMAND];			// Команда, записанная в текущей строке

//char var[] = { "a" };					// Вспомогательная строка из одного символа
char variable;
extern float gl_rgstr[];


float stack[SIZE_of_STACK];
int sp;


#define LENGTH_OF_NUMBER	20
char current_number[LENGTH_OF_NUMBER];



/* SearchCommand():	Функция проверяет строку "line" на наличие команды. 
 *					Если команда найдена - возвращается её код. 
 *					Если в команде ошибка, возвращается "-1".
 *					Если НЕ команда, возвращается "0".
 */
//int SearchCommand(char *line) 
//{
//	int i = 0;
//	char c;
//	char *name;
//
//	// Пропуск пробелов
//	c = *line++;
//	while (IsWhiteSpace_m(c))
//		c = *line++;
//
//	// Запись командного слова
//	if (IsLetter_m(c) || c == '_') {
//		while (IsLetter_m(c) || c == '_') {
//			command_word[i++] = c;
//			if (i >= SIZE_of_COMMAND - 1) {
//				printf("\n>TYPE: error\n>SOURCE: ReadCommand()\n>MESSAGE: the command is too long");
//				return -1;
//			}
//			c = *line++;
//		}
//		command_word[i] = '\0';
//	}
//	// Не команда
//	else
//		return 0;
//
//	//######## Если встретилось командное слово, то нужно определить, является ли оно КОМАНДОЙ
//	while (IsWhiteSpace_m(c))
//		c = *line++;
//	
//	if (c == '\n') {
//		for (i = 0; *(name = available_commands[i].name) != '\0'; ++i) {
//			if (CompareStrings(command_word, name, 1))
//				return available_commands[i].code;
//		}
//		printf("\n>TYPE: warning\n>SOURCE: FindCommand()\n>MESSAGE: The command \"%s\" wasn't found in the list of the stack's commands", command_word);
//		return -1;
//	}
//	//else
//		// printf() - тут надо бы вывести в поток сообщений, что "не команда так как не заканчивается символом '\n'"
//	return 0;
//}


// Функция получает код команды и исполняет её.
void ExecuteCommand(int code)
{
	int i;
	//char c;
	//char *str;
	float temp;

	//########
	switch (code) {
	//#### Вывести список команд
	case HELP:
		printf("\nThe list of the available commands:"
				"\n\tPUSH\t- put the content of the accumulator in the stack"
				"\n\tPOP\t- put the top element of the stack in the accumulator"
				"\n\tSWAP\t- swap the two top elements of the stack"
				"\n\tTOP\t- print the top element of the stack"
				"\n\tPRINT_STACK\t- print the entire content of the stack"
				"\n\tCLEAR\t- clear the entire content of the stack"
				"\n\t\"var\"\t - print the value of the variable \"var\""
				"\n\t\"var=\"\t - write the value from the accumulator in the variable \"var\""
				"\n\tPRINT_VAR\t- print the values of the all variables");
				//"\n\tINF\t- get an information about the programm"
		break;

	//#### Операции над стеком
	//#### Поместить содержимое аккумулятора в стек
	case PUSH:
		stack[++sp] = gl_rgstr[0];
		printf(">> Operation completed <<");
		break;
	//#### Поместить содержимое стека в аккумулятор
	case POP:
		gl_rgstr[0] = stack[sp--];
		printf(">> Operation completed <<");
		break;
	//#### Обменять местами верхние эл-ты стека
	case SWAP:
		temp = stack[sp];
		stack[sp] = stack[sp - 1];
		stack[sp - 1] = temp;
		printf(">> Operation completed <<");
		break;
	//#### Очистить стек
	case CLEAR:
		while (sp)
			stack[sp--] = 0.0;
		printf(">> Operation completed <<");
		break;
	//#### Вывести содержимое стека
	case PRINT_STACK:
		printf("\n\n");
		if (sp == 0)
			printf("\t\t##\tstack is empty\t##\n");
		for (i = sp; i > 0; --i)
			printf("\t\t##\t%10f\t##\n", stack[i]);
		printf("\n\n>> Operation completed <<");
		break;
	//#### Вывести верхний эл-т стека без его удаления
	case TOP:
		printf("\n%10f", stack[sp]);
		printf(">> Operation completed <<");
		break;



	//#### Вывести содержимое всех переменных
	case PRINT_VAR:
		for (i = 1; i <= NUM_of_REGISTERS; ++i)
			printf("\n\t%c = %f", ('a' + i - 1), gl_rgstr[i]);
		break;

	//#### 
	default:
		break;
	}
}



//######## Если нужно вывести значение переменной
	//else if (c == '\n' && i == 1) {
	//	if (IsLetter(*command))
	//		return 100 + (ToLow(*command) - 'a' + 1);	// Возвращаем код от 101 до 126
	//	else {
	//		printf("\n>TYPE: warning\n>SOURCE: FindCommand()\n>MESSAGE: The \"%c\" wasn't found in the list of the affordable variables", *command);
	//		return -1;
	//	}
	//}

	//######## Если команда присваивания, то возвращаем код от 1 до 26, соответствующий переменной, в которую надо записать значение из аккумулятора
	//else if (c == '=') {
	//	for (i = 'a'; i <= 'z'; ++i) {
	//		*var = i;								// Заменяем символ строкой из этого символа
	//		if (CompareStrings(command, var, 1))
	//			return i - 'a' + 1;
	//	}
	//	printf("\n>TYPE: warning\n>SOURCE: FindCommand()\n>MESSAGE: The \"%s\" wasn't found in the list of the affordable variables", command);
	//	return -1;
	//}




////######## Если встретился знак операции, а команда состоит из одного символа латинского алфавита
//	else if ((SignOfOperation(c) || c == '=' || c == '\n') && IsLetter(command[0]) && i == 1 && first_sign == '\0') {
//		switch(c) {
//		case '+':
//			return PLUS;
//			break;
//		case '-':
//			return MINUS;
//			break;
//		case '*':
//			return MULTIPLICATION;
//			break;
//		case '/':
//			return DIVISION;
//			break;
//		case '%':
//			return RESIDUE;
//			break;
//		case '=':
//			return OUTPUT;
//		case '\n':
//			printf("\n>TYPE: warning\n>SOURCE: FindCommand()\n>MESSAGE: You didn't point the sign of the wishful operation", *command);
//			return -1;
//			break;
//		default:
//			printf("\n>TYPE: warning\n>SOURCE: FindCommand()\n>MESSAGE: Unexpected error. Please, enter \"HELP\" for help", *command);
//			return -1;
//			break;
//		}
//	}


//// Алгебраические операции между аккумулятором и переменными
//	else if (first_sign != '\0' && IsLetter(command[0]) && i == 1) {
//		switch(first_sign) {
//		case '+':
//			return TO_ACC + PLUS;
//			break;
//		case '-':
//			return TO_ACC + MINUS;
//			break;
//		case '*':
//			return TO_ACC + MULTIPLICATION;
//			break;
//		case '/':
//			return TO_ACC + DIVISION;
//			break;
//		case '%':
//			return TO_ACC + RESIDUE;
//			break;
//		case '=':
//			return TO_ACC + INPUT;
//			break;
//		default:
//			printf("\n>TYPE: warning\n>SOURCE: FindCommand()\n>MESSAGE: Unexpected error. Please, enter \"HELP\" for help", *command);
//			return -1;
//			break;
//		}
//	}


//#### Операции над переменными
	//case PLUS:
	//	rgstr[command[0] - 'a'] += rgstr[0];
	//	printf(">> Operation completed <<");
	//	break;
	//case MINUS:
	//	rgstr[command[0] - 'a'] -= rgstr[0];
	//	printf(">> Operation completed <<");
	//	break;
	//case MULTIPLICATION:
	//	rgstr[command[0] - 'a'] *= rgstr[0];
	//	printf(">> Operation completed <<");
	//	break;
	//case DIVISION:
	//	rgstr[command[0] - 'a'] /= rgstr[0];
	//	printf(">> Operation completed <<");
	//	break;
	///*case RESIDUE:
	//	rgstr[command[0] - 'a'] = (int) rgstr[command[0] - 'a'];
	//	rgstr[command[0] - 'a'] %= (int) rgstr[0];
	//	printf(">> Operation completed <<");
	//	break;*/
	// Вывод содержимого аккумулятора
	/*case OUTPUT: 
		printf("\n>%c = %10f\n", command[0], rgstr[command[0] - 'a']);
		printf(">> Operation completed <<");
		break;*/


//#### Операции над аккумулятором
	//case TO_ACC + PLUS:
	//	rgstr[0] += rgstr[command[0] - 'a'];
	//	printf(">> Operation completed <<");
	//	break;
	//case TO_ACC + MINUS:
	//	rgstr[0] -= rgstr[command[0] - 'a'];
	//	printf(">> Operation completed <<");
	//	break;
	//case TO_ACC + MULTIPLICATION:
	//	rgstr[0] *= rgstr[command[0] - 'a'];
	//	printf(">> Operation completed <<");
	//	break;
	//case TO_ACC + DIVISION:
	//	rgstr[0] /= rgstr[command[0] - 'a'];
	//	printf(">> Operation completed <<");
	//	break;
	///*case TO_ACC + RESIDUE:
	//	rgstr[0] %= rgstr[command[0] - 'a'];
	//	printf(">> Operation completed <<");
	//	break;*/
	//case TO_ACC + INPUT:
	//	rgstr[0] = rgstr[command[0] - 'a'];
	//	printf(">> Operation completed <<");
	//	break;