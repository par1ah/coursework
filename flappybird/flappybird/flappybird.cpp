#include <iostream>
#include <string>
#include <thread>
using namespace std;

#include "game_header.h"
#define CURL_STATICLIB
#include <curl\curl.h>
#include <string>

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

void main2(int x) { //получение информации с сервера
	auto curl = curl_easy_init();
	if (curl) {
		std::string data;
		data.append("http://localhost:8080/?&key=");
		data.append(std::to_string(x));
		curl_easy_setopt(curl, CURLOPT_URL, data.c_str());
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

		std::string response_string;
		std::string header_string;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

		char* url;
		long response_code;
		double elapsed;
		//curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		//curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
		//curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		//std::cout << response_string << std::endl;
		//std::cout << header_string << std::endl;
		curl = NULL;
	}
	return;
}


class FlappyBird : public games

{
public:
	FlappyBird()
	{
		AppName = L"Flappy Bururbl4";	//Широкий строковый литерал имеет тип "массив n const wchar_t", 
									//где n - размер строки, как определено ниже; 
									//в нем есть статической продолжительности хранения и инициализируется заданными символами.
	}

private:

	thread test;
	float Position = 0.0f; //позиция челика
	float Velocity = 0.0f; //скорость челика
	float Acceleration = 0.0f; //ускорение
	float Gravity = 100.0f; //притяжение
	float LevelPosition = 0.0f; //точка спавна

	float SectionWidth;
	list<int> listSection;

	bool HasCollided = false; //пробел
	bool ResetGame = false;

	int AttemptCount = 0; //счетчик попыток
	int FlapCount = 0; //счетчик пробелов
	int MaxFlapCount = 0; // макс пробелов


protected:
	// вызывается 
	virtual bool OnUserCreate()
	{
		listSection = { 0, 0, 0, 0 };
		//test = thread(Send, nMaxFlapCount);
		//test.detach();
		ResetGame = true;
		SectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);
		return true;
	}

	// вызывается
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (ResetGame) //обнуление игры
		{
			HasCollided = false;
			ResetGame = false;
			listSection = { 0, 0, 0, 0 };
			Acceleration = 0.0f;
			Velocity = 0.0f;
			Position = ScreenHeight() / 2.0f;
			FlapCount = 0;
			AttemptCount++;
		}

		// геймплей
		if (HasCollided)
		{
			// ничего не делать, пока не освободится место
			if (m_keys[VK_SPACE].bReleased) {
				thread test(main2, 3);
				test.detach();
				//main2(nMaxFlapCount);
				ResetGame = true;
			}

		}
		else
		{

			if (m_keys[VK_SPACE].bPressed && Velocity >= Gravity / 10.0f)
			{
				Acceleration = 0.0f;
				Velocity = -Gravity / 4.0f;
				FlapCount++;
				if (FlapCount > MaxFlapCount)
					MaxFlapCount = FlapCount;
			}
			else
				Acceleration += Gravity * fElapsedTime; //передвижение

			if (Acceleration >= Gravity)
				Acceleration = Gravity;

			Velocity += Acceleration * fElapsedTime;
			Position += Velocity * fElapsedTime;
			LevelPosition += 14.0f * fElapsedTime;

			if (LevelPosition > SectionWidth)
			{
				LevelPosition -= SectionWidth;
				listSection.pop_front();
				int i = rand() % (ScreenHeight() - 20);
				if (i <= 10) i = 0;
				listSection.push_back(i);
			}

			// дисплей
			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

			// столбики рисуются
			int section = 0;
			for (auto s : listSection)
			{
				if (s != 0)
				{
					Fill(section * SectionWidth + 11 - LevelPosition, ScreenHeight() - s - 2, section * SectionWidth + 16 - LevelPosition, ScreenHeight(), PIXEL_SOLID, FG_WHITE); //обводка справа
					Fill(section * SectionWidth + 9 - LevelPosition, ScreenHeight() - s - 2, section * SectionWidth + 14 - LevelPosition, ScreenHeight(), PIXEL_SOLID, FG_WHITE); //обводка слева
					Fill(section * SectionWidth + 10 - LevelPosition, ScreenHeight() - s - 1, section * SectionWidth + 15 - LevelPosition, ScreenHeight(), PIXEL_SOLID, FG_DARK_YELLOW);//столбики снизу
					Fill(section * SectionWidth + 11 - LevelPosition, 0, section * SectionWidth + 16 - LevelPosition, ScreenHeight() - s - 15, PIXEL_SOLID, FG_WHITE); // обводка справа
					Fill(section * SectionWidth + 9 - LevelPosition, 0, section * SectionWidth + 14 - LevelPosition, ScreenHeight() - s - 15, PIXEL_SOLID, FG_WHITE); //обводка слева
					Fill(section * SectionWidth + 10 - LevelPosition, 0, section * SectionWidth + 15 - LevelPosition, ScreenHeight() - s - 16, PIXEL_SOLID, FG_DARK_BLUE); //столбики сверху
				}
				section++;
			}

			int man = (int)(ScreenWidth() / 3.0f);

			// обнаружение столкновений
			HasCollided = Position < 2 || Position > ScreenHeight() - 2 ||
				bufScreen[(int)(Position - 1) * ScreenWidth() + man].Char.UnicodeChar != L' ' || 
				bufScreen[(int)(Position + 0) * ScreenWidth() + man].Char.UnicodeChar != L' ' || 
				bufScreen[(int)(Position - 1) * ScreenWidth() + man + 5].Char.UnicodeChar != L' ' || 
				bufScreen[(int)(Position + 0) * ScreenWidth() + man + 5].Char.UnicodeChar != L' '; 

			// человечек
			if (Velocity > 0)
			{
				DrawString(man, Position - 1, L" (••) ");
				DrawString(man, Position + 0, L" /██╲ ");
				DrawString(man, Position + 1, L"  ▌▌");
			}
			else
			{
				DrawString(man, Position - 1, L" (-.)/");
				DrawString(man, Position + 0, L" [)/");
				DrawString(man, Position + 1, L" /  ");
			}

			DrawString(1, 1, L"Attempt: " + to_wstring(AttemptCount) + L" Score: " + to_wstring(FlapCount) + L" High Score: " + to_wstring(MaxFlapCount));
		}

		return true;
	}
};

int main()
{

	FlappyBird game;
	game.ConstructConsole(100, 60, 160, 160); //рамеры окна консоли
	game.Start();

	return 0;
}