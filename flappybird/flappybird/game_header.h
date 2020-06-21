#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

enum COLOUR 
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class Sprite
{
public:
	int nWidth = 0;
	int nHeight = 0;

private:
	short* Glyphs = nullptr;
	short* Colours = nullptr;

	void Create(int w, int h)
	{
		nWidth = w;
		nHeight = h;
		Glyphs = new short[w * h];
		Colours = new short[w * h];
		for (int i = 0; i < w * h; i++)
		{
			Glyphs[i] = L' ';
			Colours[i] = FG_BLACK;
		}
	}

public:
	short GetGlyph(int x, int y)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return L' ';
		else
			return Glyphs[y * nWidth + x];
	}

	short GetColour(int x, int y)
	{
		if (x < 0 || x >= nWidth || y < 0 || y >= nHeight)
			return FG_BLACK;
		else
			return Colours[y * nWidth + x];
	}
};

class games
{
public:
	games()
	{
		nScreenWidth = 1920;
		nScreenHeight = 1080;

		Console = GetStdHandle(STD_OUTPUT_HANDLE);
		ConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		bEnableSound = false;

		AppName = L"Default";
	}

	int ConstructConsole(int width, int height, int fontw, int fonth)
	{

		nScreenWidth = width;
		nScreenHeight = height;
	
		// получение информации о буфере экрана и проверка максимально допустимого размера окна. вывод
		// ошибки при превышении, так что пользователь узнает, что их размеры/размер шрифта слишком болльшой
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(Console, &csbi))
			return Error(L"получить информацию о буфере экрана консоли");
		if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
			return Error(L"высота экрана/высота Шрифта слишком большая");
		if (nScreenWidth > csbi.dwMaximumWindowSize.X)
			return Error(L"ширина экрана/ширина шрифта слишком большая");

		// установить размер консоли
		rectWindow = { 0, 0, (short)nScreenWidth - 1, (short)nScreenHeight - 1 };
		if (!SetConsoleWindowInfo(Console, TRUE, &rectWindow))
			return Error(L"информация об окне консоли");

		// выделить память для буфера
		bufScreen = new CHAR_INFO[nScreenWidth * nScreenHeight];
		memset(bufScreen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

		return 1;
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
		{
			bufScreen[y * nScreenWidth + x].Char.UnicodeChar = c;
			bufScreen[y * nScreenWidth + x].Attributes = col;
		}
	}

	void Fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void DrawString(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			bufScreen[y * nScreenWidth + x + i].Char.UnicodeChar = c[i];
			bufScreen[y * nScreenWidth + x + i].Attributes = col;
		}
	}

	void Clip(int& x, int& y)
	{
		if (x < 0) x = 0;
		if (x >= nScreenWidth) x = nScreenWidth;
		if (y < 0) y = 0;
		if (y >= nScreenHeight) y = nScreenHeight;
	}

	void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
	}

public:
	void Start()
	{
		//запуск потока
		AtomActive = true;
		std::thread t = std::thread(&games::GameThread, this);

		//выход из потока
		t.join();
	}

	int ScreenWidth()
	{
		return nScreenWidth;
	}

	int ScreenHeight()
	{
		return nScreenHeight;
	}

private:
	void GameThread()
	{
		//создаем ресурсы как часть потока
		if (!OnUserCreate())
			AtomActive = false;

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (AtomActive)
		{
			
			while (AtomActive)
			{
				
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				float fElapsedTime = elapsedTime.count();

				// управление с клавы
				for (int i = 0; i < 256; i++)
				{
					keyState[i] = GetAsyncKeyState(i);

					m_keys[i].bPressed = false;
					m_keys[i].bReleased = false;

					if (keyState[i] != keyOldState[i])
					{
						if (keyState[i] & 0x8000)
						{
							m_keys[i].bPressed = !m_keys[i].bHeld;
							m_keys[i].bHeld = true;
						}
						else
						{
							m_keys[i].bReleased = true;
							m_keys[i].bHeld = false;
						}
					}
					keyOldState[i] = keyState[i];
				}
				// покадровое обновление, если убрать консоль пустая
				if (!OnUserUpdate(fElapsedTime))
					AtomActive = false;
				// обновление заголовка консоли и текущий буфер экрана
				wchar_t s[256];
				swprintf_s(s, 256, L" %s - FPS: %3.2f", AppName.c_str(), 1.0f / fElapsedTime);
				SetConsoleTitle(s);
				WriteConsoleOutput(Console, bufScreen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &rectWindow);
			}
		}
	}

public:
	// переопределение функции
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;

	// очистка
	virtual bool OnUserDestroy() { return true; }

	unsigned int SampleRate;
	unsigned int Channels;
	unsigned int BlockCount;
	unsigned int BlockSamples;
	unsigned int BlockCurrent;

protected:

	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;

protected:
	int Error(const wchar_t* msg)
	{
		wchar_t buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(OriginalConsole);
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

protected:
	int nScreenWidth;
	int nScreenHeight;
	CHAR_INFO* bufScreen;
	std::wstring AppName;
	HANDLE OriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO OriginalConsoleInfo;
	HANDLE Console;
	HANDLE ConsoleIn;
	SMALL_RECT rectWindow;
	short keyOldState[256] = { 0 };
	short keyState[256] = { 0 };
	bool mouseOldState[5] = { 0 };
	bool mouseState[5] = { 0 };
	bool ConsoleInFocus = true;
	bool bEnableSound = false;

	// должны быть статическими из-за вызова OnDestroy, который может сделать ОС
	//порождает специальный поток только для этого
	static std::atomic<bool> AtomActive;
	static std::condition_variable m_cvGameFinished;
	static std::mutex m_muxGame;
};

// Определяем статические переменные
std::atomic<bool> games::AtomActive(false);
std::condition_variable games::m_cvGameFinished;
std::mutex games::m_muxGame;