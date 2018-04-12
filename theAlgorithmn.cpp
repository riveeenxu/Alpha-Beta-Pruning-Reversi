/*
* AI.cpp :
* Description:
* this program is able to:
*	1. read from board.txt and analyze the possible moves
*	2. figure out the potential best move through alphabeta tree
*	3. send the move information to the interface(through move.txt)
*   
* header file credit to Dr.fish. maybe.
* have fun
*/


#include <iostream>
#include <fstream>
#include <time.h>
#include "gamecomm.h"

//#include "ab.h"

using namespace std;


bool pass=false;
int nodecount = 0;
const int boardsize = 8;
const int maxsucc = 45;

const int maxturn = 1;
const int minturn = -1;
const int blank = 0;
const int VS = -1000000;
const int VL = 1000000;
int succ = -1;


struct board
{
	int m[boardsize][boardsize];	
	// 1, 0, -1
	int r,c, turn;	// the move that gets to this board ---- LOOKS LIKE STH IMPORTANT

		board(int n[][boardsize],int row= boardsize, int column= boardsize, int t=1)
		{
			for (int k=0;k<boardsize;k++)
			for (int l=0;l<boardsize;l++)
			m[k][l]=n[k][l];
			r=row;c=column; turn=t;
		}
		board(int row = boardsize, int column = boardsize, int t = 1)
		{
			for (int k = 0; k<boardsize; k++)
				for (int l = 0; l<boardsize; l++)
					m[k][l] = 0;
			r = row; c = column; turn = t;
		}
};

typedef board * state_t;
state_t success = NULL;

int alphabeta(state_t state, int maxDepth, int curDepth, int alpha, int beta);

void printboard(state_t s)
{
	for (int k = 0; k < boardsize; k++){
		for (int l = 0; l < boardsize; l++) {
			cout << s->m[k][l];
		}
		cout << "\n" << endl;
	}
}


bool isterminal(state_t state)
{
	int count = 0;
	//return (state >= 14 && state <= 40);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (state->m[i][j] != blank)
				count++;
		}
	}
	return count>=64 ? 1 : 0;

}

void swap(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = tmp;
}


void flip_in_d(state_t c, int a, int b, int turn, int rinc, int cinc)
{
	const  int origRow = a;
	const  int origCol = b;
	 int row = origRow + rinc;
	 int col = origCol + cinc;
	 c->m[origRow][origCol] = turn;
	while (row < boardsize && col < boardsize&&row >= 0 && col >= 0) {
		int currentcheck = c->m[row][col];
		if (currentcheck == -(turn)) {
			row += rinc;
			col += cinc;
			continue;
		}
		if (currentcheck == turn) {
			row -= rinc;
			col -= cinc;
			while (row != origRow || col != origCol) {
				c->m[row][col] = turn;
				row -= rinc;
				col -= cinc;
			}
			break;
		}
		// Empty square
		break;
	}
}

// function for flip to generate the successor
void flip(state_t c,int a, int b, int turn)
{
	flip_in_d(c, a, b, turn, -1, 0);
	flip_in_d(c, a, b, turn, 1, 0);
	flip_in_d(c, a, b, turn, 0, 1);
	flip_in_d(c, a, b, turn, 0, -1);
	flip_in_d(c, a, b, turn, -1, -1);
	flip_in_d(c, a, b, turn, -1, 1);
	flip_in_d(c, a, b, turn, 1, -1);
	flip_in_d(c, a, b, turn, 1, 1);
}



bool find_in_d(state_t s, int a, int b, int rinc, int cinc,int turn)
{	
	// neighbor postion
	 int row = a + rinc;
	 int col = b + cinc;

	if (row < 0 || col < 0 || row >= boardsize || col >= boardsize) {
		return false;
	}

	// If the immediate neighbor is the same?
	if (s->m[row][col] == turn) {
		return false;
	}

	while (row < boardsize && col < boardsize&&row >=0&&col>=0) {
		int currentcheck = s->m[row][col];
		if (currentcheck == -(turn)) {
			row += rinc;
			col += cinc;
			continue;
		}
		if (currentcheck == turn) {
			//cout << "found a possible move AT " << a << "," << b<< endl;
			//char n;
			//cin >> n;
			return true;
		}

		// Blank square
		return false;
	}
	// Off bounds
	return false;
}
// valid move? if neightbour is itself or blank then no;
bool findsucc(state_t s,int a, int b,int turn)
{
	if (s->m[a][b] != blank)
	{
		//cout << "occupied" << endl;
		return false;
	}
	//up
	if (find_in_d(s, a, b, -1, 0, turn))
		return true;

	//down
	if (find_in_d(s, a, b, 1, 0,turn))
		return true;
	//left
	if (find_in_d(s, a, b, 0, -1, turn))
		return true;
	//right
	if (find_in_d(s, a, b, 0, 1, turn))
		return true;
	//upleft
	if (find_in_d(s, a, b, -1, -1, turn))
		return true;
	//upright
	if (find_in_d(s, a, b, -1, 1, turn))
		return true;
	//downleft
	if (find_in_d(s, a, b, 1, -1, turn))
		return true;
	//downright
	if (find_in_d(s, a, b, 1, 1, turn))
		return true;

	return false;
}

void expand(state_t state, state_t* successor, int &sn, int turn)
{
	//state_t c;
	int itter = 0;
	for (int i = 0; i < boardsize; i++)
	{
		for (int j = 0; j < boardsize; j++)
		{
			//cout << "looking at location" << i << ", " << j << endl;

			//if there is legal move
			if (findsucc(state, i, j,turn))
			{

				//cout << "find a possible move" << endl;
				successor[itter] = new board(state->m);

				//cout << "ready to flip" << endl;
				flip(successor[itter], i, j, turn);
				successor[itter]->r = i;
				successor[itter]->c = j;
				successor[itter]->turn = turn;
				//printboard(successor[itter]);
				itter++;
				//cout << itter << endl;

			}
				
		}
	}
	sn = itter;
	if (sn == 0)
		pass = true;
	// 1/ find all the legal moves
	// 1.5/ how many ?
	// 2/ put them into an array
	// return


}

int max(int a, int b)
{
	return a>b ? a : b;
}

int min(int a, int b)
{
	return a>b ? b : a;
}

int heuristic(state_t s)
{
	//cout << "calculating heuristic" << endl;
	int score = 0;

	int current = 0;

	for (int i = 0; i < boardsize; i++)
	{
		for (int j = 0; j < boardsize; j++)
		{
			if (s->m[i][j] == s->turn)
				current++;
		}
	}

	// good to have corner
	if (s->m[0][0] == s->turn||s->m[0][boardsize-1]==s->turn|| 
		s->m[boardsize-1][0] == s->turn||s->m[boardsize-1][boardsize - 1] == s->turn) {
		score += 7;
	}
	// bad not to have one
	else if (s->m[0][0] == -(s->turn) || s->m[0][boardsize - 1] == -(s->turn) ||
		s->m[boardsize - 1][0] == -(s->turn) || s->m[boardsize - 1][boardsize - 1] == -(s->turn)) {
		score -= 7;
	}

	// second control pt
	if (s->m[0][2] == maxturn || s->m[0][boardsize - 3] == maxturn ||
		s->m[2][0] == maxturn || s->m[2][boardsize-1] == maxturn||
		s->m[boardsize-3][0] == maxturn || s->m[boardsize - 3][boardsize-1] == maxturn||
		s->m[boardsize - 1][2] == maxturn || s->m[boardsize - 1][boardsize-3] == maxturn )
		score += 4;


	//third control pt
	if (s->m[2][2] == maxturn || s->m[boardsize - 3][boardsize - 3] == maxturn ||
		s->m[2][boardsize - 3] == maxturn || s->m[boardsize - 3][2] == maxturn)
		score += 3;

	// edges
	for ( int i = 0; i < boardsize; i++) {
		if (s->m[i][0] == maxturn)
			score+= 2;
		if (s->m[i][boardsize - 1] == maxturn)
			score += 2;
		if (s->m[0][i] == maxturn)
			score += 2;
		if (s->m[boardsize - 1][i] == maxturn)
			score += 2;
	}

	// near corners when corner is empty, bad moves
	//left upper corner
	if (s->m[0][1] == maxturn && s->m[0][0] == blank)
		score -= 5;
	if (s->m[1][1] == maxturn && s->m[0][0] == blank)
		score -= 5;
	if (s->m[1][0] == maxturn && s->m[0][0] == blank)
		score -= 5;
	// right upper corner
	if (s->m[0][boardsize - 2] == maxturn && s->m[0][boardsize - 1] == blank)
		score -= 5;
	if (s->m[1][boardsize - 2] == maxturn && s->m[0][boardsize - 1] == blank)
		score -= 5;
	if (s->m[1][boardsize - 1] == maxturn && s->m[0][boardsize - 1] == blank)
		score -= 5;

	// left lower corner
	if (s->m[boardsize - 2][0] == maxturn && s->m[boardsize - 1][0] == blank)
		score -= 5;
	if (s->m[boardsize - 2][1] == maxturn && s->m[boardsize - 1][0] == blank)
		score -= 5;
	if (s->m[boardsize - 1][1] == maxturn && s->m[boardsize - 1][0] == blank)
		score -= 5;

	//right lower corner
	if (s->m[boardsize - 2][boardsize - 2] == maxturn && s->m[boardsize - 1][boardsize - 1] == blank)
		score -= 5;
	if (s->m[boardsize - 1][boardsize - 2] == maxturn && s->m[boardsize - 1][boardsize - 1] == blank)
		score -= 5;
	if (s->m[boardsize - 2][boardsize - 1] == maxturn && s->m[boardsize - 1][boardsize - 1] == blank)
		score -= 5;
	
	//	//return currentGame.score() + score;
	//cout << "score: "<<current + score<< endl;
	return current + score ;
}


// alpha-beta search
// current state, maxdepth, currentdepth, alpha, beta
int alphabeta(state_t state, int maxDepth, int curDepth,int alpha, int beta)
{
	nodecount++;
	//cout << "entetring " << state << endl;
	if (curDepth == maxDepth || isterminal(state)) // CUTOFF test
	{
		int UtilV = heuristic(state);
		//cout << UtilV<<" heuristic value "<<endl;
		//pass = false;
		return UtilV;  //  the heuristic value of state
	}

	//state_t* successor = new state_t[maxsucc];
	state_t successor[maxsucc];


	int succnum, turn;

	if (curDepth % 2 == 0) // This is a MAX node 
						   // since MAX has depth of: 0, 2, 4, 6, ...
		turn = maxturn;
	else
		turn = minturn;

	expand(state, successor, succnum, turn); // find all successors of state

	//cout << "end expansion, there are totally " << succnum << "possible moves" << endl;
	if (pass)
	{
		turn = -turn;
		curDepth++;
		pass = false;
		
	}

	if (turn == maxturn) // This is a MAX node 
						 // since MAX has depth of: 0, 2, 4, 6, ...
	{
		alpha = VS; // initialize to some very small value 
		for (int k = 0; k<succnum; k++)
		{
			// recursively find the value of each successor
			int curvalue = alphabeta(successor[k], maxDepth, curDepth + 1, alpha, beta);
			//alpha = max(alpha,curvalue); // update alpha
			if (curvalue>alpha || curvalue == alpha && time(0) % 2 == 0)
			{
				alpha = curvalue;
				if (curDepth == 0) {

					success = successor[k];
					//cout << "possible solution" << endl;
					/*printboard(success);
					cout << succ->r << " and " << succ->c << endl;*/
				}
			}

			//prune
			if (alpha >= beta)  return alpha;//{ succ = successor[k]; cout << "this happened" << endl; }
		}
		//cout << state << " [" << alpha << "]\n";
		return alpha;
	}
	else // A MIN node
	{

		beta = VL;  // initialize to some very large value
		for (int k = 0; k<succnum; k++)
		{
			// recursively find the value of each successor
			int curvalue = alphabeta(successor[k], maxDepth, curDepth + 1, alpha, beta);
			beta = min(beta, curvalue); // update beta
			if (alpha >= beta) return beta;
		}
		//cout << state << " [" << beta << "]\n";
		return beta;
	}
}



int main()
{
	int n[8][8];
	getGameBoard(n);


	state_t s = new board(n);

	alphabeta(s, 6, 0, VS, VL);
	//alphabeta(s, 3, 0, VS, VL);
	cout << "The best move place at" << success->r << " , " << success->c <<"and moved by "<<success->turn<< endl;
	//int R;
	//cin >> R;
	//printboard(success);
	//cout << endl;
	//int R;
	//cin >> R;
	if (success)
		putMove(success->r, success->c);

}

