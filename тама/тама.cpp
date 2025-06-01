#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <algorithm>
#include <vector>
#include <limits>
#include <windows.h>

using namespace std;

class Tamagotchi {
private:
    string name;
    string type;
    int hunger;
    int happiness;
    int energy;
    int health;
    int age;
    int level;
    int exp;
    bool isAlive;
    time_t lastUpdateTime;
    time_t lastRandomEventTime;
    string mood;

    void updateMood() {
        if (!isAlive) {
            mood = "мёртв";
            return;
        }
        if (health < 30) {
            mood = "болен";
        }
        else if (happiness > 70 && hunger < 30 && energy > 50) {
            mood = "счастлив";
        }
        else if (happiness < 30 || hunger > 70 || energy < 20) {
            mood = "грустный";
        }
        else {
            mood = "нормальный";
        }
    }

    void triggerRandomEvent() {
        time_t currentTime = time(0);
        if (difftime(currentTime, lastRandomEventTime) < 60) return;

        int eventChance = rand() % 100;
        if (eventChance < 15) {
            int eventType = rand() % 6;
            switch (eventType) {
            case 0:
                health -= 15;
                cout << "\n[Событие] " << name << " заболел!\n";
                break;
            case 1:
                hunger = max(0, hunger - 10);
                cout << "\n[Событие] " << name << " нашёл еду!\n";
                break;
            case 2:
                happiness = max(0, happiness - 10);
                cout << "\n[Событие] " << name << " увидел страшный сон!\n";
                break;
            case 3:
                if (happiness < 30) {
                    happiness = 0;
                    cout << "\n[Событие] " << name << " сбежал из-за плохого обращения!\n";
                }
                break;
            case 4:
                happiness = max(0, happiness - 5);
                cout << "\n[Событие] Идёт дождь. " << name << " грустит.\n";
                break;
            case 5:
                exp += 5;
                cout << "\n[Событие] " << name << " чему-то научился! (+5 опыта)\n";
                break;
            }
            lastRandomEventTime = currentTime;
        }
    }

    void checkLevelUp() {
        while (exp >= level * 20) {
            exp -= level * 20;
            level++;
            health = 100;
            energy = 100;
            cout << "\n>>> " << name << " достиг уровня " << level << "! <<<\n";
            cout << "Здоровье и энергия полностью восстановлены!\n";
        }
    }

    void playObstacleRace() {
        cout << "\n=== " << name << " бежит! ===\n";
        cout << "Нажимай [ПРОБЕЛ], когда увидишь препятствие '▲'!\n";
        cout << "Готовься...\n";
        Sleep(2000);

        int score = 0;
        for (int i = 0; i < 5; ++i) {
            if (rand() % 2) {
                cout << "▲ - Прыгай!\n";
                clock_t start = clock();
                while (true) {
                    if (_kbhit()) {
                        if (_getch() == ' ') {
                            double reactionTime = (double)(clock() - start) / CLOCKS_PER_SEC * 1000;
                            if (reactionTime < 500) {
                                score += 2;
                                cout << "Идеально! (+2 очка)\n";
                            }
                            else {
                                score += 1;
                                cout << "Успел! (+1 очко)\n";
                            }
                        }
                        break;
                    }
                    if ((double)(clock() - start) / CLOCKS_PER_SEC > 1.0) {
                        cout << "Пропустил...\n";
                        break;
                    }
                }
            }
            else {
                cout << "  - Бежим дальше\n";
            }
            Sleep(800);
        }

        happiness = min(100, happiness + 10 + score * 3);
        energy = max(0, energy - 15);
        exp += score;

        cout << "\nИтог: " << score << " очков. ";
        if (score >= 6) {
            cout << name << " в восторге!\n";
        }
        else if (score >= 3) {
            cout << name << " доволен.\n";
        }
        else {
            cout << name << " хотел бы лучше...\n";
        }
    }

    void playGuessNumber() {
        cout << "\n=== " << name << " загадал число! ===\n";
        cout << "Попробуй угадать число от 1 до 10, которое загадал " << name << "!\n";

        int secretNumber = rand() % 10 + 1;
        int attempts = 3;
        int guess;

        while (attempts > 0) {
            cout << "У тебя " << attempts << " попыток. Твой вариант: ";
            cin >> guess;

            if (guess == secretNumber) {
                cout << "Правильно! " << name << " рад, что ты угадал!\n";
                happiness = min(100, happiness + 15);
                exp += 5;
                return;
            }
            else if (guess < secretNumber) {
                cout << "Слишком мало! " << name << " смотрит на тебя с надеждой.\n";
            }
            else {
                cout << "Слишком много! " << name << " виляет хвостом.\n";
            }
            attempts--;
        }

        cout << "Ты не угадал! " << name << " загадал число " << secretNumber << ".\n";
        happiness = max(0, happiness - 5);
        exp += 1;
    }

public:
    Tamagotchi(string petName, string petType) :
        name(petName),
        type(petType),
        hunger(50),
        happiness(50),
        energy(50),
        health(100),
        age(0),
        level(1),
        exp(0),
        isAlive(true),
        mood("нормальный"),
        lastUpdateTime(time(0)),
        lastRandomEventTime(time(0)) {
    }

    void updateStats() {
        time_t currentTime = time(0);
        double secondsPassed = difftime(currentTime, lastUpdateTime);

        if (secondsPassed >= 5) {
            hunger = min(100, hunger + 5);
            happiness = max(0, happiness - 3);
            energy = max(0, energy - 4);
            age += 1;
            lastUpdateTime = currentTime;
            updateMood();
            triggerRandomEvent();
            checkLevelUp();
        }

        if (hunger >= 100 || health <= 0) {
            isAlive = false;
            mood = "мёртв";
        }
    }

    void feed() {
        if (!isAlive) return;
        hunger = max(0, hunger - 20);
        health = min(100, health + 5);
        exp += 2;
        updateMood();
    }

    void play() {
        if (!isAlive) return;

        cout << "\nВыбери игру:\n";
        cout << "1. Бег с препятствиями\n";
        cout << "2. Угадай число\n";
        cout << "Твой выбор (1-2): ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            playObstacleRace();
        }
        else if (choice == 2) {
            playGuessNumber();
        }
        else {
            cout << "Неверный выбор! " << name << " скучает...\n";
        }
    }

    void sleep() {
        if (!isAlive) return;
        energy = min(100, energy + 30);
        health = min(100, health + 10);
        exp += 1;
        updateMood();
    }

    void showPet() {
        if (type == "dog") {
            cout << "    / \\__\n";
            if (mood == "счастлив") {
                cout << "   (    @\\___\n";
                cout << "   /         O\n";
            }
            else if (mood == "грустный" || mood == "болен") {
                cout << "   (    -\\___\n";
                cout << "   /         o\n";
            }
            else if (mood == "мёртв") {
                cout << "   (    x\\___\n";
                cout << "   /         X\n";
            }
            else {
                cout << "   (    o\\___\n";
                cout << "   /         O\n";
            }
            cout << "  /   (_____/\n";
            cout << " /_____/ U\n";
        }
        else {
            cout << "       /\\_/\\\n";
            if (mood == "счастлив") {
                cout << "      ( ^.^ )\n";
                cout << "       > ^ <\n";
            }
            else if (mood == "грустный") {
                cout << "      ( >.< )\n";
                cout << "       -   -\n";
            }
            else if (mood == "болен") {
                cout << "      ( +.+ )\n";
                cout << "       -   -\n";
            }
            else if (mood == "мёртв") {
                cout << "      ( X X )\n";
                cout << "       ___ \n";
            }
            else {
                cout << "      ( -.- )\n";
                cout << "       o   o\n";
            }
            cout << "      \"     \"\n";
        }
    }

    void showStatus() {
        cout << "\n--- " << name << " (" << (type == "dog" ? "Пёсик" : "Котик") << ") ---\n";
        cout << "Уровень: " << level << " (" << exp << "/" << level * 20 << " опыта)\n";
        cout << "Возраст: " << age << " дней\n";
        cout << "Состояние: " << mood << "\n";
        cout << "Голод:     [";
        for (int i = 0; i < 10; i++) cout << (hunger > i * 10 ? "#" : "-");
        cout << "] " << hunger << "%\n";
        cout << "Радость:   [";
        for (int i = 0; i < 10; i++) cout << (happiness > i * 10 ? "#" : "-");
        cout << "] " << happiness << "%\n";
        cout << "Энергия:   [";
        for (int i = 0; i < 10; i++) cout << (energy > i * 10 ? "#" : "-");
        cout << "] " << energy << "%\n";
        cout << "Здоровье:  [";
        for (int i = 0; i < 10; i++) cout << (health > i * 10 ? "#" : "-");
        cout << "] " << health << "%\n";

        if (!isAlive) {
            cout << "\n" << name << " умер... :'(\n";
        }
    }

    bool alive() const {
        return isAlive;
    }

    int getAge() const {
        return age;
    }
};

void clearScreen() {
    system("cls");
}

void showMainMenu() {
    cout << "\nВыбери действие:\n";
    cout << "1. Покормить (+2 опыта)\n";
    cout << "2. Поиграть\n";
    cout << "3. Уложить спать (+1 опыт)\n";
    cout << "4. Ничего не делать\n";
    cout << "5. Выход\n";
}

void showWelcome() {
    cout << "  ___________________________________  \n";
    cout << " /                                   \\ \n";
    cout << "|   Д О Б Р О   П О Ж А Л О В А Т Ь   |\n";
    cout << "|          В  Т А М А Г О Ч И         |\n";
    cout << " \\___________________________________/ \n\n";
}

int main() {
    srand(time(0));
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    clearScreen();
    showWelcome();

    cout << "Выбери питомца:\n";
    cout << "1. Собака\n";
    cout << "2. Кошка\n";
    cout << "Выбери (1-2): ";

    int petType;
    cin >> petType;
    cin.ignore();

    if (petType != 1 && petType != 2) {
        cout << "Неверный выбор! По умолчанию выбран пёсик.\n";
        petType = 1;
    }

    string typeName = (petType == 1) ? "dog" : "cat";

    cout << "\nКак назовёшь своего питомца? ";
    string petName;
    getline(cin, petName);

    Tamagotchi pet(petName, typeName);

    while (pet.alive()) {
        clearScreen();
        showWelcome();

        pet.updateStats();
        pet.showPet();
        pet.showStatus();

        showMainMenu();
        cout << "Выбери действие (1-5): ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            pet.feed();
            cout << "\n" << petName << " поел! Ням-ням! (+2 опыта)\n";
            break;
        case 2:
            pet.play();
            break;
        case 3:
            pet.sleep();
            cout << "\n" << petName << " сладко спит... Хр-р-р... (+1 опыт)\n";
            break;
        case 4:
            cout << "\nТы ничего не делаешь...\n";
            break;
        case 5:
            cout << "\nДо свидания!\n";
            return 0;
        default:
            cout << "\nНеверный выбор! Попробуй снова.\n";
        }

        cout << "\nНажми любую клавишу чтобы продолжить...";
        _getch();
    }

    clearScreen();
    showWelcome();
    pet.showPet();
    pet.showStatus();

    cout << "\nИгра окончена. " << petName << " умер...\n";
    cout << "Он достиг уровня " << pet.getAge() << " и прожил " << pet.getAge() << " дней.\n";

    return 0;
}