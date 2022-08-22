#include "header.h"

#define NAME_SIZE 20

extern float gl_rgstr[];
char *gl__line_position; // Need to store position in the line, which is passed in the function (and we NEED to save the place where the function finished its work)

// Array of the available text-commands and their codes
struct components_of_command
{
	char *name;
	int code;
} command[] = {
	"HELP", 		HELP,
	"PUSH",			PUSH,
	"POP",			POP,
	"SWAP",			SWAP,
	"TOP",			TOP,
	"PRINT_STACK",	PRINT_STACK,
	"CLEAR",		CLEAR,
	"PRINT_VAR",	PRINT_VAR,
	"",				0,
};

struct components_of_function
{
	char *name;
	int number_of_arguments;
	int code;
} function[] = {
	"sin",		1,	SIN,
	"cos",		1,	COS,
	"exp",		1,	EXP,
	"sqrt",		1,	SQRT,
	"pow",		2,	POW,
	"last_f", 	0,	LAST_FUNCTION,
	"",			0,	0,
};

// table of operation priorities
struct {
	int weight;
	char sign;
} table_of_priorities[] = {
	-2,	'*',
	-2,	'/',
	-2,	'%',
	-3,	'+',
	-3,	'-',
	-13,	'=',
	-14, '(',

	
	0, '\0',
};

/* CreateList():	Creates the doubly linked list, each block of which contains ...
*/
int CreateList(struct block_of_list *root, char *line)
{
	float number = 0.0;
	int code = 0;

	struct block_of_list *block = root->next_block;
	struct block_of_list *last_block = root;			

	while (*line != '\0')
	{

		// Add a new block and set links between it and the previous one
		if (block == NULL) {
			block = AddBlock();
			block->last_block = last_block;
			if (last_block != NULL) {
				last_block->next_block = block;
			}
		}
		else {
			;
		}

		// Skip white spaces and exit if meets the end of the line
		while (*line == ' ' || *line == '\t')
		{
			++line;
			if (*line == '\0')
				break;
		}

		// Store number to the current the block
		if (mcr__IsDigit(*line))
		{
			number = ReadNumber(line);
			line = gl__line_position;
			block->is_number = 1;
			block->type.number.value = number;
		}
		// Store sign to the current block
		else if (mcr__IsSign(*line))
		{
			block->is_sign = 1;
			if (mcr__IsSpecial(*line))
				block->is_special = 1;
			block->type.sign.value = *line;
			block->type.sign.priority = GetPriority(*line);
			if (*line == '\n')
				break;
			++line;
		}
		// Store function / command / or variable
		else if (mcr__IsLetter(*line) || *line == '_')
		{
			code = ReadName(line);
			line = gl__line_position;
			// Fit the current block as block with number ('cause variable contains number)
			if (mcr__LowerCase('a' + code - 1))		// ## Replace to just 'code', 'cause a little unclear
			{
				block->is_variable = 1;
				block->is_number = 1;
				block->type.number.value = gl_rgstr[code]; // gl_rgstr[] contains variables (exactly their values)
				block->type.number.variable = 'a' + code - 1;
			}
			// Fit the current block as block with command
			else if (mcr__IsCommand(code))
			{
				block->is_command = 1;
				block->type.command.code = code;
			}
			// Fit the current block as block with function
			else if (mcr__IsFunction(code))
			{
				block->is_function = 1;
				block->type.function.code = code;
			}
		}
		// Met unknown symbol
		else
		{
			printf("\n\n>TYPE: error\n>SOURCE: CreateList()\n>MESSAGE: Unknown symbol: \"%c\"", *line);
			return -1;
		}

		// Move to the next block (which isn't exist yet, just NULL-pointer)
		last_block = block;
		block = block->next_block;
	}

	return 0;
}




/* AddBlock():	allocates memory in the heap for the new block of the list.
*/
struct block_of_list *AddBlock()
{
	struct block_of_list *new_block;

	new_block = (struct block_of_list *)malloc(sizeof(struct block_of_list));
	new_block->last_block = NULL;
	new_block->next_block = NULL;

	new_block->is_number = 0;
	new_block->is_variable = 0;
	new_block->is_sign = 0;
	new_block->is_special = 0;
	new_block->is_function = 0;
	new_block->is_command = 0;

	new_block->type.number.value = 0.0;
	new_block->type.number.variable = '\0';

	return new_block;
}

/* ReadName():	read the set of symbols from the line and return "code" of their meaning;
*				return "-1" if symbols mean nothing.
*/
int ReadName(char *line)
{
	int i, k;
	char name[NAME_SIZE];

	// Just read the current name, that is contained in the line
	for (i = 0; *line == '_' || mcr__IsLetter(*line); ++i)
	{
		if (i == NAME_SIZE - 1) {
			printf("\n>TYPE: error\n>SOURCE: ReadName()\n>MESSAGE: name is too long");
			return -1;
		}
		name[i] = *line;
		++line;
	}
	name[i] = '\0';
	gl__line_position = line;

	// If we meet a name of variable, then will return variable's code ('a' = 1 ... 'z' = 26)
	if ((name[0] >= 'a' && name[0] <= 'z') && name[1] == '\0')
		return (name[0] - 'a' + 1);
	// If we meet a command or a function
	else {
		for (k = 0; *command[k].name != '\0'; ++k)
		{
			if (CompareStrings(name, command[k].name, 1) == 1)
				return command[k].code;
		}
		for (k = 0; *function[k].name != '\0'; ++k)
		{
			if (CompareStrings(name, function[k].name, 1) == 1)
				return function[k].code;
		}
	}

	// If the name is unknown
	printf("\n>TYPE: error\n>SOURCE: ReadName()\n>MESSAGE: Unknown name: \"%s\"", name);
	return -1;
}

/* ReadNumber():	reads the number from the "line" and returns it.
*/
float ReadNumber(char *line)
{
	char c;
	int divisor = 0; // need for non-integer numbers
	float number = 0.0;

	c = *line;
	while (mcr__IsDigit(c))
	{
		number = number * 10 + (c - '0');
		divisor = divisor * 10;
		c = *++line;
		// if non-integer number
		if ((c == '.' || c == ',') && divisor == 0)
		{
			divisor = 1;
			c = *++line;
		}
	}
	gl__line_position = line; // Here we store current position in the line

	return (divisor == 0) ? number : number / divisor;
}

/* GetPriority():	gets the "sign" and returns the sign's priority;
*					"0" is the highest priority, then 1,2,3...13,14... etc;
*					returns "-1" if there is not current sign in the table-of-priorities
*/
int GetPriority(char sign)
{
	int i;

	for (i = 0; table_of_priorities[i].weight != 0; ++i)
		if (sign == table_of_priorities[i].sign)
			return table_of_priorities[i].weight;

	return 0;
}

// Р’С‹РІРѕРґ СЃРѕРѕР±С‰РµРЅРёСЏ
//void PrintMessage(char *source, char *type, char *message)
//{
//	printf("\n> SOURCE:	\"%s\"", source);
//	printf("\n>   TYPE:	\"%s\"", type);
//	printf("\n>MESSAGE:	\"%s\"", message);
//
//	return;
//}

// ReadNumber(): РІРµСЂСЃРёСЏ СЃ С‡С‚РµРЅРёРµРј РїРѕ С‡Р°СЃС‚СЏРј
/*float ReadNumber(char *last_c)
{
	int		integer			=	0;
	float	fractional		=	0.0;
	int		d				=	0;
	char	part			=	'i';
	char	c				=	*last_c;

	do {
		switch(part) {
		case 'i':
			integer = integer * 10 + (c - '0');
			c = getchar();
			if (c == '.' || c == ',') {
				c = getchar();
				part = 'f';
			}
			break;
		case 'f':
			fractional = fractional * 10 + (c - '0');
			d = d * 10;
			c = getchar();
			break;
		default:
			break;
		}
	} while (MyIsdigit(c));
	*last_c = c;
	fractional /= d;

	return integer + fractional;
}*/

// Р¤СѓРЅРєС†РёСЏ СЃС‡РёС‚С‹РІР°РµС‚ С‡РёСЃР»Рѕ РёР· РїРѕС‚РѕРєР° РІРІРѕРґР° Рё РІРѕР·РІСЂР°С‰Р°РµС‚ РµРіРѕ. РџРѕСЃР»РµРґРЅРёР№ СЃС‡РёС‚Р°РЅРЅС‹Р№ СЃРёРјРІРѕР», РЅРµ СЏРІР»СЏСЋС‰РёР№СЃСЏ С†РёС„СЂРѕР№, Р·Р°РїРёСЃС‹РІР°РµС‚СЃСЏ РїРѕ
// Р°РґСЂРµСЃСѓ last_c. РџСЂРё РѕС‡РµРЅСЊ РґР»РёРЅРЅС‹С… С‡РёСЃР»Р°С… РµСЃС‚СЊ РІРµСЂРѕСЏС‚РЅРѕСЃС‚СЊ РїРµСЂРµРїРѕР»РЅРµРЅРёСЏ. Р’РѕР·РјРѕР¶РЅРѕ, РІ С‚Р°РєРѕРј СЃР»СѓС‡Р°Рµ Р»СѓС‡С€Рµ РѕС‚РґРµР»СЊРЅРѕ СЃС‡РёС‚Р°С‚СЊ С†РµР»СѓСЋ
// Рё РґСЂРѕР±РЅСѓСЋ С‡Р°СЃС‚Рё (СЃРј. РІРµСЂСЃРёСЋ РІ РєРѕРЅС†Рµ).
//float ReadNumber(char **line)
//{
//	char c;
//	float number = 0.0;
//	int divisor = 0;
//
//	c = *(*line)++;
//	do {
//		number = number * 10 + (c - '0');
//		divisor = divisor * 10;
//		c = *(*line)++;
//		// РќР°С‡РёРЅР°РµРј РЅР°РєР°РїР»РёРІР°С‚СЊ РґСЂРѕР±РЅСѓСЋ С‡Р°СЃС‚СЊ
//		if (c == '.' || c == ',') {
//			divisor = (divisor == 0) ? 1 : divisor;
//			c = *(*line)++;
//		}
//	} while (IsDigit_m(c));
//	(*line)--;										// РўР°Рє РєР°Рє СѓР±РµР¶Р°Р»Рё РЅР° РѕРґРёРЅ СЃРёРјРІРѕР» РІРїРµСЂС‘Рґ
//
//	return (divisor == 0) ? number : number / divisor;
//}

//int FillTheList(struct block_of_list *root, char *line)
//{
//	int		i;
//	char	c;
//	char	*name;							// РџСЂРѕСЃС‚Рѕ РІСЃРїРѕРјРѕРіР°С‚РµР»СЊРЅР°СЏ РїРµСЂРµРјРµРЅРЅР°СЏ
//
//	int was_function = 0;
//
//	int		order	=	0;					// РџРѕСЂСЏРґРєРѕРІС‹Р№ РЅРѕРјРµСЂ Р°СЂРёС„РјРµС‚РёС‡РµСЃРєРѕР№ РѕРїРµСЂР°С†РёРё РІ РІС‹СЂР°Р¶РµРЅРёРё
//	float	number	=	0.0;
//
//
//	char	func[MAX_WORD];				// РЎСЋРґР° Р·Р°РїРёСЃС‹РІР°РµС‚СЃСЏ РёРјСЏ РёР· РІС‹СЂР°Р¶РµРЅРёСЏ Рё РїСЂРѕРІРµСЂСЏРµС‚СЃСЏ Р·Р°С‚РµРј, СЏРІР»СЏРµС‚СЃСЏ Р»Рё РёРјСЏ - РёРјРµРЅРµРј С„-С†РёРё.
//	int		arg = 1;					// РљРѕР»-РІРѕ Р°СЂРіСѓРјРµРЅС‚РѕРІ С„-С†РёРё (1 РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ)
//	char	x[SIZE_of_LINE];
//	char	y[SIZE_of_LINE];
//
//	struct block_of_list	*block			=	root->next_block;
//	struct block_of_list	*last_block		=	root;
//
//
//
//	while ((c = *line++) != '\0') {
//		number = 0.0;
//
//		//######## Р”РѕР±Р°РІР»СЏРµРј РЅРѕРІС‹Р№ Р±Р»РѕРє РІ СЃРїРёСЃРѕРє
//		if (block == NULL)
//			block = AddBlock();
//		block->last_block = last_block;
//		if (last_block != NULL)
//			last_block->next_block = block;
//
//		// РџСЂРѕРїСѓСЃРє СЃРёРјРІРѕР»РѕРІ РїСѓСЃС‚РѕРіРѕ РїСЂРѕСЃС‚СЂР°РЅСЃС‚РІР°
//		while (c == ' ' || c == '\t') {
//			c = *line++;
//			if (c == '\0')
//				break;
//		}
//
//		 //######## РЎС‡РёС‚С‹РІР°СЋ С‡РёСЃР»Рѕ
//		if (IsDigit_m(c)) {
//			--line;								// Р’РѕР·РІСЂР°С‰Р°РµРјСЃСЏ РЅР° РѕРґРёРЅ СЃРёРјРІРѕР» РЅР°Р·Р°Рґ
//			line += ReadNumber(line, &number);
//
//			//#### Р—Р°РїРёСЃС‹РІР°СЋ С‡РёСЃР»Рѕ РІ Р±Р»РѕРє
//			block->is_number = 1;
//			block->type.number.value = number;
//		}
//		//######## РЎС‡РёС‚С‹РІР°СЋ Р·РЅР°Рє РѕРїРµСЂР°С†РёРё
//		else if (IsSign_m(c)) {
//			block->is_sign = 1;
//			block->type.sign.value = c;
//			block->type.sign.order = ++order;
//		}
//		//######## РЎС‡РёС‚С‹РІР°СЋ С„СѓРЅРєС†РёСЋ РёР»Рё РїРµСЂРµРјРµРЅРЅСѓСЋ
//		else if (IsLetter_m(c) || c == '_') {
//			// Р—Р°РїРёСЃСЊ РёРјРµРЅРё
//			for (i = 0; IsLetter_m(c) || c == '_'; ++i) {
//				 func[i] = c;
//				 c = *line++;
//			 }
//			 func[i] = '\0';
//
//			 //	Р•СЃР»Рё РїРµСЂРµРјРµРЅРЅР°СЏ, С‚Рѕ Р·Р°РїРёСЃС‹РІР°РµРј РµС‘ Р·РЅР°С‡РµРЅРёРµ РІ Р±Р»РѕРє
//			 if (i == 1 && 'a' <= func[0] && func[0] <= 'z') {
//				 block->is_number = 1;
//				 block->is_variable = 1;
//				 block->type.number.value = gl_rgstr[func[0] - 'a' + 1];
//				 block->type.number.var = func[0];
//				 --line;					// РљРѕСЃС‚С‹Р»СЊ, С‚Р°Рє РєР°Рє СѓР±РµР¶Р°Р»Рё РЅР° РѕРґРёРЅ СЃРёРјРІРѕР» РІРїРµСЂРµРґ РІС‹С€Рµ
//			 }
//			 // Р•СЃР»Рё С„СѓРЅРєС†РёСЏ, С‚Рѕ Р·Р°РїРёСЃС‹РІР°СЋ РёРјСЏ С„СѓРЅРєС†РёРё РІ Р±Р»РѕРє
//			 else {
//				 for (i = 0; *(name = type_of_function[i].function) != '\0'; ++i) {
//					 if (CompareStrings(func, name, 1) == 1) {
//						 block->is_function = 1;
//						 CopyString(block->type.function.name, func, 0);
//						 was_function = 1;
//						 break;
//					 }
//				 }
//				 if (*name == '\0') {
//					 printf("\n>TYPE: error\n>SOURCE: FillTheList()\n>MESSAGE: unknown name of function \"%s\"", func);
//					 return 1;
//				 }
//			 }
//
//			 if (was_function == 1) {
//
//				 // РџСЂРѕРїСѓСЃРє РїСЂРѕР±РµР»РѕРІ
//				 while (c == ' ' || c == '\t')
//					 c = *line++;
//
//				 //#### Р—Р°РїРёСЃСЊ РІ РјР°СЃСЃРёРІС‹ x[] Рё y[] Р·РЅР°С‡РµРЅРёСЏ Р°СЂРіСѓРјРµРЅС‚РѕРІ С„СѓРЅРєС†РёРё (РїРѕРєР° РІ РІРёРґРµ СЃС‚СЂРѕРєРё)
//				 if (c == '(') {
//					 c = *line++;
//					 for (i = 0, arg = 1; c != ')' ; ++i) {
//						 switch(arg) {
//						 case 1:
//							 x[i] = c;
//							 break;
//						 case 2:
//							 y[i] = c;
//							 break;
//						 default:
//							 break;
//						 }
//						 c = *line++;
//						 if (c == ';') {
//							 x[i + 1] = '\0';
//							 arg = 2;
//							 i = -1;
//							 c = *line++;
//						 }
//					 }
//					 if (arg == 1)
//						 x[i] = '\0';
//					 else if (arg == 2)
//						 y[i] = '\0';
//				 }
//				 //#### Р’С‹РґРµР»СЏРµРј Р±Р»РѕРєРё РґРёРЅ. РїР°РјСЏС‚Рё РґР»СЏ Р°СЂРіСѓРјРµРЅС‚РѕРІ С„СѓРЅРєС†РёРё Рё С†РµРїР»СЏРµРј С‚СѓРґР° СЌС‚Рё Р°СЂРіСѓРјРµРЅС‚С‹.
//				 block->type.function.x = AddBlock();
//				 FillTheList(block->type.function.x, x);
//				 if (arg == 2) {
//					 block->type.function.y = AddBlock();
//					 FillTheList(block->type.function.y, y);
//				 }
//			 }
//		}
//
//		//######## РЎС‡РёС‚С‹РІР°СЋ СЃРїРµС†РёР°Р»СЊРЅС‹Р№ Р·РЅР°Рє
//		else {
//			block->type.special.value = c;
//			block->is_special = 1;
//			if (c == '\n')
//				break;
//		}
//
//		/*	РќРёР¶Рµ С‚РѕРЅРєРёР№ РјРѕРјРµРЅС‚. РџРѕСЃР»Рµ РІС‹РїРѕР»РЅРµРЅРёСЏ РїРµСЂРІРѕР№ СЃС‚СЂРѕС‡РєРё, СѓРєР°Р·Р°С‚РµР»СЊ "last_block" Р±СѓРґРµС‚ СЃРѕРґРµСЂР¶Р°С‚СЊ Р°РґСЂРµСЃ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ
//		 * СЌР»-С‚Р° "block" РІ РґР°РЅРЅС‹Р№ РјРѕРјРµРЅС‚. РџРѕСЃР»Рµ Р¶Рµ РІС‹РїРѕР»РЅРµРЅРёСЏ РІС‚РѕСЂРѕР№ СЃС‚СЂРѕС‡РєРё, "block" СѓР¶Рµ Р±СѓРґРµС‚ СЃРѕРґРµСЂР¶Р°С‚СЊ Р°РґСЂРµСЃ
//		 * СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ СЌР»-С‚Р° "block->next_block". Рђ "block->next_block" С‚РµРїРµСЂСЊ Р±СѓРґРµС‚ СѓРєР°Р·С‹РІР°С‚СЊ СЃР°Рј РЅР° СЃРµР±СЏ.
//		 * Рђ РІРѕС‚ "last_block" С‚РµРїРµСЂСЊ Р±СѓРґРµС‚ СѓРєР°Р·С‹РІР°С‚СЊ РќР• РЅР° С‚РµРєСѓС‰РёР№ Р°РґСЂРµСЃ "block", Р° РЅР° СЃС‚Р°СЂС‹Р№ Р°РґСЂРµСЃ.
//		 */
//		last_block = block;
//		block = block->next_block;
//	}
//
//	return 0;
//}

//int CreateFunctionBlock (char *line, struct block_of_list *block, int code)
//{
//	block->is_function = 1;
//	block->type.function.code = code;
//
//	// РџСЂРѕРїСѓСЃРє РїСЂРѕР±РµР»РѕРІ
//	while (*line == ' ' || *line == '\t')
//		line++;
//
//	//#### Р—Р°РїРёСЃСЊ РІ РјР°СЃСЃРёРІС‹ x[] Рё y[] Р·РЅР°С‡РµРЅРёСЏ Р°СЂРіСѓРјРµРЅС‚РѕРІ С„СѓРЅРєС†РёРё (РїРѕРєР° РІ РІРёРґРµ СЃС‚СЂРѕРєРё)
//	if (*line == '(') {
//		line++;
//		if (CreateList(block->type.function.x, line, 1) == 1) {
//			line = current_line;
//			CreateList(block->type.function.y, line, 1);
//			line = current_line;
//		}
//	}
//
//	return 0;
//}
