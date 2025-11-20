#include"AmazonGame.h"
#include<cstdlib>
#include<ctime>
#include<algorithm>
AmazonGame::AmazonGame() {
	initializeBoard();
}
void AmazonGame::initializeBoard() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = '.';
		}
	}
	board[0][3] = board[0][6] = 'H';
	board[3][0] = board[3][9] = 'H';
	board[6][0] = board[6][9] = 'C';
	board[9][3] = board[9][6] = 'C';
	attribution = 0.0;
    // 修正 Ccurrentstations 的 push_back 用法
    Ccurrentstations.push_back({ 6,0 });
    Ccurrentstations.push_back({ 6,9 });
    Ccurrentstations.push_back({ 9,3 });
    Ccurrentstations.push_back({ 9,6 });
	Hcurrentstations.push_back({ 3,0 });
	Hcurrentstations.push_back({ 3,9 });
	Hcurrentstations.push_back({ 0,3 });
	Hcurrentstations.push_back({ 0,6 });
	currentPlayer = 'H';
	gameOver = false;
	winner = ' ';
	moveHistory.clear();
}//完成棋盘的初始化
void AmazonGame::displayBoard() {
	std::cout << "   ";
	for (int i = 0; i < BOARD_SIZE; i++) {
		std::cout << " " << i << " ";
	}
	std::cout << std::endl;
	for (int i = 0; i < BOARD_SIZE; i++) {
		std::cout << " " << i << " ";
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << " " << board[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
bool AmazonGame::isValidMove(int fromX, int fromY, int toX, int toY) {
	if (fromX < 0 || fromX >= BOARD_SIZE || fromY < 0 || fromY >= BOARD_SIZE ||
		toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
		return false;
	}
	if (board[fromX][fromY] != currentPlayer) {
		return false;
	}
	if (board[toX][toY] != '.') {
		return false;
	}
	int dx = toX - fromX;
	int dy = toY - fromY;
	if (dx != 0 && dy != 0 && abs(dx) != abs(dy) ){
		return false;
	}
	int stepX = (dx == 0) ? 0 : (dx / abs(dx));
	int stepY = (dy == 0) ? 0 : (dy / abs(dy));
	int x = fromX + stepX;
	int y = fromY + stepY;
	while (x != toX || y != toY) {
		if (board[x][y] != '.') {
			return false;
		}
		x += stepX;
		y += stepY;
	}
	return true;
}
bool AmazonGame::isValidArrow(int fromX, int fromY, int toX, int toY) {
	// 重写这个函数，不要直接调用 isValidMove
	if (fromX < 0 || fromX >= BOARD_SIZE || fromY < 0 || fromY >= BOARD_SIZE ||
		toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
		return false;
	}
	// 检查目标位置是否为空
	if (board[toX][toY] != '.') {
		return false;
	}
	// 检查移动方向（皇后走法）
	int dx = toX - fromX;
	int dy = toY - fromY;
	if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) {
		return false;
	}
	// 检查路径上是否有障碍物
	int stepX = (dx == 0) ? 0 : (dx / abs(dx));
	int stepY = (dy == 0) ? 0 : (dy / abs(dy));
	int x = fromX + stepX;
	int y = fromY + stepY;
	while (x != toX || y != toY) {
		if (board[x][y] != '.') {
			return false;
		}
		x += stepX;
		y += stepY;
	}
	return true;
}
bool AmazonGame::makeMove(int fromX, int fromY, int toX, int toY, int arrowX, int arrowY) {
	if (!isValidMove(fromX, fromY, toX, toY) || !isValidArrow(toX
		, toY, arrowX, arrowY)) {
		return false;
	}
	Move move = { fromX,fromY,toX,toY,arrowX,arrowY };
	moveHistory.push_back(move);
	board[toX][toY] = board[fromX][fromY];
	board[fromX][fromY] = '.';
	board[arrowX][arrowY] = 'X';
    // 替换原有的 Hcurrentstations.erase({ fromX,fromY }); 为如下代码
    auto it = std::find(Hcurrentstations.begin(), Hcurrentstations.end(), std::make_pair(fromX, fromY));
    if (it != Hcurrentstations.end()) {
        Hcurrentstations.erase(it);
    }
	Hcurrentstations.push_back({ toX,toY });
	switchPlayer();
	checkGameOver();
	return true;
}
void AmazonGame::checkGameOver() {
	if (!hasValidMoves(currentPlayer)) {
		gameOver = true;
		winner = (currentPlayer == 'H') ? 'C' : 'H';
	}
}
void AmazonGame::switchPlayer() {
	currentPlayer = (currentPlayer == 'H') ? 'C' : 'H';
}
bool AmazonGame::hasValidMoves(char player) {//获得所有移动的起点
	for (auto& piece :(player=='C'?Ccurrentstations:Hcurrentstations)) {
		int x = piece.first;
		int y = piece.second;
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (isValidMove(x, y, i, j)) {
					for (int k = 0; k < BOARD_SIZE; k++) {
						for (int l = 0; l < BOARD_SIZE; l++) {
							if (isValidArrow(x, y, k, l)) {
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}
void AmazonGame::computerMove() {
	std::vector<Move>possibleMoves;
	for (auto&station:Ccurrentstations){
		int i = station.first;
		int j = station.second;
		for (int x = 0; x < BOARD_SIZE; x++) {
			for (int y = 0; y < BOARD_SIZE; y++) {
				if (isValidMove(i, j, x, y)) {
					for (int ax = 0; ax < BOARD_SIZE; ax++) {
						for (int ay = 0; ay < BOARD_SIZE; ay++) {
							if (isValidArrow(x, y, ax, ay)) {
								Move move = { i, j, x, y, ax, ay };
								possibleMoves.push_back(move);
							}
						}
					}
				}
			}
		}
	}
	if (!possibleMoves.empty()) {
		Move move= mostattribution(possibleMoves);
		std::cout << "Computer moves: (" << move.fromX << "," << move.fromY << ") -> ("
			<< move.toX << "," << move.toY << ") with arrow at ("
			<< move.arrowX << "," << move.arrowY << ")" << std::endl;
		makeMove(move.fromX, move.fromY, move.toX, move.toY, move.arrowX, move.arrowY);
	}
}
void AmazonGame::saveGame(const std::string& filename) {
	std::ofstream file(filename);
	if (!file) {
		std::cout << "error saving game" << std::endl;
		return;
	}
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			file << board[i][j];
		}
		file << std::endl;
	}
	file << currentPlayer << std::endl;
	file << gameOver << std::endl;
	file << winner << std::endl;
	file.close();
	std::cout << "Game saved successfully!" << std::endl;
}
bool AmazonGame::loadGame(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cout << "Error loading game!" << std::endl;
		return false;
	}
	for (int i = 0; i < BOARD_SIZE; i++) {
		std::string line;
		if (!std::getline(file, line) || line.length() != BOARD_SIZE) {
			return false;
		}
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = line[j];
		}
	}
	std::string temp;
	if (!std::getline(file, temp) || temp.empty())return false;
	currentPlayer = temp[0];
	if (!std::getline(file, temp) || temp.empty())return false;
	gameOver =(temp=="1");
	if (!std::getline(file, temp) || temp.empty())return false;
	winner = temp[0];
	file.close();
	std::cout << "Game loaded successfully!" << std::endl;
	return true;
}
void AmazonGame::replayGame() {
	if (moveHistory.empty()) {
		std::cout << "No moves to replay!" << std::endl;	
		return;
	}
	initializeBoard();
	displayBoard();
	for (const auto& move : moveHistory) {
		std::cout << "Move:(" << move.fromX << "," << move.fromY <<
			")->(" << move.toX << "," << move.toY << ")with arrow at("
			<< move.arrowX << "," << move.arrowY << ")"<<std::endl;
		board[move.toX][move.toY] = board[move.fromX][move.fromY];
		board[move.fromX][move.fromY] = '.';
		board[move.arrowX][move.arrowY] = 'X';

		displayBoard();
		std::cout << "Press Enter to continue...";
		std::cin.ignore();
		std::cin.get();
	}
}
void AmazonGame::showMenu() {
	std::cout << "=== Amazon Game ===" << std::endl;
	std::cout << "1. New Game" << std::endl;
	std::cout << "2. Save Game" << std::endl;
	std::cout << "3. Load Game" << std::endl;
	std::cout << "4. Replay Game" << std::endl;
	std::cout << "5. Exit" << std::endl;
	std::cout << "===================" << std::endl;
}
void AmazonGame::playGame() {
	int choice;
	bool playing = true;
	while (playing) {
		showMenu();
		std::cout << "Enter your choice:";
		std::cin >> choice;
		switch (choice) {
		case 1: {
			initializeBoard();
			std::cout << "New game started!" << std::endl;
			while (!gameOver) {
				displayBoard();
				if (currentPlayer == 'H') {
					std::cout << "your turn(H)" << std::endl;
					int fromX, fromY, toX, toY, arrowX, arrowY;
					std::cout << "Enter movecoordinates(x,y)";
					std::cin >> fromX >> fromY;
					std::cout << "Enter targetcoordinates(x,y)";
					std::cin >> toX >> toY;
					std::cout << "Enter arrowcoordinates(x,y)";
					std::cin >> arrowX >> arrowY;
					if (!makeMove(fromX, fromY, toX, toY, arrowX, arrowY)) {
						std::cout << "Invalid move! Try again." << std::endl;
					}
					else {
						std::cout << "Move made successfully!" << std::endl;
					}
				}
				else {
					std::cout << "Computer's turn(C)" << std::endl;
					computerMove();
				}
			}
			displayBoard();
			std::cout << "Game Over! Winner is: " << winner << std::endl;
		} break;
		case 2: {
			std::string filename;
			std::cout << "Enter filename to save:";
			std::cin >> filename;
			saveGame(filename);
		}
			  break;
		case 3: {
			std::string filename;
			std::cout << "Enter filename to load:";
			std::cin >> filename;
			if (loadGame(filename)) {
				std::cout << "Game loaded. Current player: " << currentPlayer << std::endl;
			}
		}break;
		case 4: {
			replayGame();
			
		}break;
		case 5: {
			playing = false;
			std::cout << "Exiting game. Goodbye!" << std::endl;
			
		}break;
		default: {
			std::cout << "Invalid choice! Try again." << std::endl;
		}break;
		}
	}
}