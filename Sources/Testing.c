#include "header.h"

static struct components_of_test {
	char *expression;
	float right_result;
} test[] = {
	//"",															0.0,		// End of tests
	"3.475-4.0*1.0+6.0",										5.475f,
	"4+2*(7-2*3)",												6.0,
	"(1+2)",													3.0,
	"(1+2*3)",													7.0,
	"((1+2))+3",												6.0,
	"7,58 - 9,21 * (5+2)",										-56.89f,
	"((1.2 + 2.5 * (3.75 - 4.60 * 1 + 1.0) / 0.15 - 1.7))",		2.0,
	"17 % 5",													2.0,
	"((1+2*(3-4*1+6)/2-5))",									1.0,
	//"pow(2;3)",													8.0,
	//"pow(5 + 4; 3 - 1)",										81.0,
	//"1 + sin(5 + pow(2; 3))",									1.4202,
	"(((1+2*((3+4)-5))))",										5.0,
	"0",														0.0,


	// Last element have to be empty
	// to correct finish the tests
	"",															0.0,
};


// Функция возвращает true, если есть тестовая строка и false в противном случае.
enum boolean GetTestLine(char *line, int limit)
{
	static int i = 0;					// Number of the test expression

	// Have a test string to return
	if (*test[i].expression != '\0') {
		if (i == 0) {
			printf("\nStart of the testing:");
		}

		CopyString(line, test[i].expression, SIZE_of_LINE);

		printf("\n\n     Expression:  %-15s", test[i].expression);
		printf("\n   Right result:  %-15f", test[i].right_result);
		printf("\n*********************************************");

		++i;
		return true;

	}
	// Test strings ended up
	else if (i != 0) {
		printf("\n\n>End of the testing");
		return false;
	}
	// Don't have test strings at all
	else {
		printf("\n\n>TYPE: message\n>SOURCE: GetTestLine()\n>MESSAGE: testing expressions are not provided");
		return false;
	}
}