#include <stdio.h>
#include <stdlib.h>

#define VSIZE 18
#define HSIZE 18
#define MINE_CHANCE 10

#define MINE 1 << 6
#define EXPLORED 1 << 5
#define FLAG 1 << 4

#define MINE_CHAR 4
#define BOOM_CHAR 15
#define MASK_CHAR 177
#define CLEARED_CHAR 2
#define FLAG_CHAR 'F'
#define EMPTY_CHAR '-'

#define SET_MINE(r, c) field[(r)][(c)] |= MINE
#define HAS_MINE(r, c) (field[(r)][(c)] & MINE) != 0
#define EXPLORE(r, c) field[(r)][(c)] |= EXPLORED
#define IS_EXPLORED(r, c) (field[(r)][(c)] & EXPLORED) != 0
#define IS_EMPTY(r, c) nearbymines((r), (c)) == 0
#define SET_FLAG(r, c) field[(r)][(c)] |= IS_EXPLORED((r), (c)) ? 0 : FLAG
#define DEL_FLAG(r, c) field[(r)][(c)] &= ~FLAG
#define HAS_FLAG(r, c) (field[(r)][(c)] & FLAG) != 0

#define PRINT_MINE() printf("%-3c", MINE_CHAR)
#define PRINT_NUM_NEARBY_MINES(r, c) printf("%-3d", nearbymines((r), (c)))
#define PRINT_EMPTY() printf("%-3c", EMPTY_CHAR)
#define PRINT_MASK() printf("%c%c%c", MASK_CHAR, MASK_CHAR, MASK_CHAR)
#define PRINT_CLEARED() printf("%-3c", CLEARED_CHAR)
#define PRINT_BOOM() printf("%-3c", BOOM_CHAR)
#define PRINT_FLAG() printf("%-3c", FLAG_CHAR)

char field[VSIZE][HSIZE];

void printfield(void (*f)(int, int)) {
	int r, c;

	for (c = 0; c < HSIZE; c++)
		printf("%-3d", c+1);
	printf("\n");
	for (c = 0; c < HSIZE; c++)
		printf("---", c+1);
	printf(".\n");

	for (r = 0; r < VSIZE; r++) {
		for (c = 0; c < HSIZE; c++)
			f(r, c);
		printf("| %d\n", r+1);
	}
}

void raw(int r, int c) {
	if (HAS_MINE(r, c))
		IS_EXPLORED(r, c) ? PRINT_BOOM() : PRINT_MINE();
	else
		IS_EMPTY(r, c) ? PRINT_EMPTY() : PRINT_NUM_NEARBY_MINES(r, c);
}

void cleared(int r, int c) {
	if (HAS_MINE(r, c))
		PRINT_CLEARED();
	else
		IS_EMPTY(r, c) ? PRINT_EMPTY() : PRINT_NUM_NEARBY_MINES(r, c);
}

void mystery(int r, int c) {
	if (IS_EXPLORED(r, c))
		IS_EMPTY(r, c) ? PRINT_EMPTY() : PRINT_NUM_NEARBY_MINES(r, c);
	else
		HAS_FLAG(r, c) ? PRINT_FLAG() : PRINT_MASK();
}

void genmines() {
	int r, c;

	srand(time(NULL));

	for (r = 0; r < VSIZE; r++)
		for (c = 0; c < HSIZE; c++)
			if (rand() % MINE_CHANCE == 0)
				SET_MINE(r, c);
}

int eachnear(int (*f)(int, int), int r, int c) {
	int counter = 0;

	// left
	if (c != 0)
		counter += f(r, c - 1);
	// up
	if (r != 0)
		counter += f(r - 1, c);
	// right
	if (c != HSIZE - 1)
		counter += f(r, c + 1);
	// down
	if (r != VSIZE - 1)
		counter += f(r + 1, c);
	// top left
	if (r != 0 && c != 0)
		counter += f(r - 1, c - 1);
	// top right
	if (r != 0 && c != HSIZE - 1)
		counter += f(r - 1, c + 1);
	// bottom left
	if (r != VSIZE - 1 && c != 0)
		counter += f(r + 1, c - 1);
	// bottom right
	if (r != VSIZE - 1 && c != HSIZE - 1)
		counter += f(r + 1, c + 1);

	return counter;
}

int hasmine(int r, int c) {
	return HAS_MINE(r, c);
}

int nearbymines(int r, int c) {
	return eachnear(hasmine, r, c);
}

int explore(int r, int c) {
	if (IS_EXPLORED(r, c))
		return 1;

	if (HAS_FLAG(r, c))
		return 2;

	EXPLORE(r, c);

	if (HAS_MINE(r, c))
		return 0;
	else if (IS_EMPTY(r, c))
		eachnear(explore, r, c);

	return 1;
}

int done() {
	int r, c;

	for (r = 0; r < VSIZE; r++)
		for (c = 0; c < HSIZE; c++)
			if (! IS_EXPLORED(r, c) && ! HAS_MINE(r, c))
				return 0;

	return 1;
}

void getrc(int *r, int *c) {
	int ok ;

	do {
		printf("Enter row and column: ");
		scanf("%d %d", r, c);

		--*r;
		--*c;

		ok = 1;
		ok &= (*r >= 0 && *r < VSIZE);
		ok &= (*c >= 0 && *c < HSIZE);
	} while (!ok);
}

int main() {
	int action, r, c, cont = 1;

	genmines();

	do {
		system("cls");

		if (cont == 2)
			printf("Can't explore a flagged spot!\n");

		printfield(mystery);

		printf("Enter 1 to explore\n      2 to flag\n      3 to delete flag\n");
		scanf("%d", &action);

		getrc(&r, &c);

		if (action == 1)
			cont = explore(r, c);
		else if (action == 2)
			SET_FLAG(r, c);
		else if (action == 3)
			DEL_FLAG(r, c);

		if (done())
			cont = -1;
	} while (cont > 0);

	system("cls");
	cont ? printfield(cleared) : printfield(raw);
	return cont ? 0 : 1;
}

