#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <chrono>

std::ifstream fin("sudoku.in");

int sudoku[10][10], retOrigSudoku[10][10];
bool used[10][10];

bool valid(int p, int q, int numar)
{
	for (int j = 1; j <= 9; ++j)
		if (sudoku[p][j] == numar)
			return false;

	for (int i = 1; i <= 9; ++i)
		if (sudoku[i][q] == numar)
			return false;

	for (int i = ((p - 1) / 3) * 3 + 1; i <= ((p - 1) / 3) * 3 + 3; ++i)
		for (int j = ((q - 1) / 3) * 3 + 1; j <= ((q - 1) / 3) * 3 + 3; ++j)
			if (sudoku[i][j] == numar)
				return false;

	sudoku[p][q] = numar; // daca am ajuns aici inseamna ca valoarea poate fi buna, ramane sa vedem in functie de celelalte

	return true;
}

bool locLiber(int& x, int& y)
{
	for (x = 1; x <= 9; ++x)
		for (y = 1; y <= 9; ++y)
			if (!sudoku[x][y]) // i si j raman salvate deoarece avem adrese la parametri
				return true;

	return false;
}

bool rezolvaSudoku()
{
	int linie, coloana;
	if (!locLiber(linie, coloana)) //daca nu gaseste un loc liber inseamna ca totul este ocupat => avem solutie
		return true;

	for (int i = 1; i <= 9; ++i)
		if (valid(linie, coloana, i))
		{
			if (rezolvaSudoku()) // merge in adancime pana cand nu se mai poate pentru a gasi si celelalte valori
				return true;

			sudoku[linie][coloana] = 0; // daca o valoare pusa nu este buna, ma reintorc pe unde am trecut si resetez valorile puse cu 0
		}

	return false;
}

bool checkSudokuCompleted()
{
	for (int i = 1; i <= 9; ++i)
		for (int j = 1; j <= 9; ++j)
			if (!used[i][j])
				return false;

	return true;
}

int main()
{
	const char entryText[] = "Bine ai venit!\n\n"
							  "Sudoku este un joc popular de logica, un puzzle. Jocul consta in rezolvarea unui tablou cu 9 linii si 9 coloane in cel mai scurt timp posibil "
							  "astfel incat pe fiecare linie si pe fiecare coloana sa se afle toate numerele de la 1 la 9 fara ca acestea sa se repete. "
							  "Asta înseamna ca, in fiecare rand, coloana, si patratul delimitat cu linie groasa, trebuie sa fie tocmai cifrele de la 1 la 9 (1, 2, 3, 4, 5, 6, 7, 8, 9). Introducerea valorilor "
							  "se face inserand, pe rand, numarul liniei si a coloanei elementului pe care vrem sa il inseram, iar apoi valoarea acestuia. Daca elementul face parte din rezolvarea "
							  "jocului acesta va fi inserat in tabel, in caz contrar acesta nu va fi inserat. Timpul scurs va fi afisat in partea dreapta-jos a tablei. \n\nSucces!";
	MessageBox(NULL, entryText, "Informatii Sudoku", MB_OK);

	sf::RenderWindow window(sf::VideoMode(1024, 740), "Sudoku!");
	window.setFramerateLimit(60);

	sf::Image image;
	if (!image.loadFromFile("sudoku.png"))
	{
		MessageBox(NULL, "Nu gasesc imaingea sudoku.png :(", "Eroare", MB_OK);
		return 0;
	}

	sf::Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		MessageBox(NULL, "Nu gasesc fontul arial.ttf :(", "Eroare", MB_OK);
		return 0;
	}

	sf::Texture texture;
	texture.loadFromImage(image);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	for (int i = 1; i <= 9; ++i)
		for (int j = 1; j <= 9; ++j)
		{
			fin >> sudoku[i][j];
			retOrigSudoku[i][j] = sudoku[i][j];
		}

	bool ok = rezolvaSudoku();

	sf::Text faraSolutie;
	faraSolutie.setString("Nu exista solutie! Unlucky :(");
	faraSolutie.setFont(font);
	faraSolutie.setCharacterSize(64);
	faraSolutie.setFillColor(sf::Color::Cyan);
	faraSolutie.setStyle(sf::Text::Bold);
	faraSolutie.setPosition((1024 - faraSolutie.getLocalBounds().width) / 2.0f, (720 - faraSolutie.getLocalBounds().height) / 2.0f);

	int i = 1, j = 1;
	std::vector<sf::Text> texts(82);
	for (auto it = begin(texts) + 1; it != end(texts); ++it)
	{
		if (j == 10)
			j = 1, ++i;
		it->setString(std::to_string(sudoku[i][j++]));
		it->setFont(font);
		it->setCharacterSize(64);
		it->setFillColor(sf::Color::Red);
		it->setStyle(sf::Text::Bold);
	}

	bool sudokuCompleted = false;
	bool update = false;

	window.requestFocus();

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	while (window.isOpen())
	{
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		sf::Text autor;
		autor.setString("Timp scurs: " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()) + "s");
		autor.setFont(font);
		autor.setCharacterSize(24);
		autor.setFillColor(sf::Color::Cyan);
		autor.setStyle(sf::Text::Bold);
		autor.setPosition(1024 - faraSolutie.getLocalBounds().width / 4 - 40, 720 - faraSolutie.getLocalBounds().height / 4);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::GainedFocus)
				update = true;

			if (event.type == sf::Event::LostFocus)
				update = false;
		}

		window.clear();
		window.draw(sprite);
		window.draw(autor);

		if (ok)
		{
			if (update && !sudokuCompleted)
			{
				int x, y, val;
				std::cout << "Linia elementului: ";
				std::cin >> x;
				std::cout << "Coloana elementului: ";
				std::cin >> y;
				if (used[x][y])
				{
					std::cout << "Elementul este deja stabilit. Introdu alte valori.\n\n";
					continue;
				}

				std::cout << "Valoarea elementului de pe linia " << x << " si coloana " <<  y << ": ";
				std::cin >> val;
				if (val < 1 || val > 9)
				{
					std::cout << "Introdu doar valori intre 1 si 9.\n";
					continue;
				}

				if (val == sudoku[x][y])
				{
					std::cout << "Valoarea introdusa este corecta. Aceasta a fost plasata in tabel.\n";
					texts[(x - 1) * 9 + y].setPosition(60.f * y + 45.f * (y - 1), 25.f * x + 48.f * (x - 1));
					window.draw(texts[(x - 1) * 9 + y]);
					used[x][y] = true;
				}
				else
				{
					std::cout << "Valoarea introdusa este incorecta. Aceasta nu a fost plasata in tabel.\n";
					continue;
				}
			}

			for (int i = 1, z = 0; i <= 9; ++i)
				for (int j = 1; j <= 9; ++j)
				{
					++z;
					if (retOrigSudoku[i][j])
					{
						texts[z].setPosition(60.f * j + 45.f * (j - 1), 25.f * i + 48.f * (i - 1));
						window.draw(texts[z]);
						used[i][j] = true;
					}
					else if (used[i][j])
					{
						texts[z].setPosition(60.f * j + 45.f * (j - 1), 25.f * i + 48.f * (i - 1));
						window.draw(texts[z]);
					}
				}

			sudokuCompleted = checkSudokuCompleted();
			if (sudokuCompleted)
			{
				window.display();
				std::cout << "\n BRAVO!\n\n";
				Sleep(60 * 1000); // 60 de secunde
				window.close();
			}
		}
		else
		{
			window.draw(faraSolutie);
			sudokuCompleted = true;
		}

		window.display();
	}
}