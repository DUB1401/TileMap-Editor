//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") 
#pragma comment(lib, "Winmm.lib")

///Отключение малозначимых предупреждений компилятора для ранних этапов сборки.
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4305)
#pragma warning(disable:4267)
#pragma warning(disable:4390)

#include "SFML/Graphics.hpp"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <string>
#include <fstream>

using namespace sf;
using namespace std;

//Глобально доступная переменная версии.
string MODULE_VERSION = "0.0.zero";
string CODE_NAME = "Cornflakes";

///=====================================================================================================================///
///DUBO_API

//Удаляет определённое число последних символов из строки.
std::string DeleteLastSymbols(std::string Str, unsigned int NumOfDeleteSymbols) {
	std::string ReturnStr;
	for (int i = 0; i < Str.length() - NumOfDeleteSymbols; i++) {
		ReturnStr.push_back(Str[i]);
	}
	return ReturnStr;
}

//Удаляет определённое число первых символов из строки.
std::string DeleteFirstSymbols(std::string Str, unsigned int NumOfDeleteSymbols) {
	std::string ReturnStr;
	for (int i = NumOfDeleteSymbols; i < Str.length(); i++) ReturnStr.push_back(Str[i]);
	return ReturnStr;
}

//Находит позицию первого символа в строке. Ркомендуется использовать вместе с StringHaveSymbol, для исключения нахождения символа на нулевой позиции.
unsigned int FindFirstSymbol(std::string Str, char Symbol) {
	for (int i = 0; i < Str.length(); i++) if (Str[i] == Symbol) return i;
	return NULL;
}

//Проверяет комплиментарность строк.
bool isStringComplemetarityString(string Str1, string Str2) {
	for (int i = 0; i < Str2.length(); i++) if (Str2[i] != Str1[i]) return FALSE; 
	return TRUE;
}

template <typename NUM>
//Преобразует любое число в строку.
std::string NUMtoSTRING(NUM Number) {
	std::stringstream out;
	out << Number;
	return out.str();
}

//Преобразует логическую переменную в строку.
string BOOLtoSTRING(bool Value) {
	if (Value) return "TRUE"; else return "FALSE";
}

//Преобразует строку в логическую переменную.
bool STRINGtoBOOL(string Value) {
	if (Value == "True" || Value == "TRUE" || Value == "true") return TRUE; else return FALSE;
}

///=====================================================================================================================///

class CHUNK {
private:
	//Директория пакета текстур.
	string TexturePack;
	//Директория исполнения.
	string Directory;
	//Стиль чанка.
	string Style;
	//Адрес файла чанка.
	string CHDR;
	//Спрайт для отрисовки тайлов.
	Sprite CurrentSprite;
	//Массив текстур всех тайлов.
	vector<Texture> Textures;
	//Текстура селектора тайла.
	Texture SelectorTexture;
	//Спрайт селектора тайла.
	Sprite SelectorSprite;
	//Текстура границы зоны чанков.
	Texture BorderTexture;
	//Спрайт границы зоны чанков.
	Sprite BorderSprite;

	///Текстуры и спрайты кнопок.
	Texture Button1Texture;
	Texture Button2Texture;
	Texture Button3Texture;
	Texture OnlineButtonTexture;
	Texture ErrorTexture;
	Sprite Button1;
	Sprite Button2;
	Sprite Button3;
	Sprite OnlineButton;

	///Элементы красного меню.
	RectangleShape SaveAndExit;
	RectangleShape OpenChunk;
	RectangleShape RefreshCells;
	RectangleShape FocusTextBox;
	Texture CheckBoxONTexture;
	Texture CheckBoxOFFTexture;
	Sprite CellsCheckBox;

	///Информация о тайлах и чанке.
	//Глоабльный шрифт, использующийся в стандартном тексте программы.
	Font TextFont;
	vector<Text> Lines;
	vector<Text> LinesRed;
	vector<Text> LinesGreen;
	//Вектор ключей для чтения и записи настроек.
	vector<string> Keys;
	unsigned int CHMAP[16][16];
	unsigned int PNGS = 0;
	RectangleShape Cells;

	//Перечисление для получения доступа к текстовым слоям.
	enum {Info = 0, Name, NameUI, BY, BYUI, Styles, StylesUI, Description, X_coord, X_coordUI, Y_coord, Y_coordUI, ID, IDUI};
	enum RedButton {Exit = 0, PragmaInfo, Version, CodeName, Open};

	//Идентификаторы для доступа к слоям GUI.
	enum {ButtonGreen = 1, ButtonYellow, ButtonRed, Grid, GridUI};

	//Нынешний слой GUI. По умолчанию жёлтый.
	unsigned short int CurrentButton = ButtonYellow;

	///Логические переключатели.
	bool isClicked = false;
	bool isSaveAndExitClicked = false;
	bool isCellsCheckBox = false;
	bool isOpenChunkClicked = false;
	vector<bool> Settings;
	enum SETTINGS {CellsON = 0};

	///Функции обработки данных класса.

	//Создаёт обычный текст.
	Text CreateSmallText(string Str, Vector2f Position) {
		Text SmallText;
		SmallText.setFont(TextFont);
		SmallText.setCharacterSize(14);
		SmallText.setOutlineColor(sf::Color::White);
		SmallText.setString(Str);
		SmallText.setPosition(Position);
		return SmallText;
	}

	//Создаёт заглавный текст.
	Text CreateBigText(string Str, Vector2f Position) {
		Text SmallText;
		SmallText.setFont(TextFont);
		SmallText.setCharacterSize(20);
		SmallText.setOutlineColor(sf::Color::White);
		SmallText.setString(Str);
		SmallText.setPosition(Position);
		return SmallText;
	}

	//Создаёт динамичный текст.
	Text CreateDynoText(string Str, Vector2f Position) {
		Text SmallText;
		SmallText.setFont(TextFont);
		SmallText.setCharacterSize(14);
		SmallText.setFillColor(sf::Color::Green);
		SmallText.setString(Str);
		SmallText.setPosition(Position);
		return SmallText;
	}

	//Проверяет, находится ли курсор внутри прямоугольной области.
	bool isCursorInRect(RenderWindow *window, Vector2f StartPos, Vector2f EndPos) {
		Vector2f Coords = window->mapPixelToCoords(Mouse::getPosition(*window));
		if (Coords.x >= StartPos.x && Coords.x <= EndPos.x && Coords.y >= StartPos.y && Coords.y <= EndPos.y) return TRUE; else return FALSE;
	}

	//Записывает сообщение для редактора тайлов.
	void _SendMessage() {
		ofstream MESSAGE(Directory + "Data\\DIALOG_TILE.txt");
		MESSAGE << Lines[NameUI].getString().toAnsiString() << endl;
		MESSAGE << Style << endl;
		MESSAGE << (int)SelectorSprite.getPosition().x / 32 + 1 << endl;
		MESSAGE << (int)SelectorSprite.getPosition().y / 32 + 1 << endl;
		MESSAGE << TexturePack << endl;
		MESSAGE << CHMAP[(int)SelectorSprite.getPosition().y / 32][(int)SelectorSprite.getPosition().x / 32] << endl;
		MESSAGE.close();
	}


	//Читает изменения тайла из файла диалога.
	void _GetMessage() {
		ifstream MESSAGE(Directory + "Data\\DIALOG_TILE.txt");
		string *Bufer = new string;
		unsigned int *X = new unsigned int;
		unsigned int *Y = new unsigned int;
		unsigned int *ID = new unsigned int;
		getline(MESSAGE, *Bufer);
		MESSAGE.close();
		if (isStringComplemetarityString(*Bufer, "-->")) {
			*Bufer = DeleteFirstSymbols(*Bufer, 3);
			*X = atoi(DeleteLastSymbols(*Bufer, FindFirstSymbol(*Bufer, '_')).c_str());
			*Bufer = DeleteFirstSymbols(*Bufer, FindFirstSymbol(*Bufer, '_') + 1);
			*Y = atoi(DeleteLastSymbols(*Bufer, FindFirstSymbol(*Bufer, '=')).c_str());
			*Bufer = DeleteFirstSymbols(*Bufer, FindFirstSymbol(*Bufer, '=') + 1);
			*ID = atoi((*Bufer).c_str());
			CHMAP[*Y - 1][*X - 1] = *ID;
		}
		delete Bufer;
		delete X, Y, ID;
	}

	//Сохраняет чанк.
	void SaveCHUNK() {
		ofstream File(CHDR);
		File << "[CHUNK]" << endl;
		for (int H = 0; H < 16; H++) for (int W = 0; W < 16; W++) File << H << '_' << W << " = " << CHMAP[H][W] << endl;
		File.close();
	}

	//Загружает чанк.
	void LoadCHUNK() {
		for (int H = 0; H < 16; H++) for (int W = 0; W < 16; W++) {
			char Bufer[256];
			string Key = NUMtoSTRING(H) + "_" + NUMtoSTRING(W);
			GetPrivateProfileString("CHUNK", Key.c_str(), NULL, Bufer, 256, CHDR.c_str());
			CHMAP[H][W] = atoi(Bufer);
		}
	}

	//
	void OpenCHUNK() {
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		string *ADR = new string(Directory + "Data\\DIALOG_OPEN.exe");
		ShExecInfo.lpFile = (*ADR).c_str();
		ShExecInfo.lpParameters = "";
		string *DIR = new string(Directory + "Data");
		ShExecInfo.lpDirectory = (*DIR).c_str();
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}

	//Сохраняет настройки.
	void SaveSETTINGS() {
		ofstream File("Data\\SETTINGS.INI");
		File << "[SETTINGS]" << endl;
		File << Keys[0] << " = " << BOOLtoSTRING(Settings[SETTINGS::CellsON]) << endl;
	}

	//Загружает настройки.
	void LoadSETTINGS() {
		char Bufer[256];
		string *ADR = new string(Directory + "Data\\SETTINGS.INI");
		for (int i = 0; i < Keys.size(); i++) {
			GetPrivateProfileString("SETTINGS", Keys[i].c_str(), NULL, Bufer, 256, (*ADR).c_str());
			Settings[i] = STRINGtoBOOL(Bufer);
		}
		delete ADR;
	}

public:

	//Отрисовка сетки.
	void UpdateCells(RenderWindow *window) {
		if (Settings[SETTINGS::CellsON]) {
			for (int H = 1; H < 16; H++) {
				Cells.setPosition(Vector2f(0, 32 * H));
				Cells.setSize(Vector2f(512, 1));
				(*window).draw(Cells);
			}
			for (int W = 1; W < 16; W++) {
				Cells.setPosition(Vector2f(32 * W, 0));
				Cells.setSize(Vector2f(1, 512));
				(*window).draw(Cells);
			}
		}
	}

	//Обновление селектора и его отрисовка.
	void UpdateSelector(RenderWindow *window) {
		Vector2f Position = window->mapPixelToCoords(Mouse::getPosition(*window));
		if (isCursorInRect(window, Vector2f(0, 0), Vector2f(511, 511))) {
			///Преобразование координат селектора.
			Position.x = (int)Position.x / 32 * 32; Position.y = (int)Position.y / 32 * 32;
			///Установка координат.
			SelectorSprite.setPosition(Position);
			///Отрисовка селектора.
			(*window).draw(SelectorSprite);
	
			///Запуск редактора тайла по нажатию кнопки.
			if (Mouse::isButtonPressed(Mouse::Left) && !isClicked) {
				_SendMessage();
				SHELLEXECUTEINFO ShExecInfo = { 0 };
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShExecInfo.hwnd = NULL;
				ShExecInfo.lpVerb = NULL;
				string *ADR = new string(Directory + "Data\\DIALOG_ID.exe");
				ShExecInfo.lpFile = (*ADR).c_str();
				ShExecInfo.lpParameters = "";
				string *DIR = new string(Directory + "Data");
				ShExecInfo.lpDirectory = (*DIR).c_str();
				ShExecInfo.nShow = SW_SHOW;
				ShExecInfo.hInstApp = NULL;
				ShellExecuteEx(&ShExecInfo);
				WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
				_GetMessage();
				isClicked = true;
				delete ADR, DIR;
			}
			if (!Mouse::isButtonPressed(Mouse::Left)) isClicked = false;

		}
	}

	//Обновление кнопок навигации.
	void UpdateButtons(RenderWindow *window) {

		///Проверка наведения курсора.
		if (isCursorInRect(window, Vector2f(628, 432), Vector2f(628 + 64, 496))) { 
			Button1.setColor(Color::Blue); 
			if (Mouse::isButtonPressed(Mouse::Left)) CurrentButton = ButtonGreen;
		}
		else Button1.setColor(Color::White);
		if (isCursorInRect(window, Vector2f(712, 432), Vector2f(712 + 64, 496))) { 
			Button2.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left)) CurrentButton = ButtonYellow;
		}
		else Button2.setColor(Color::White);
		if (isCursorInRect(window, Vector2f(796, 432), Vector2f(796 + 64, 496))) { 
			Button3.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left)) CurrentButton = ButtonRed;
		}
		else Button3.setColor(Color::White);

		///Установка рамки для кнопки нынешнего слоя.
		OnlineButton.setPosition(Vector2f(623 + 84 * (CurrentButton - 1), 427));

		///Отрисовка кнопок.
		(*window).draw(OnlineButton);
		(*window).draw(Button1);
		(*window).draw(Button2);
		(*window).draw(Button3);
	}

	//Обновление красного меню.
	void UpdateRed(RenderWindow *window) {

		///Обработка кнопки сохранения и выхода.
		if (isCursorInRect(window, Vector2f(630, 372), Vector2f(858, 412))) {
			SaveAndExit.setFillColor(Color(0, 100, 0));
			if (Mouse::isButtonPressed(Mouse::Left) && !isSaveAndExitClicked) {
				isSaveAndExitClicked = TRUE;
				SaveCHUNK();
				SaveSETTINGS();
				exit(0);
			}
		} else SaveAndExit.setFillColor(Color(255, 69, 0));
		if (!Mouse::isButtonPressed(Mouse::Left)) isSaveAndExitClicked = FALSE;

		///Обработка кнопки диалога открытия чанка.
		if (isCursorInRect(window, Vector2f(630, 262), Vector2f(858, 302))) {
			OpenChunk.setFillColor(Color(0, 100, 0));
			if (Mouse::isButtonPressed(Mouse::Left) && !isOpenChunkClicked) {
				isOpenChunkClicked = TRUE;
				
			}
		}
		else OpenChunk.setFillColor(Color(255, 69, 0));
		if (!Mouse::isButtonPressed(Mouse::Left)) isOpenChunkClicked = FALSE;

		///Отображение рамки около CheckBox, на который наведён курсор. Также переключение состояня CheckBox.
		if (isCursorInRect(window, Vector2f(628, 332), Vector2f(724, 357))) { 
			FocusTextBox.setPosition(627, 331);
			(*window).draw(FocusTextBox);
			if (Mouse::isButtonPressed(Mouse::Left) && !isCellsCheckBox) {
				isCellsCheckBox = TRUE;
				if (Settings[SETTINGS::CellsON]) Settings[SETTINGS::CellsON] = FALSE; else Settings[SETTINGS::CellsON] = TRUE;
			};
		}
		if (!Mouse::isButtonPressed(Mouse::Left)) isCellsCheckBox = FALSE;

		///Переключение режима CheckBox.
		if (Settings[SETTINGS::CellsON]) CellsCheckBox.setTexture(CheckBoxONTexture); else CellsCheckBox.setTexture(CheckBoxOFFTexture);
		
		///Отрисовка элементов.
		(*window).draw(CellsCheckBox);
		(*window).draw(SaveAndExit);
		(*window).draw(OpenChunk);
	}

	//Открытие текстурпака для его редактирования.
	CHUNK(string Directory, string TexturePack, string Style, string CHDR){
		this->TexturePack = DeleteLastSymbols(TexturePack, string("CONFIG.INI").length());
		this->Style = Style;
		this->Directory = DeleteLastSymbols(Directory, string("TileMap Editor.exe").length());
		this->CHDR = CHDR;

		///Загрузка свойств текста.
		{
			Text *TEXTbuf = new Text;
			TextFont.loadFromFile(this->Directory + "Data\\Arialuni.ttf");
			*TEXTbuf = CreateBigText("Texturepack information", Vector2f(530, 0)); Lines.push_back(*TEXTbuf);
			*TEXTbuf = CreateSmallText("Pack name: ", Vector2f(530, 30)); Lines.push_back(*TEXTbuf);
			//Буфер для читаемого из файла конфигурации значения.
			char Bufer[256];
			GetPrivateProfileString("CONFIGURATION", "PACK_NAME", NULL, Bufer, 256, TexturePack.c_str());
			*TEXTbuf = CreateDynoText(string(Bufer), Vector2f(610, 30)); Lines.push_back(*TEXTbuf);

			*TEXTbuf = CreateSmallText("Made by: ", Vector2f(780, 30)); Lines.push_back(*TEXTbuf);
			GetPrivateProfileString("CONFIGURATION", "BY", NULL, Bufer, 256, TexturePack.c_str());
			*TEXTbuf = CreateDynoText(string(Bufer), Vector2f(845, 30)); Lines.push_back(*TEXTbuf);

			*TEXTbuf = CreateSmallText("Supported styles: ", Vector2f(530, 60)); Lines.push_back(*TEXTbuf);
			GetPrivateProfileString("DESCRIPTION", "STYLES ", NULL, Bufer, 256, TexturePack.c_str());
			*TEXTbuf = CreateDynoText(string(Bufer), Vector2f(650, 60)); Lines.push_back(*TEXTbuf);

			*TEXTbuf = CreateBigText("Tile description", Vector2f(530, 90)); Lines.push_back(*TEXTbuf);
			*TEXTbuf = CreateSmallText("X-axis: ", Vector2f(530, 120)); Lines.push_back(*TEXTbuf);
			*TEXTbuf = CreateDynoText(NUMtoSTRING(SelectorSprite.getPosition().x / 32 + 1), Vector2f(580, 120)); Lines.push_back(*TEXTbuf);
			*TEXTbuf = CreateSmallText("Y-axis: ", Vector2f(610, 120)); Lines.push_back(*TEXTbuf);
			*TEXTbuf = CreateDynoText(NUMtoSTRING(SelectorSprite.getPosition().y / 32 + 1), Vector2f(660, 120)); Lines.push_back(*TEXTbuf);

			*TEXTbuf = CreateSmallText("Tile ID: ", Vector2f(700, 120)); Lines.push_back(*TEXTbuf);
			*TEXTbuf = CreateDynoText("--", Vector2f(750, 120)); Lines.push_back(*TEXTbuf);

			///Надписи для красного меню.
			*TEXTbuf = CreateBigText("Save & Exit", Vector2f(628 + 65, 372 + 6)); LinesRed.push_back(*TEXTbuf);
			*TEXTbuf = CreateBigText("Module information", Vector2f(530, 0)); LinesRed.push_back(*TEXTbuf);
			*TEXTbuf = CreateSmallText("Version: ", Vector2f(530, 30)); LinesRed.push_back(*TEXTbuf);
			*TEXTbuf = CreateDynoText(MODULE_VERSION + " (" + CODE_NAME + ")", Vector2f(590, 30)); LinesRed.push_back(*TEXTbuf);
			*TEXTbuf = CreateSmallText("Grid: ", Vector2f(628, 312)); LinesRed.push_back(*TEXTbuf);
			*TEXTbuf = CreateDynoText("???", Vector2f(663, 312)); LinesRed.push_back(*TEXTbuf);
			*TEXTbuf = CreateBigText("Open chunk", Vector2f(628 + 60, 262 + 6)); LinesRed.push_back(*TEXTbuf);

			///Надписи для зелёного меню.
			*TEXTbuf = CreateBigText("Instruments", Vector2f(530, 0)); LinesGreen.push_back(*TEXTbuf);

			delete TEXTbuf;
		}

		///Загрузка чанка.
		LoadCHUNK();

		///Загрузка пакета текстур.
		Texture *TXbuf = new Texture;
		while ((*TXbuf).loadFromFile(this->TexturePack + Style + "\\" + NUMtoSTRING(PNGS + 1) + ".png")) { 
			Textures.push_back(*TXbuf);
			PNGS++; 
		}
		delete TXbuf;

		///Загрузка селектора.
		SelectorTexture.loadFromFile(this->Directory + "Data\\Selector.png");
		SelectorSprite.setTexture(SelectorTexture);

		///Загрузка рамки для кнопок слоёв.
		OnlineButtonTexture.loadFromFile(this->Directory + "Data\\CurrentButton.png");
		OnlineButton.setTexture(OnlineButtonTexture);

		///Загрузка границы зоны чанка.
		BorderTexture.loadFromFile(this->Directory + "Data\\Border.png");
		BorderSprite.setTexture(BorderTexture);
		BorderSprite.setPosition(512, 0);

		///Загрузка кнопок управления.
		{
			Button1Texture.loadFromFile(this->Directory + "Data\\Button1.png");
			Button2Texture.loadFromFile(this->Directory + "Data\\Button2.png");
			Button3Texture.loadFromFile(this->Directory + "Data\\Button3.png");
			Button1.setTexture(Button1Texture);
			Button2.setTexture(Button2Texture);
			Button3.setTexture(Button3Texture);
			Button1.setPosition(628, 432);
			Button2.setPosition(712, 432);
			Button3.setPosition(796, 432); 
		}

		///Генерация элементов красного меню.
		{
			SaveAndExit.setPosition(630, 372);
			SaveAndExit.setSize(Vector2f(228, 40));
			SaveAndExit.setOutlineThickness(2);
			SaveAndExit.setOutlineColor(Color::Black);
			CheckBoxONTexture.loadFromFile(this->Directory + "Data\\CheckBoxON.png");
			CheckBoxOFFTexture.loadFromFile(this->Directory + "Data\\CheckBoxOFF.png");
			CellsCheckBox.setPosition(628, 332); 
			FocusTextBox.setSize(Vector2f(98, 27));
			FocusTextBox.setFillColor(Color::White);

			OpenChunk.setPosition(630, 262);
			OpenChunk.setSize(Vector2f(228, 40));
			OpenChunk.setOutlineThickness(2);
			OpenChunk.setOutlineColor(Color::Black);
		}

		///Установка настроек по умолчанию.
		///-->CellsON
		Settings.push_back(TRUE);

		///Загрузка текстуры ошибки.
		ErrorTexture.loadFromFile("Data\\Error.png");

		///Чтение настроек.
		Cells.setFillColor(Color::Black);
		Keys.push_back("GRID");
		LoadSETTINGS();
	}

	//Обновление рабочей области.
	void update(RenderWindow *window) {
		///Обновление тайлов чанка.
		for (int H = 0; H < 16; H++) for (int W = 0; W < 16; W++) {
			///Установка нужной текстуры для спрайта.
			if (CHMAP[H][W] <= PNGS && CHMAP[H][W] > 0) {
				CurrentSprite.setTexture(Textures[CHMAP[H][W] - 1]);
			} else CurrentSprite.setTexture(ErrorTexture);
			///Установка параметров спрайта.
			CurrentSprite.setPosition(32 * W, 32 * H);
			///Отрисовка спрайтов и элементов управления.
			if (CHMAP[H][W] != 0)(*window).draw(CurrentSprite);
		}
		///Отрисовка границы зоны чанка.
		(*window).draw(BorderSprite);

		UpdateSelector(window);
		UpdateCells(window);
		UpdateButtons(window);

		if (CurrentButton == ButtonGreen) {
			///Обновление надписей.
			for (int i = 0; i < LinesGreen.size(); i++) (*window).draw(LinesGreen[i]);
		}

		if (CurrentButton == ButtonYellow) {
			///Обновление надписей.
			for (int i = 0; i < Lines.size(); i++) (*window).draw(Lines[i]);
			///Обновление информации о тайле.
			if (isCursorInRect(window, Vector2f(0, 0), Vector2f(511, 511))) {
				Lines[X_coordUI].setString(NUMtoSTRING(SelectorSprite.getPosition().x / 32 + 1));
				Lines[Y_coordUI].setString(NUMtoSTRING(SelectorSprite.getPosition().y / 32 + 1));
				Lines[IDUI].setString(NUMtoSTRING(CHMAP[(int)SelectorSprite.getPosition().y / 32][(int)SelectorSprite.getPosition().x / 32]));
			}
			else {
				Lines[X_coordUI].setString("--");
				Lines[Y_coordUI].setString("--");
				Lines[IDUI].setString("--");
			}
		}

		if (CurrentButton == ButtonRed) {
			UpdateRed(window);

			///Переключение текста над CheckBox.
			if (Settings[SETTINGS::CellsON]) { LinesRed[GridUI].setFillColor(Color::Green); LinesRed[GridUI].setString("ON"); }
			else { LinesRed[GridUI].setFillColor(Color::Red); LinesRed[GridUI].setString("OFF"); }

			///Обновление надписей.
			for (int i = 0; i < LinesRed.size(); i++) (*window).draw(LinesRed[i]);
		}
	}

};

int main(int argc, char* argv[]) {

	///Включение русского языка.
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	///Проверка наличия аргумента с адресом. Уведомление и выход в случае отсутствия аргумента.
	///if (argc == 1) { MessageBox(NULL, "Couldn't open texturepack configuration file.\nThe module will be closed.", "Notification", MB_ICONERROR); exit(0); }

	RenderWindow window(VideoMode(960, 512), "TileMap Editor", Style::Close);

	CHUNK CH(argv[0], "C:\\Users\\Влад\\Documents\\Visual Studio 2017\\Projects\\CHUNK\\x64\\Debug\\Texturepack\\CONFIG.INI", "Desert", "C:\\Users\\Влад\\Documents\\Visual Studio 2017\\Projects\\CHUNK\\x64\\Debug\\CH=1_1.INI");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(Color(45, 45, 48));
		
		CH.update(&window);

		window.display();
	}
	

	return EXIT_SUCCESS;
}