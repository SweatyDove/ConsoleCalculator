#include "header.h"


int CheckList(struct block_of_list *root)
{
	struct block_of_list *block = NULL;
	int parenthesis = 0;

	for (block = root->next_block; block != NULL; block = block->next_block) {
		if (block->is_sign == 1) {
			switch(block->type.sign.value) {
			case '(':
				++parenthesis;
				break;
			case ')':
				if (--parenthesis < 0) {
					printf ("\n\n>TYPE: error\n>SOURCE: CheckList()\n>MESSAGE: parenthesis balanced incorrectly");
					return -1;
				}
				break;
			case '\n':
				if (block == root->next_block) {
					printf ("\n\n>TYPE: warning\n>SOURCE: CheckList()\n>MESSAGE: empty string");
					return -2;
				}
				break;
			// Other problems with "signs"
			default:
				break;
			}	
		}
		// Problems with other types of block
		else {
			;
		}
	}
	if (parenthesis != 0) {
		printf ("\n\n>TYPE: error\n>SOURCE: CheckList()\n>MESSAGE: parenthesis balanced incorrectly");
		return -1;
	}


	return 0;
}