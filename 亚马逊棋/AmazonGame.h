#pragma once
#ifndef AMAZONGAME_H
#define AMAZONGAME_H
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
struct Move {
	int fromX, fromY, toX, toY;
	int arrowX, arrowY;
};
class AmazonGame {
private:
	static const int BOARD_SIZE = 10;
	char board[BOARD_SIZE][BOARD_SIZE];
	double attribution;
	char currentPlayer;//H:human,C:computer
	bool gameOver;
	char winner;
	std::vector<std::pair<int, int>>Ccurrentstations;
	std::vector<std::pair<int, int>>Hcurrentstations;
	std::vector<Move>moveHistory;

public:
	AmazonGame();
	void initializeBoard();//初始化棋盘
	void displayBoard();//展示棋盘
	bool isValidMove(int fromX, int fromY, int toX, int toY);
	bool isValidArrow(int fromX, int fromY, int toX, int toY);
	bool makeMove(int fromX, int fromY, int toX, int toY, int arrowX, int arrowY);
	void checkGameOver();
	void switchPlayer();
	bool hasValidMoves(char player);
	void computerMove();
	void saveGame(const std::string& filename);
	bool loadGame(const std::string& filename);
	void replayGame();
	void showMenu();
	void playGame();
	int areacalculate(Move);
	Move mostattribution(const std::vector<Move>);
	// Getter methods
	bool isGameOver() const { return gameOver; }
	char getCurrentPlayer() const { return currentPlayer; }
	char getWinner() const { return winner; }
};

#endif

