/*
 Paz Cohen
/4/18
 This code receives a sudoku board, finds the cell with minimum options, if there is one option, the board is automatically updated.
 If there is no cell with 1 option, it asks the player what number he wishes to update the cell.
 If the boards is completed, a message will appear, if there were wrong choices, the code will detect and update the user for duplications.
 */
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef struct _Array {
	short* arr;
	unsigned short size;
}Array;

int FillBoard(short** board, Array ***possibilities);
Array ***PossibleDigits(short *sudokuBoard);
short* RowOpt(short* sBoard, int row);//make an "bucket array" saving the numbers in every row
short* ColOpt(short* sBoard, int col);//make an "bucket array" saving the numbers in every col				 |0|1|2|
short* x3x3Opt(short*sBoard, int row, int col);//make an "bucket array" saving the numbers in every 3*3 cube |3|4|5| for example: for cell[8][4] fuction calculates the option for (3*3) cube number 7
int find3x3(int i, int j);//receives a row and col and determines which one of the 9 (3*3) cube it appears	 |6|7|8|
Array* uniteOpt(short* rowOpt, short* colOpt, short* x3Opt);//receives row/col/3*3 buckets for each and every parallel cube to unite the possible options for this cube
void sudokoPrintBoard(short *sBoard);
void reCalc(short * board, Array ***possibilities);//Once a filling is made, the board all options need to be calculated again to avoid duplications.
void main()
{
	short board[81] = { 5,-1,4,-1,7,-1,-1,1,-1,6,-1,2,1,-1,-1,3,-1,-1,1,-1,8,-1,4,-1,-1,6,-1,-1,5,-1,-1,6,-1,-1,2,-1,-1,2,-1,8,-1,3,-1,-1,-1,-1,-1,-1,-1,-1,4,-1,5,6,-1,6,1,5,3,7,2,8,4,-1,8,7,-1,1,9,-1,3,-1,-1,-1,-1,2,8,-1,-1,-1,9 };
	short * sudoko_board = board;
	Array*** possible_solutions;

	printf(" Initial board\n");
	sudokoPrintBoard(sudoko_board);

	printf("Press enter to start playing...\n");
	getchar();

	possible_solutions = PossibleDigits(sudoko_board);

	if (FillBoard(&sudoko_board, possible_solutions) == -1)
		printf("User's selections led to duplications\n");
	else
	{
		sudokoPrintBoard(sudoko_board);
		printf("Board filled successfully\n");
	}

	int k, s;
	for (k = 0; k < 9; k++)
		for (s = 0; s < 9; s++)
		{
			free(possible_solutions[k][s]);
		}
	for (k = 0; k < 9; k++)
		free(possible_solutions[k]);
	free(possible_solutions);
	printf("Press enter to exit.\n");
	getchar();
	getchar();
}

Array ***PossibleDigits(short *sudokuBoard)
{
	int i, j, s = 0;
	Array ***possible;//matrix of pointers to Array
	possible = (Array***)malloc(9 * sizeof(Array**));
	if (!possible)
	{
		printf("Memory error!");
	}
	for (int k = 0; k < 9; k++)
		possible[k] = (Array**)malloc(9 * sizeof(Array*));
	short **rowOptions = (short**)malloc(9 * sizeof(short*));
	if (!rowOptions) printf("Memory error!");
	short **colOptions = (short**)malloc(9 * sizeof(short*));
	if (!colOptions) printf("Memory error!");
	short **x3Options = (short**)malloc(9 * sizeof(short*));
	if (!x3Options) printf("Memory error!");
	for (i = 0; i < 9; i++)
	{
		rowOptions[i] = RowOpt(sudokuBoard, i);
		colOptions[i] = ColOpt(sudokuBoard, i);
	}
	for (i = 0; i < 9; i += 3)
	{
		for (j = 0; j < 9; j += 3)
		{
			x3Options[s] = x3x3Opt(sudokuBoard, i, j);
			s++;
		}
	}
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			if (*(sudokuBoard + i * 9 + j) == -1)
			{
				possible[i][j] = uniteOpt(rowOptions[i], colOptions[j], x3Options[find3x3(i, j)]);
			}
			else
			{
				possible[i][j] = NULL;//if current cell already has a number, keep it, but pointing to NULL to avoid access violation
			}
		}
	}

	for (s = 0; s < 9; s++)
	{
		free(rowOptions[s]);
		free(colOptions[s]);
		free(x3Options[s]);
	}
	free(rowOptions);
	free(colOptions);
	free(x3Options);

	return possible;
}

void sudokoPrintBoard(short *sBoard)
{
	short tempBoard[9][9];
	short i, j, cur = 0;
	printf("  | 0 1 2| 3 4 5| 6 7 8\n");
	for (i = 0; i < 9; i++)
	{
		if (i % 3 == 0)
			printf("-----------------------\n");
		printf("%2d", i);
		for (j = 0; j < 9; j++)
		{
			if (j % 3 == 0)
				printf("|");
			tempBoard[i][j] = *(sBoard + cur);
			cur++;
			if (tempBoard[i][j] != -1)
				printf("%2d", tempBoard[i][j]);
			else printf("  ");

		}
		printf("\n");

	}
	printf("-----------------------\n");
	return;
}

short* RowOpt(short* sBoard, int row)
{
	int f = 0, curVal;
	short *pos = (short*)calloc(9, sizeof(short));
	if (!pos) printf("Memory error!");
	for (f; f < 9; f++)
	{
		curVal = sBoard[row * 9 + f];
		if (curVal != -1)
			pos[curVal - 1]++;
	}
	return pos;
}
short* ColOpt(short* sBoard, int col)
{
	int f = 0, curVal;
	short *pos = (short*)calloc(9, sizeof(short));
	if (!pos) printf("Memory error!");
	for (f; f < 9; f++)
	{
		curVal = sBoard[col + 9 * f];
		if (curVal != -1)
			pos[curVal - 1]++;
	}
	return pos;
}

short* x3x3Opt(short *sBoard, int row, int col)
{
	int rowEnd, colEnd, curVal, colKeep;
	rowEnd = row + 2;
	colEnd = col + 2;
	colKeep = col;
	short* pos = (short*)calloc(9, sizeof(short));
	if (!pos) printf("Memory error!");
	for (row; row <= rowEnd; row++)
	{
		for (col = colKeep; col <= colEnd; col++)
		{
			curVal = *(sBoard + row * 9 + col);
			if (curVal != -1)
			{
				(pos[curVal - 1])++;
			}
		}
	}
	return pos;
}
int find3x3(int i, int j)
{
	i /= 3;
	j /= 3;
	return i * 3 + j;
}

Array* uniteOpt(short* rowOpt, short* colOpt, short* x3Opt)
{
	Array* toReturn = (Array*)malloc(sizeof(Array));
	if (!toReturn) printf("Memory error!");
	short i, tempVal = 11, logicSize = 0;
	short arr[9] = { 0 };
	for (i = 0; i < 9; i++)
	{
		tempVal = *(rowOpt + i) + *(colOpt + i) + *(x3Opt + i);
		if (tempVal == 0)//current number is relevant for this cube
		{
			arr[logicSize] = i + 1;
			logicSize++;
		}
	}
	toReturn->size = logicSize;
	if (logicSize == 0)//cube is already filled
	{
		toReturn->arr = NULL;
		return toReturn;
	}
	short* newArr = (short*)malloc(logicSize * sizeof(short));
	for (i = 0; i < logicSize; i++)
		newArr[i] = arr[i];
	toReturn->arr = newArr;
	return toReturn;
}

int FillBoard(short** board, Array ***possibilities)
{
	Array* minArr;
	int k, i, j, minRow, minCol, input;//minRow, minCol will hold the location for the cell with minimum options to fill
	short minOptions = 9;
	BOOL filledOnce = TRUE, allGood = TRUE, foundEmpty = TRUE;
	while (filledOnce)
	{
		filledOnce = FALSE;
		for (i = 0; i < 9; i++)
		{
			for (j = 0; j < 9; j++)
			{
				if (possibilities[i][j] != NULL)
				{
					if (possibilities[i][j]->size == 1)
					{
						filledOnce = TRUE;
						*(*(board)+i * 9 + j) = possibilities[i][j]->arr[0];//update current cube
						free(possibilities[i][j]);
						possibilities[i][j] = NULL;
					}
				}
			}
		}
		if (filledOnce)
		{
			reCalc(*board, possibilities);
		}
	}
	while (allGood)
	{
		foundEmpty = FALSE;
		minOptions = 9;
		for (i = 0; i < 9; i++)
		{
			for (j = 0; j < 9; j++)
			{
				if (possibilities[i][j])//if the current sudoku cell is not yet filled
				{
					foundEmpty = TRUE;
					if (minOptions > possibilities[i][j]->size)
					{
						if (possibilities[i][j]->size == 0)//found empty cell in the sudoku board that cant be filled
						{
							return -1;
						}
						minOptions = possibilities[i][j]->size;
						minArr = possibilities[i][j];
						minRow = i;
						minCol = j;
					}
				}
			}
		}
		if (!foundEmpty)//all cells are full - SUCCESS
			return 0;
		if (allGood)//sudoku board can be filled
		{
			if (possibilities[minRow][minCol]->size == 1)//if one option, update automatically
			{
				(*board)[minRow * 9 + minCol] = possibilities[minRow][minCol]->arr[0];
				free(possibilities[minRow][minCol]);
				possibilities[minRow][minCol] = NULL;
			}
			else {
				sudokoPrintBoard(*board);
				printf("Cell number [%d,%d] currently holds the minimum number of possible values, select one of the below:\n", minRow, minCol);
				for (k = 0; k < possibilities[minRow][minCol]->size; k++)
					printf("%d. %d\n", k + 1, possibilities[minRow][minCol]->arr[k]);
				scanf("%d", &input);
				printf("Updating cell %d with value %d\n", minRow * 9 + minCol, input);
				(*board)[minRow * 9 + minCol] = (short)input;
			}
			reCalc(*board, possibilities);
		}
		else return -1;
	}
	return 0;
}
void reCalc(short *board, Array ***possibilities)
{
	short i, j, s = 0;
	short **rowOptions = (short**)malloc(9 * sizeof(short*));
	if (!rowOptions) printf("Memory error!");
	short **colOptions = (short**)malloc(9 * sizeof(short*));
	if (!colOptions) printf("Memory error!");
	short **x3Options = (short**)malloc(9 * sizeof(short*));
	if (!x3Options) printf("Memory error!");
	for (i = 0; i < 9; i++)
	{
		rowOptions[i] = RowOpt(board, i);
		colOptions[i] = ColOpt(board, i);
	}
	for (i = 0; i < 9; i += 3)
	{
		for (j = 0; j < 9; j += 3)
		{
			x3Options[s] = x3x3Opt(board, i, j);
			s++;
		}
	}
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			if (*(board + i * 9 + j) == -1)
			{
				possibilities[i][j] = uniteOpt(rowOptions[i], colOptions[j], x3Options[find3x3(i, j)]);
			}
			else
			{
				possibilities[i][j] = NULL;//if current cell already has a number, keep it, but pointing to NULL to avoid access violation
			}
		}
	}
	for (s = 0; s < 9; s++)
	{
		free(rowOptions[s]);
		free(colOptions[s]);
		free(x3Options[s]);
	}
	free(rowOptions);
	free(colOptions);
	free(x3Options);
}