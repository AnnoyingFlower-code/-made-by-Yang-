// 贪吃蛇（made by Yang）.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <conio.h>

class Func {
protected:
	int color;
	struct Pos { int x; int y; };
public:
	Func() :color(NULL) {}
	~Func() {}
	int Random(const int& min, const int& max) {
		return min + rand() % ((max - min) + 1);
	}
	void Draw(const Pos& pos, const int& color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)pos.x, (short)pos.y });
		std::cout << std::setw(2) << "\0";
	}
	template<typename T>
	void Draw(const Pos& pos, T str, const int& color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)pos.x, (short)pos.y });
		std::cout << str;
	}
};

class Console :protected Func {
private:
	CONSOLE_CURSOR_INFO info;
public:
	Console() {
		info = { 1, 0 };
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		system("mode con cols=110 lines=31");
		Init();
	}
	~Console() {}
	void Init() {
		for (int i = 0; i <= 49; i++) {
			for (int j = 0; j <= 30; j++) {
				Draw(Pos{ i * 2,j }, 0x0f);
				if (i == 0 || i == 49 || j == 0 || j == 30) {
					Draw(Pos{ i * 2,j }, 0x8f);
				}
			}
		}
	}
};

class Unit :protected Func {
private:
	std::vector<Pos> unit_xy;
	int direction;
	int score;
	bool lose;
public:
	Unit() {
		lose = false;
		Init();
	}
	~Unit() {}
	void Init() {
		score = 3;
		direction = 4;
		color = 0x2f;
		unit_xy.clear();
		unit_xy.push_back(Pos{ Random(24, 25), Random(14, 15) });
		unit_xy[0].x *= 2;
		int temp = unit_xy[0].x + 2;
		unit_xy.push_back(Pos{ temp,unit_xy[0].y });
		temp = unit_xy[1].x + 2;
		unit_xy.push_back(Pos{ temp,unit_xy[1].y });
		temp = unit_xy[2].x + 2;
		unit_xy.push_back(Pos{ temp,unit_xy[2].y });
		Draw(Pos{ 101, 1 }, "长度:    ", 0x0f);
		Draw(Pos{ 106, 1 }, score, 0x0f);
	}
	void Move(int key) {
		switch (key) {
		case 'w':if (direction != 8)direction = 2; break;
		case 'a':if (direction != 6)direction = 4; break;
		case 's':if (direction != 2)direction = 8; break;
		case 'd':if (direction != 4)direction = 6; break;
		}
	}
	const Pos& GetPosition() {
		return unit_xy[0];
	}
	std::vector<Pos>& GetUnit() {
		return unit_xy;
	}
	bool Dead() {
		return lose;
	}
	void IsDead(Console& console, Unit& unit) {
		for (std::vector<Pos>::iterator temp = unit_xy.begin() + 1; temp != unit_xy.end(); ++temp) {
			if (unit_xy[0].x == (*temp).x && unit_xy[0].y == (*temp).y) {
				lose = true;
			}
		}
		if (unit_xy[0].x < 1 || unit_xy[0].x > 96 || unit_xy[0].y < 1 || unit_xy[0].y > 29) {
			lose = true;
		}
		if (lose) {
			Draw(Pos{ 46, 15 }, "游戏失败", 0x0f);
			Draw(Pos{ 44, 16 }, "按空格键继续", 0x0f);
			Draw(Pos{ 32, 18 }, "不会操作？ Yang知道游戏总是用WASD移动的", 0x0f);
		}
		while (lose) {
			if (_kbhit() == 1 && _getch() == 32) {
				console.Init();
				unit.Init();
				break;
			}
		}
	}
	void Play() {
		for (std::vector<Pos>::iterator temp = unit_xy.end() - 1; temp != unit_xy.begin(); --temp) {
			*temp = *(temp - 1);
		}
		switch (direction) {
		case 1:unit_xy[0].x -= 2; unit_xy[0].y -= 1; break;
		case 2:unit_xy[0].y -= 1; break;
		case 3:unit_xy[0].x += 2; unit_xy[0].y -= 1; break;
		case 4:unit_xy[0].x -= 2; break;
		case 6:unit_xy[0].x += 2; break;
		case 7:unit_xy[0].x -= 2; unit_xy[0].y += 1; break;
		case 8:unit_xy[0].y += 1; break;
		case 9:unit_xy[0].x += 2; unit_xy[0].y += 1; break;
		}
		for (std::vector<Pos>::iterator temp = unit_xy.begin(); temp != unit_xy.end(); ++temp) {
			if (temp == unit_xy.end() - 1) {
				Draw(*temp, 0x0f);
			}
			else if (temp == unit_xy.begin()) {
				Draw(*temp, 0xbf);
			}
			else {
				Draw(*temp, color);
			}
		}
	}
	void LevelUP() {
		score++;
		unit_xy.push_back(*(unit_xy.end() - 1));
		Draw(Pos{ 106, 1 }, score, 0x0f);
	}
	void Reset() {
		lose = false;
	}
	const int& GetScore() {
		if (score < 100) {
			return score;
		}
		else {
			return 99;
		}
	}
};

class Food :protected Func {
private:
	Pos food_xy;
public:
	Food() {
		food_xy = { 0, 0 };
		color = 0xcf;
	}
	~Food() {}
	Food(Unit& unit) {
		color = 0xcf;
		CreateFood(unit);
	}
	void SetColor(const int& color) {
		this->color = color;
	}
	void CreateFood(Unit& unit) {
		while (1) {
			bool end = false;
			food_xy = { Random(1, 48), Random(1, 29) };
			food_xy.x *= 2;
			for (std::vector<Pos>::iterator temp = unit.GetUnit().begin(); temp != unit.GetUnit().end(); ++temp) {
				if (food_xy.x == (*temp).x && food_xy.y == (*temp).y) {
					end = true;
				}
			}
			if (!end) {
				break;
			}
		}
		Draw(food_xy, color);
	}
	const Pos& GetPosition() {
		return food_xy;
	}
};

int main() {
	srand((unsigned int)time(NULL));
	Console cmd;
	Unit unit;
	Food food(unit);
	while (1) {
		if (_kbhit() == 1) {
			unit.Move(_getch());
		}
		if (food.GetPosition().x == unit.GetPosition().x &&
			food.GetPosition().y == unit.GetPosition().y) {
			unit.LevelUP();
			food.CreateFood(unit);
		}
		unit.Play();
		unit.IsDead(cmd, unit);
		if (unit.Dead()) {
			food.CreateFood(unit);
			unit.Reset();
		}
		Sleep(100 - unit.GetScore());
	}

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
