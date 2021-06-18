#include<iostream>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<conio.h>
#include<cstring>
#include<windows.h>
#include<fstream>
#include<vector>
#include <random>

#define ILOSC_ELEMENTOW 81
#define N 2000000
using namespace std;

/*

- okreslenie wszystkich mozliwych cyfr w kazdym wolnym kwadracie
- wyszukiwanie rozwiazan metoda min-max z alpha-beta prunning
- dodac sensowne dodawanie rozwiazanego sudoku, zeby pozniej nie sprawdzac wszystkich czy juz bylo takie sudoku

*/

unsigned long long  ILOSC_ROZWIAZANYCH_SUDOKU = 0;

struct Rozwiazania
{
	Rozwiazania() {
	}
	Rozwiazania(unsigned Sudoku[9][9]) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				sudoku[i][j] = Sudoku[i][j];
			}
		}
	}
	unsigned sudoku[9][9];
};

struct TrudneSudoku {
	TrudneSudoku() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				sudoku[i][j] = 0;
			}
		}

		liczbaCyfrWejsciowych = 0;
		liczbaRozwiazan = 100000;
	}
	TrudneSudoku(unsigned Sudoku[9][9], unsigned LiczbaCyfrWejsciowych, unsigned LiczbaRozwiazan) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				sudoku[i][j] = Sudoku[i][j];
			}
		}
		liczbaCyfrWejsciowych = LiczbaCyfrWejsciowych;
		liczbaRozwiazan = LiczbaRozwiazan;
	}
	unsigned liczbaCyfrWejsciowych;
	unsigned liczbaRozwiazan;
	unsigned sudoku[9][9];
};

struct MozliweCyfry {
	vector<unsigned int > cyfryZajete;
	vector<unsigned int > cyfryDoUzycia;
	unsigned wiersz, kolumna;
};

vector < Rozwiazania > tablica(N);
vector < TrudneSudoku > trudneSudoku(N);

vector <MozliweCyfry > mozliweCyfry(ILOSC_ELEMENTOW);

void wypelnij(unsigned sudoku[9][9])
{
	for (int wiersz = 0; wiersz < 9; wiersz++)
		for (int kolumna = 0; kolumna < 9; kolumna++)
			cin >> sudoku[wiersz][kolumna];
}

void wypelnij0(unsigned sudoku[9][9])
{
	for (int wiersz = 0; wiersz < 9; wiersz++)
		for (int kolumna = 0; kolumna < 9; kolumna++)
			sudoku[wiersz][kolumna] = 0;
}

void wypisz(unsigned sudoku[9][9])
{
	cout << "\n\n\n";

	HANDLE hOut;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	for (unsigned wiersz = 0; wiersz < 9; wiersz++)
	{
		for (unsigned kolumna = 0; kolumna < 9; kolumna++)
		{
			if (sudoku[wiersz][kolumna] == 0)
			{
				SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
				cout << sudoku[wiersz][kolumna] << " ";
			}
			else
			{
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				cout << sudoku[wiersz][kolumna] << " ";
			}
		}
		cout << endl;
	}
}

void stale(unsigned sudoku[9][9], unsigned tablica[9][9])
{
	wypelnij0(tablica);
	for (unsigned wiersz = 0; wiersz < 9; wiersz++)
	{
		for (unsigned kolumna = 0; kolumna < 9; kolumna++)
		{
			if (sudoku[wiersz][kolumna] != 0)
			{
				tablica[wiersz][kolumna] = sudoku[wiersz][kolumna];
			}
		}
	}
}

void wypisz(vector<Rozwiazania>& rozwiazania) {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "\n\n\n";
	unsigned licznik = 0;
	for (unsigned i = 0; i < ILOSC_ROZWIAZANYCH_SUDOKU; i++)
	{
		cout << endl << licznik + 1 << ". " << endl;
		for (unsigned wiersz = 0; wiersz < 9; wiersz++)
		{
			for (unsigned kolumna = 0; kolumna < 9; kolumna++)
			{
				SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				cout << tablica[i].sudoku[wiersz][kolumna] << " ";
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			}
			cout << endl;
		}
		licznik++;
	}
}
void wypisz(vector<TrudneSudoku>& rozwiazania, unsigned granicaDolna, unsigned granicaGorna) {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "\n\n\n";
	unsigned licznik = 0;
	for (unsigned i = granicaDolna; i < granicaGorna; i++)
	{
		cout << endl << licznik + 1 << ".\n LiczbaCyfr : " << ::trudneSudoku[i].liczbaCyfrWejsciowych;
		cout << endl << "Liczba Rozwiazan : " << ::trudneSudoku[i].liczbaRozwiazan << endl;
		for (unsigned wiersz = 0; wiersz < 9; wiersz++)
		{
			for (unsigned kolumna = 0; kolumna < 9; kolumna++)
			{
				SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				cout << trudneSudoku[i].sudoku[wiersz][kolumna] << " ";
				SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			}
			cout << endl;
		}
		licznik++;
	}
}

bool CzyByla(vector< unsigned int >& tablica, unsigned cyfra)
{
	if (tablica.size() == 0)
		return false;

	for (unsigned i = 0; i < tablica.size(); i++)
		if (cyfra == tablica[i])
			return true;

	return false;
}

void wypelijCyfryDoUzycia(vector< unsigned int >& zajete, vector< unsigned int >& doUzycia) {
	bool jest = false;
	for (int i = 1; i <= 9; i++) {
		bool jest = false;
		for (int j = 0; j < zajete.size(); j++) {
			if (zajete[j] == i) {
				jest = true;
				break;
			}
		}
		if (!jest)
			doUzycia.push_back(i);
	}
}

void MetodaKratkowa(unsigned sudoku[9][9], vector< unsigned int >& Npowt, unsigned wiersz, unsigned kolumna)
{
	for (unsigned k = 0; k < 9; k++)
		if (CzyByla(Npowt, sudoku[wiersz][k]) == false)
		{
			Npowt.push_back(sudoku[wiersz][k]);
		}

	for (unsigned w = 0; w < 9; w++)
		if (CzyByla(Npowt, sudoku[w][kolumna]) == false)
		{
			Npowt.push_back(sudoku[w][kolumna]);
		}

	for (unsigned w2 = (wiersz / 3) * 3; w2 < ((wiersz / 3) * 3 + 3); w2++)
		for (unsigned k2 = (kolumna / 3) * 3; k2 < ((kolumna / 3) * 3 + 3); k2++)
			if (CzyByla(Npowt, sudoku[w2][k2]) == false)
			{
				Npowt.push_back(sudoku[w2][k2]);
			}
}

void nadpisz(unsigned sudoku[9][9], unsigned sudoku1[9][9])
{
	for (int wiersz = 0; wiersz < 9; wiersz++)
		for (int kolumna = 0; kolumna < 9; kolumna++)
			sudoku1[wiersz][kolumna] = sudoku[wiersz][kolumna];
}

void zapisz(Rozwiazania tablica[], unsigned ilosc)
{
	ofstream wyjscie("sudoku.txt");
	for (unsigned i = 0; i < ilosc; i++)
	{
		wyjscie << i + 1 << ". " << endl;
		for (unsigned wiersz = 0; wiersz < 9; wiersz++)
		{
			for (unsigned kolumna = 0; kolumna < 9; kolumna++)
				wyjscie << tablica[i].sudoku[wiersz][kolumna] << "  ";

			wyjscie << "\n";
		}
		wyjscie << "\n\n\n";
	}
	cout << "WYNIKI ZOSTALY ZAPISANE W PLIKU O NAZWIE 'sudoku'\n";
}

void wypelnijMozliweCyfry(vector <MozliweCyfry >& mozliweCyfry, unsigned sudoku[9][9]) {
	for (int wiersz = 0; wiersz < 9; wiersz++) {
		for (int kolumna = 0; kolumna < 9; kolumna++) {
			if (!sudoku[wiersz][kolumna]) {
				MetodaKratkowa(sudoku, mozliweCyfry[wiersz * 9 + kolumna].cyfryZajete, wiersz, kolumna);
				wypelijCyfryDoUzycia(mozliweCyfry[wiersz * 9 + kolumna].cyfryZajete, mozliweCyfry[wiersz * 9 + kolumna].cyfryDoUzycia);
			}

			mozliweCyfry[wiersz * 9 + kolumna].wiersz = wiersz;
			mozliweCyfry[wiersz * 9 + kolumna].kolumna = kolumna;
		}
	}
}

void wypiszMozliweCyfry(vector <MozliweCyfry >& mozliweCyfry) {
	cout << endl;
	for (int wiersz = 0; wiersz < 9; wiersz++) {
		for (int kolumna = 0; kolumna < 9; kolumna++) {
			cout << "( " << mozliweCyfry[wiersz * 9 + kolumna].wiersz << " , " << mozliweCyfry[wiersz * 9 + kolumna].kolumna << " ) : ";
			for (int i = 0; i < mozliweCyfry[wiersz * 9 + kolumna].cyfryDoUzycia.size(); i++) {
				cout << mozliweCyfry[wiersz * 9 + kolumna].cyfryDoUzycia[i];
			}
			cout << endl;
		}
	}
}

bool sprawdzCzyMozliwe(unsigned sudoku[9][9], unsigned wiersz, unsigned kolumna) {
	if (wiersz == 999 && kolumna == 999) return true;
	for (unsigned k = 0; k < 9; k++)
		if (k != kolumna)
			if (sudoku[wiersz][kolumna] == sudoku[wiersz][k])
				return false;

	for (unsigned w = 0; w < 9; w++)
		if (w != wiersz)
			if (sudoku[wiersz][kolumna] == sudoku[w][kolumna])
				return false;

	for (unsigned w2 = (wiersz / 3) * 3; w2 < ((wiersz / 3) * 3 + 3); w2++)
		for (unsigned k2 = (kolumna / 3) * 3; k2 < ((kolumna / 3) * 3 + 3); k2++)
			if (w2 != wiersz || k2 != kolumna)
				if (sudoku[wiersz][kolumna] == sudoku[w2][k2])
					return false;

	return true;
}

void minmax(unsigned sudoku[9][9], unsigned indeksSprawdzanejCyfry, unsigned wstawionyWiersz, unsigned wstawionaKolumna, unsigned long long& LICZBA_ROZWIAZANYCH_SUDOKU, unsigned MAX) {
	if (LICZBA_ROZWIAZANYCH_SUDOKU >= MAX) return;

	if (sprawdzCzyMozliwe(sudoku, wstawionyWiersz, wstawionaKolumna) == false) return;

	if (indeksSprawdzanejCyfry == ILOSC_ELEMENTOW) {
		tablica[LICZBA_ROZWIAZANYCH_SUDOKU++] = Rozwiazania(sudoku);
		if (LICZBA_ROZWIAZANYCH_SUDOKU % 10000 == 0)
			cout << LICZBA_ROZWIAZANYCH_SUDOKU << "\r";

		return;
	}

	unsigned sudoku2[9][9];

	nadpisz(sudoku, sudoku2);

	if (mozliweCyfry[indeksSprawdzanejCyfry].cyfryDoUzycia.size() == 0) {
		minmax(sudoku2, indeksSprawdzanejCyfry + 1, mozliweCyfry[indeksSprawdzanejCyfry].wiersz, mozliweCyfry[indeksSprawdzanejCyfry].kolumna, LICZBA_ROZWIAZANYCH_SUDOKU, MAX);
	}
	else {
		for (int j = 0; j < mozliweCyfry[indeksSprawdzanejCyfry].cyfryDoUzycia.size(); j++) {
			sudoku2[mozliweCyfry[indeksSprawdzanejCyfry].wiersz][mozliweCyfry[indeksSprawdzanejCyfry].kolumna] = mozliweCyfry[indeksSprawdzanejCyfry].cyfryDoUzycia[j];
			minmax(sudoku2, indeksSprawdzanejCyfry + 1, mozliweCyfry[indeksSprawdzanejCyfry].wiersz, mozliweCyfry[indeksSprawdzanejCyfry].kolumna, LICZBA_ROZWIAZANYCH_SUDOKU, MAX);
		}
	}
}

void wyszukajTrudnychSudoku() {
	mt19937 rng;
	rng.seed(random_device()());
	uniform_int_distribution<mt19937::result_type> losowyWiersz(0, 8);
	uniform_int_distribution<mt19937::result_type> losowaKolumna(0, 8);

	unsigned sudoku[9][9];
	wypelnij0(sudoku);
	unsigned liczbaLosowan = 10000, liczbaNaTablicy = 26, wiersz, kolumna, maxRozwiazan = 10, licznikRozwiazan = 0;
	for (int i = 0; i < ILOSC_ROZWIAZANYCH_SUDOKU; i++) {
		if (i % 500 == 0)
			cout << "Przerabiam " << i << " sudoku\r";

		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < liczbaNaTablicy; k++) {
				do {
					wiersz = losowyWiersz(rng);
					kolumna = losowaKolumna(rng);
				} while (sudoku[wiersz][kolumna] != 0);

				sudoku[wiersz][kolumna] = ::tablica[i].sudoku[wiersz][kolumna];
			}

			//minmax(sudoku, 0, 999, 999, licznikRozwiazan, maxRozwiazan);

			if (::trudneSudoku[liczbaNaTablicy].liczbaRozwiazan > licznikRozwiazan)
				::trudneSudoku[liczbaNaTablicy] = TrudneSudoku(sudoku, liczbaNaTablicy, licznikRozwiazan);

			licznikRozwiazan = 0;
			wypelnij0(sudoku);
		}
	}
}

void rozwiazywanie(Rozwiazania tablica[], unsigned& ilosc)
{
	unsigned  sudoku1[9][9], sudokuStale[9][9], Npowt[10], zakres = 0, losowa, kombinacje = 0, tablica1[9][9];
	unsigned sudoku[9][9] = {
		{ 0,1,0,0,0,4,0,0,7},
		{ 3,5,0,0,0,0,0,0,0},
		{ 0,0,0,0,0,0,0,9,0},
		{ 0,8,0,0,0,0,4,0,9},
		{ 1,0,0,0,0,0,0,0,0},
		{ 0,0,5,0,0,8,2,0,0},
		{ 0,4,0,5,3,0,0,0,0},
		{ 0,0,0,0,0,0,7,0,0},
		{ 0,0,0,0,0,2,0,8,0}
	};
	cout << "nCzy chcesz miec sudoku wypelnione zerami ? y/n? \n ";
	char odp;
	cin >> odp;
	if (odp == 'y') {
		wypelnij0(sudoku);
	}
	else {
		cout << "Wypelnij sudoku, w puste miejsca wstaw 0\n";
		wypelnij(sudoku);
	}

	cout << "\n\n\n\nTWOJE SUDOKU \n";

	wypisz(sudoku);

	stale(sudoku, tablica1);

	wypelnijMozliweCyfry(mozliweCyfry, sudoku);

	minmax(sudoku, 0, 999, 999, ILOSC_ROZWIAZANYCH_SUDOKU, 10);

	cout << "Znaleziono : " << ILOSC_ROZWIAZANYCH_SUDOKU << " rozwiazan.\n";

	//wyszukajTrudnychSudoku();

	//wypisz(trudneSudoku, 25, 29);

	_getch();

	wypisz(::tablica);

	nadpisz(sudoku, sudokuStale);
}

int main()
{
	char znak;
	unsigned ilosc = 0;
	do
	{
		cout << "Opcje programu: \n";
		cout << "Rozwiazywanie Sudoku - nacisnij r\n";
		znak = _getch();
		switch (znak)
		{
		case 'r': rozwiazywanie(&tablica[0], ilosc); break;
		case 27: break;
		default: cout << "                   Nieprawidlowy klawisz!\n"; break;
		}
	} while (znak != 27);

	return 0;
}
// Torso & MojoJojo Production