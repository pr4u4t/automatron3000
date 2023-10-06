# Szpuler
## Główne okno programu
Okno składa się z menu górnego, paska statusu oraz centralnej kontrolki MDI (Multi Document Interface) umożliwiającej otwieranie okien funkcyjnych w głównym oknie programu. 
![alt text](https://github.com/pr4u4t/szpuler/blob/main/doc/main_window.png?raw=true)

## Ustawienia
Okno ustawień dostępne w menu górnym okna głównego serial/settings zawiera ustawienia portu serial/COM(rs232) służącego do komunikacji z urządzeniem arduino. Po lewej znajduje się menu wyboru portu umożliwiające wybranie portu lub wpisanie jego nazwy po wybraniu opcji custom. Po prawej znajdują się poddtawowe ustawienia portu serial takie np. baud rate, parity, flow control. Na dole okna znajdują się dwa pola wyboru, local echo oraz auto connect, którego zaznaczenie powoduje automatyczne otwarcie połączenia z portem szeregowym po uruchomieniu programu.
![alt text](https://github.com/pr4u4t/szpuler/blob/main/doc/settings.png?raw=true)

## Zdarzenia (Log)
Okno  dostępne w menu górnym programu file/log umożliwia podgląd wszystkich zdarzeń zachodzących w programie i akcji podejmowanych przez program. Służy diagnostyce programu. Dziennik zdarzeń zapisywany jest w katalogu programu w pliku szpuler.log. 
![alt text](https://github.com/pr4u4t/szpuler/blob/main/doc/log.png?raw=true)

## Konsola (Console)
Okno dostępne w menu górnym okna głównego file/console umożliwia bezpośrednie wysyłanie znaków wpisanych do urządzenia po porcie szeregowym. Służy diagnostyce urządzenia oraz komunikacji z nim. Dane wysyłane są poprzez główne połączenie otwarte w programie konfigurowalne w Ustawieniach.
![alt text](https://github.com/pr4u4t/szpuler/blob/main/doc/console.png?raw=true)

## Baza danych (Database)
Okno dostępne w menu górnym okna głównego file/database, służy skanowaniu części i wysyłaniu ich kodów do urządzenia podpiętego pod port szeregowy. Centralnym elementem tego okna jest kontrolka tabeli wyświetlająca podgląd bazy danych programu lub listę zeskanowanych kodów w zależności od wyboru użytkownika. 
![alt text](https://github.com/pr4u4t/szpuler/blob/main/doc/database.png?raw=true)
W przeciwieństwie do oryginalnej wersji programu kody nie są tu przechowywane w plikach tekstowych a w bazie danych SQL (SQLite3). Zmiana ta pozwoliła na dodanie funkcji importu oraz eksportu bazy z programu. Pod główną kontrolką znajdują się trzy przyciski odpowiadające za import i eksport:
- import dir
- import csv
- export csv

### import katalogu (import dir)
Uruchomienie tej funkcji służy importowi kodów szpul i półek z oryginalnej wersji programu. Po jej uruchomieniu wyświetla się okno dialogowe służące wyborowi katalogu w którym znajduje się baza oryginalnego szpulera w plikach *.txt. Po wybraniu katalogu i naciśnięciu przycisku "ok" import nastąpi automatycznie. Przykładowe okno dialogowe zaprezentowane poniżej. ** uwaga import spowoduje usunięcie poprzednich danych z bazy, jest to nieodwracalne **
![alt text](https://github.com/pr4u4t/szpuler/blob/main/doc/dialog.png?raw=true)

### import csv (import csv) 
Uruchomienie tej funkcji powoduje import bazy danych z pliku csv. Plik ten może być wygenerowany w MS excel'u lub Libre Calc ze standardowego arkusza kalkulacyjnego [Excel zapis csv](https://support.microsoft.com/pl-pl/office/zapisywanie-skoroszytu-w-formacie-tekstowym-txt-lub-csv-3e9a9d6c-70da-4255-aa28-fcacf1f081e6). Wyeksportowana z programu baza danych może zostać wczytana w arkuszu kalkulacyjnym tam obrobiona, a później z powrotem zaimportowana do programu.

### eksport csv (export csv)
Uruchomienie tej funkcji służy eksportowi bazy danych programu do pliku tekstowego, po jej uruchomieniu użytkownik podaje plik docelowy poprzez okno dialogowe do którego ma zostać zapisana baza. W bazie tej można później wprowadzić zmiany w programie obsługującym arkusze kalkulacyjne i na powrót zaimportować ją korzystając z opcji opisanej powyżej.
