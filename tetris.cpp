#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <thread>
#include <vector>
#include <stdio.h>

using namespace std;

HWND hWnd = GetConsoleWindow();
HDC hDC = GetDC(hWnd);

class Tetramino;

class Field
{
private:
	static const int nFieldWidth = 12;
	static const int nFieldHeight = 18;
	int pField[nFieldWidth * nFieldHeight];
	friend class Tetramino;
	friend class Game;
	friend class ScreenBuffer;
public:
	Field() {}
};

class ScreenBuffer
{
	static Field field;
	int size = 20;
	int myscreenbuffer[field.nFieldWidth * field.nFieldHeight];
	int posX = 200;
	int posY = 20;
	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(10, 10, 10));
	HBRUSH Brushes[10] = {
	CreateSolidBrush(RGB(0, 0, 0)) ,
	CreateSolidBrush(RGB(255, 0, 0)) ,
	CreateSolidBrush(RGB(0, 255, 0)) ,
	CreateSolidBrush(RGB(0, 0, 255)) ,
	CreateSolidBrush(RGB(255, 255, 0)) ,
	CreateSolidBrush(RGB(0, 255, 255)) ,
	CreateSolidBrush(RGB(255, 0, 255)),
	CreateSolidBrush(RGB(200, 0, 100)),
	CreateSolidBrush(RGB(255, 255, 255)),
	CreateSolidBrush(RGB(255, 140, 0))
	};
public:
	ScreenBuffer() {}
	ScreenBuffer(Field& field)
	{
		this->field = field;
	}
	~ScreenBuffer()
	{
		for (int i = 0; i < 10; i++)
			delete Brushes[i];
		delete Pen;
	}
	void fill(int pField[216], Field& field) {
		for (int i = 0; i < field.nFieldWidth * field.nFieldHeight; i++)
		{
			this->myscreenbuffer[i] = pField[i];
		}
	};
	void fill(int temp[16], int nCurrentX, int nCurrentY, Field& field) {
		for (int py = 0; py < 4; py++)
			for (int px = 0; px < 4; px++)
			{
				if (temp[py * 4 + px] != 0)
					this->myscreenbuffer[(nCurrentY + py) *
					field.nFieldWidth + (nCurrentX + px)] = temp[py * 4 + px];
			}
	};
	void draw(Field& field) {
		SelectObject(hDC, Pen);
		for (int i = 0; i < field.nFieldHeight; i++)
		{
			for (int j = 0; j < field.nFieldWidth; j++)
			{
				switch (myscreenbuffer[i * field.nFieldWidth + j])
				{
				case 1:
					SelectObject(hDC, Brushes[1]);
					break;
				case 2:
					SelectObject(hDC, Brushes[2]);
					break;
				case 3:
					SelectObject(hDC, Brushes[3]);
					break;
				case 4:
					SelectObject(hDC, Brushes[4]);
					break;
				case 5:
					SelectObject(hDC, Brushes[5]);
					break;
				case 6:
					SelectObject(hDC, Brushes[6]);
					break;
				case 7:
					SelectObject(hDC, Brushes[7]);
					break;
				case 8:
					SelectObject(hDC, Brushes[8]);
					break;
				case 9:
					SelectObject(hDC, Brushes[9]);
					break;
				default:
					SelectObject(hDC, Brushes[0]);
					break;
				}//switch
				Rectangle(hDC, j * size + posX, i * size + posY, j * size +
					size + posX, i * size + size + posY);
			}//for
		}//for
	}
};

class Tetramino
{
private:
	int tetromino[7][16] =
	{
	{
	0,0,1,0,
	0,0,1,0,
	0,0,1,0,
	0,0,1,0
	},
	{
	0,0,2,0,
	0,2,2,0,
	0,0,2,0,
	0,0,0,0
	},
	{
	0,0,0,0,
	0,3,3,0,
	0,3,3,0,
	0,0,0,0
	},
	{
	0,0,4,0,
	0,4,4,0,
	0,4,0,0,
	0,0,0,0
	},
	{
	0,5,0,0,
	0,5,5,0,
	0,0,5,0,
	0,0,0,0
	},
	{
	0,6,0,0,
	0,6,0,0,
	0,6,6,0,
	0,0,0,0
	},
	{
	0,0,7,0,
	0,0,7,0,
	0,7,7,0,
	0,0,0,0
	}
	};
public:
	int Rotate(int px, int py, int r)
	{
		int pi = 0;
		switch (r % 4)
		{
		case 0: // 0 degrees // 0 1 2 3
			pi = py * 4 + px; // 4 5 6 7
			break; // 8 9 10 11
			//12 13 14 15
		case 1: // 90 degrees //12 8 4 0
			pi = 12 + py - (px * 4); //13 9 5 1
			break; //14 10 6 2
			//15 11 7 3
		case 2: // 180 degrees //15 14 13 12
			pi = 15 - (py * 4) - px; //11 10 9 8
			break; // 7 6 5 4
			// 3 2 1 0
		case 3: // 270 degrees // 3 7 11 15
			pi = 3 - py + (px * 4); // 2 6 10 14
			break; // 1 5 9 13
		} // 0 4 8 12
		return pi;
	}
	bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY, Field&
		field)
	{
		// All Field cells >0 are occupied
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
			{
				// Get index into piece
				int pi = Rotate(px, py, nRotation);
				// Get index into field
				int fi = (nPosY + py) * field.nFieldWidth + (nPosX + px);
				if (nPosX + px >= 0 && nPosX + px < field.nFieldWidth)
				{
					if (nPosY + py >= 0 && nPosY + py <
						field.nFieldHeight)
					{ // In Bounds so do collision check
						if (tetromino[nTetromino][pi] != 0 &&
							field.pField[fi] != 0)
							return false; // fail on first hit
					}
				}
			}
		return true;
	}
	friend class Game;
};

void wait_ms(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

class Game
{
private:
	Field field;
	Tetramino tetr;
	ScreenBuffer s;
	bool bKey[4];
	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX;
	int nCurrentY = 0;
	int nSpeed = 20;
	int nSpeedCount = 0;
	bool bForceDown = false;
	bool bRotateHold = true;
	int nPieceCount = 0;
	int nScore = 0;
	vector<int> vLines;
	bool started = false;
public:
	bool bGameOver = false;
	Game(Field& field, Tetramino& tetr)
	{
		this->field = field;
		this->tetr = tetr;
	}
	Game() {}
	void gameplay()
	{
		if (!started) {
			for (int x = 0; x < field.nFieldWidth; x++)
				for (int y = 0; y < field.nFieldHeight; y++)
					if (/* left-right-buttom: */ x == 0 || x ==
						field.nFieldWidth - 1 || y == field.nFieldHeight - 1) {
						field.pField[y * field.nFieldWidth + x] = 8;
					}
					else
					{
						field.pField[y * field.nFieldWidth + x] = 0;
					}
		}
		started = true;
		//this_thread::sleep_for(30ms); // Small Step = 1 Game Tick
		wait_ms(30);
		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);
		// Input ========================
		for (int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned
				char)("\x27\x25\x28\x26"[k]))) != 0; // R L D Z
		// Handle player movement
		if (bKey[0] && tetr.DoesPieceFit(nCurrentPiece, nCurrentRotation,
			nCurrentX + 1, nCurrentY, field)) {
			nCurrentX++;
		};
		if (bKey[1] && tetr.DoesPieceFit(nCurrentPiece, nCurrentRotation,
			nCurrentX - 1, nCurrentY, field)) {
			nCurrentX--;
		};
		if (bKey[2] && tetr.DoesPieceFit(nCurrentPiece, nCurrentRotation,
			nCurrentX, nCurrentY + 1, field)) {
			nCurrentY++;
		};
		if (bKey[3]) // Rotate, but latch to stop wild spinning
		{
			nCurrentRotation += (bRotateHold &&
				tetr.DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY, field))
				? 1 : 0;
			bRotateHold = false;
		}
		else
			bRotateHold = true;
		if (bForceDown)// Force the piece down the playfield if it's time
		{ // Update difficulty every 30 pieces
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0)
				if (nSpeed >= 10) nSpeed--;
			// Test if piece can be moved down
			if (tetr.DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX,
				nCurrentY + 1, field))
				nCurrentY++; // It can, so do it!
			else
			{// It can't! so Lock the piece in place
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if
							(tetr.tetromino[nCurrentPiece][tetr.Rotate(px, py, nCurrentRotation)] != 0)
							field.pField[(nCurrentY + py) * field.
							nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
				// Check for lines
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < field.nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < field.nFieldWidth - 1;
							px++)
							bLine = bLine &&
							((field.pField[(nCurrentY + py) * field.nFieldWidth + px]) != 0);
						if (bLine)
						{
							// Remove Line, set to =
							for (int px = 1; px < field.nFieldWidth
								- 1; px++)
								field.pField[(nCurrentY + py) *
								field.nFieldWidth + px] = 9;
							vLines.push_back(nCurrentY + py);
						}
					}
				nScore += 25;
				if (!vLines.empty()) {
					nScore = nScore + (vLines.size() * 2) * 100;
				};
				// Pick New Piece
				nCurrentX = field.nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;
				// If piece does not fit straight away, game over!
				bGameOver = !tetr.DoesPieceFit(nCurrentPiece,
					nCurrentRotation, nCurrentX, nCurrentY, field);
				started = !bGameOver;
			}
		}
		// Animate Line Completion
		if (!vLines.empty())
		{
			// Display Frame (cheekily to draw lines)
			s.fill(field.pField, field);
			s.draw(field);
			// Delay a bit
			//this_thread::sleep_for(400ms);
			wait_ms(400);
			for (auto& v : vLines) {
				//this_thread::sleep_for(1000ms);
				wait_ms(1000);
				for (int px = 1; px < field.nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						field.pField[py * field.nFieldWidth + px] =
						field.pField[(py - 1) * field.nFieldWidth + px];
					field.pField[px] = 0;
				}
			}
			vLines.clear();
		}
		//put field into buffer
//Put Current Piece in the screen buffer
		int temp1[16] = { 0 };
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				temp1[(py * 4 + px)] =
				tetr.tetromino[nCurrentPiece][tetr.Rotate(px, py, nCurrentRotation)];
		s.fill(field.pField, field);
		s.fill(temp1, nCurrentX, nCurrentY, field);
		s.draw(field);
}
friend class menu;
};
class menu
{
	string name;
	COORD coordscreen;
	HPEN WPen;
	HPEN BPen;
	HBRUSH BBrush;
public:
	menu(string name)
	{
		this->name = name;
		WPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		BPen = CreatePen(PS_SOLID, 2, RGB(12, 12, 12));
		BBrush = CreateSolidBrush(RGB(12, 12, 12));
	}
	void start()
	{
		coordscreen.X = 22;
		coordscreen.Y = 7;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
			(COORD)coordscreen);
		SelectObject(hDC, WPen);
		cout << "Введите свое имя: ";
		MoveToEx(hDC, 100, 70, 0);
		LineTo(hDC, 450, 70);
		LineTo(hDC, 450, 200);
		LineTo(hDC, 100, 200);
		LineTo(hDC, 100, 70);
		cin >> name;
		coordscreen.Y = 0;
		coordscreen.X = 0;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
			(COORD)coordscreen);
		SelectObject(hDC, BPen);
		SelectObject(hDC, BBrush);
		Rectangle(hDC, 95, 65, 455, 205);
	}
	void end(Game& game)
	{
		SelectObject(hDC, BPen);
		SelectObject(hDC, BBrush);
		Rectangle(hDC, 0, 0, 800, 800);
		coordscreen.Y = 6;
		coordscreen.X = 26;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
			(COORD)coordscreen);
		cout << "Игра окончена!!! ";
		coordscreen.Y = 7;
		coordscreen.X = 21;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
			(COORD)coordscreen);
		cout << "Игрок - " << name << ", очки - " << game.nScore << endl;
		system("pause>0");
	}
	~menu()
	{
		DeleteObject(WPen);
		DeleteObject(BPen);
		DeleteObject(BBrush);
	}
};
int main()
{
	setlocale(LC_ALL, "rus");
	string name;
	Field field;
	Tetramino tetr;
	Game game(field, tetr);
	menu player(name);
	player.start(); //Menu is called
	while (!game.bGameOver) // Main Loop
	{
		game.gameplay();
	}
	player.end(game);
	return 0;
}