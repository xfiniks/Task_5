#include <iostream>
#include <future>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

//Использооввание стандортной библиотеки.
using std::string, std::cin, std::cout, std::to_string, std::vector, std::shuffle,
std::begin, std::end, std::mt19937, std::async, std::launch, std::future, std::exception;

struct Monk {
  string name;
  long long chi{};

  friend bool operator>(const Monk &first_monk, const Monk &second_monk);
  friend bool operator<(const Monk &first_monk, const Monk &second_monk);

};
bool operator>(const Monk &first_monk, const Monk &second_monk) {
    return first_monk.chi > second_monk.chi;
}
bool operator<(const Monk &first_monk, const Monk &second_monk) {
    return first_monk.chi < second_monk.chi;
}

// Генерация рандомного количества энергии Ци.(Используется вихрь Мерсенна)
long long getRandomChi() {
    std::random_device rnd;
    std::mt19937 mersenne(rnd());
    std::uniform_int_distribution<long long> random_chi(1, 100);
    return random_chi(mersenne);
}

// Определения победителя пары.
Monk findStrongest(Monk &first_monk, Monk &second_monk) {
    cout << "Начинается битва монаха " + first_monk.name + " набравшего энергию " + to_string(first_monk.chi)
        + " Ци и монаха " +
        second_monk.name + " запасшегося " + to_string(second_monk.chi) + " Ци.\n";
    Monk winner;
    if (first_monk > second_monk) {
        winner = first_monk;
        winner.chi += second_monk.chi;
    } else if (first_monk < second_monk) {
        winner = second_monk;
        winner.chi += first_monk.chi;
    } else {
        if (getRandomChi() % 2) {
            winner = first_monk;
            winner.chi += second_monk.chi;
        } else {
            winner = second_monk;
            winner.chi += first_monk.chi;
        }
    }
    cout << "Монах " + winner.name + " победил и увеличил свою энергию до " + to_string(winner.chi) + " Ци!.\n\n";
    return winner;
}

// Составление турнирной сетки и разделение на потоки.
Monk splitParticipants(vector<Monk> monks) {
    if (monks.size() == 1) {
        monks[0].chi *= 2;
        cout << "Монах " + monks[0].name + " присел поел и увеличил свою энергию до "
             << to_string(monks[0].chi) + " Ци!.\n\n";
        return monks[0];
    } else if (monks.size() == 2) {
        return findStrongest(monks[0], monks[1]);
    } else {
        shuffle(begin(monks), end(monks), mt19937(std::random_device()()));

        unsigned long border_index = monks.size() / 2 + monks.size() % 2;

        // Левая сетка.
        vector<Monk> left_grid(monks.begin() + border_index, monks.end());
        // Правая сетка.
        vector<Monk> right_grid(monks.begin(), monks.begin() + border_index);

        // Асинхронный запуск для нахождения победителя левой сетки.
        future future_left_winner = async(launch::async, splitParticipants, left_grid);
        Monk left_winner = future_left_winner.get();

        // Асинхронный запуск для нахождения победителя правой сетки.
        future future_right_winner = async(launch::async, splitParticipants, right_grid);
        Monk right_winner = future_right_winner.get();

        return findStrongest(right_winner, left_winner);
    }
}

// Генерация случайного имени.
string getRandomName(int len) {
    string name;
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < len; ++i) {
        name += alphabet[getRandomChi() % (alphabet.size() - 1)];
    }
    return name;
}

// Генерация случайного имени и энергии Ци для каждого монаха.
vector<Monk> generateMonks(int number_of_monks) {
    vector<Monk> monks;
    for (int i = 0; i < number_of_monks; ++i) {
        monks.push_back({getRandomName(6), getRandomChi()});
    }
    return monks;
}

// Ввод с консоли количества монахов.
vector<Monk> getNumberOfMonks() {
    cout << "Введите количество монахов: \n";
    string line;
    int number_of_monks;
    while (true) {
        try {
            cin >> line;
            number_of_monks = stoi(line);
            if (number_of_monks < 0) {
                throw exception();
            }
            break;
        } catch (...) {
            std::cout << "Введите натуральное число: \n";
        }
    }
    return generateMonks(number_of_monks);
}

int main(int argc, char *argv[]) {
    int number_of_monks;
    vector<Monk> monks;
    if (argc == 1) {
        monks = getNumberOfMonks();
    } else if (argc == 2) {
        try {
            string arg = argv[1];
            number_of_monks = std::stoi(arg);
            monks = generateMonks(number_of_monks);
        } catch (...) {
            cout << "Неверный аргумент командной строки. Мы позвали 10 знакомых монахов подраться. \n";
            number_of_monks = 10;
            monks = generateMonks(10);
        }
    } else {
        cout << "Неверный аргумент командной строки. Мы позвали 10 знакомых монахов подраться. \n";
        number_of_monks = 10;
        monks = generateMonks(10);
    }

    if (monks.empty()) {
        cout << "Все монахи сейчас спят, драться некому. \n";
        return 0;
    }
    cout << "В турнире участвуют " + to_string(number_of_monks) + " монахов:\n";
    for (const Monk &monk: monks) {
        cout << "Монах " + monk.name + " с энергией Ци: " + to_string(monk.chi) + '\n';
    }
    cout << '\n';
    Monk best_monk = splitParticipants(monks);

    cout << "\nПобедителем и королем Гималаев становится " << best_monk.name << " ,который набрал " << best_monk.chi
         << " энергии Ци.\n";
}