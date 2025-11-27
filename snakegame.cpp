#include <iostream>
#include <windows.h>
#include <queue>
#include <random>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

const char APPLE = '@';
const char SNAKE = '*';

enum DELAYS {
    OPENING_DELAY = 500,
    INPUT_DELAY = 200,
    FRAMERATE_DELAY = 100,
    CLOSING_DELAY = 1500,
    RESUME_DELAY = 700
};

enum CONSOLE {
    CONSOLE_WIDTH = 60,
    CONSOLE_HEIGHT = 32,
};

enum class CHOICE {
    gameRestart,
    gamestart,
    instruction,
    history,
    exit,
    gameEnd,
    gameResume,
    backToMainMenu
};

const int center_width = CONSOLE_WIDTH / 2;
const int center_height = CONSOLE_HEIGHT / 2;

const int mainMenu_gap = 5;
const int mainMenu_title = 4;
const int mainMenu_highestscore = mainMenu_title + 2;
const int mainMenu_start = mainMenu_highestscore + mainMenu_gap;
const int mainMenu_instruction = mainMenu_start + mainMenu_gap;
const int mainMenu_history = mainMenu_instruction + mainMenu_gap;
const int mainMenu_exit = mainMenu_history + mainMenu_gap;

void gotoCOORD(short x, short y) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(consoleHandle, coord);
}

int printString_center(const string& str, int coord_y) {
    int coord_x = (CONSOLE_WIDTH - str.size()) / 2;
    gotoCOORD(coord_x, coord_y);
    cout << str;
    return coord_x;
}

pair<int, int> printString_center(const string& str1, const string str2, int spaces_n, int coord_y) {
    string spaces(spaces_n, ' ');

    int coord_x1 = (CONSOLE_WIDTH - (str1.size() + str2.size() + spaces_n)) / 2;
    int coord_x2 = coord_x1 + str1.size() + spaces_n;

    gotoCOORD(coord_x1, coord_y);
    cout << str1 << spaces << str2;

    return { coord_x1,coord_x2 };
}

void printString_right(const string& str, int coord_y) {
    int coord_x = (CONSOLE_WIDTH - str.size());
    gotoCOORD(coord_x, coord_y);
    cout << str;
}

void setConsoleTextColor(WORD color) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(consoleHandle, color);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void saveConsoleScreen(CHAR_INFO buffer[], COORD& bufferSize, COORD& cursorCOORD) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
    bufferSize = csbi.dwSize;
    cursorCOORD = csbi.dwCursorPosition;
    ReadConsoleOutput(consoleHandle, buffer, bufferSize, { 0,0 }, &csbi.srWindow);
}

void restoreConsoleScreen(CHAR_INFO buffer[], COORD& bufferSize, COORD& cursorCOORD) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
    WriteConsoleOutput(consoleHandle, buffer, bufferSize, { 0,0 }, &csbi.srWindow);
    SetConsoleCursorPosition(consoleHandle, cursorCOORD);
}

void initConsole(int width, int height, string title) {
    string command_consoleSize = "mode con cols=" + to_string(width) + " lines=" + to_string(height);
    string command_title = "title " + title;
    system(command_consoleSize.c_str());
    system(command_title.c_str());
    hideCursor();
}

void displayOpening() {
    printString_center("Qja Games Presents", 12);

    for (int i = -3; i <= 3; i += 2) {
        gotoCOORD(center_width + i, 15);
        cout << ".";
        Sleep(OPENING_DELAY);
    }
}

void displayDefaultMainMenu(int highestScore) {
    printString_center("[ Snake Game ]", mainMenu_title);
    printString_center("Highest Score : " + to_string(highestScore), mainMenu_highestscore);

    printString_center("Start", mainMenu_start);

    printString_center("Instruction", mainMenu_instruction);

    printString_center("History", mainMenu_history);

    printString_center("Exit", mainMenu_exit);

    printString_right("madeby_Qja", CONSOLE_HEIGHT - 1);
}

CHOICE activateMainMenu() {
    CHOICE choice = CHOICE::gamestart;
    bool enter = false;
    bool vanish = false;

    do {
        Sleep(INPUT_DELAY);

        if (GetAsyncKeyState(VK_UP) && choice > CHOICE::gamestart) {
            choice = static_cast<CHOICE>(static_cast<int>(choice) - 1);
            vanish = true;
        }
        else if (GetAsyncKeyState(VK_DOWN) && choice < CHOICE::exit) {
            choice = static_cast<CHOICE>(static_cast<int>(choice) + 1);
            vanish = true;
        }
        else if (GetAsyncKeyState(VK_RETURN)) {
            vanish = true;
            enter = true;
            continue;
        }

        vanish ^= true;
        if (!vanish) {
            switch (choice) {
            case CHOICE::gamestart:
                printString_center("> Start <", mainMenu_start);
                printString_center("  Instruction  ", mainMenu_instruction);
                printString_center("  History  ", mainMenu_history);
                printString_center("  Exit  ", mainMenu_exit);
                break;
            case CHOICE::instruction:
                printString_center("  Start  ", mainMenu_start);
                printString_center("> Instruction <", mainMenu_instruction);
                printString_center("  History  ", mainMenu_history);
                printString_center("  Exit  ", mainMenu_exit);
                break;
            case CHOICE::history:
                printString_center("  Start  ", mainMenu_start);
                printString_center("  Instruction  ", mainMenu_instruction);
                printString_center("> History <", mainMenu_history);
                printString_center("  Exit  ", mainMenu_exit);
                break;
            case CHOICE::exit:
                printString_center("  Start  ", mainMenu_start);
                printString_center("  Instruction  ", mainMenu_instruction);
                printString_center("  History  ", mainMenu_history);
                printString_center("> Exit <", mainMenu_exit);
                break;
            default:
                break;
            }
        }
        else {
            switch (choice) {
            case CHOICE::gamestart:
                printString_center("  Start  ", mainMenu_start);
                break;
            case CHOICE::instruction:
                printString_center("  Instruction  ", mainMenu_instruction);
                break;
            case CHOICE::history:
                printString_center("  History  ", mainMenu_history);
                break;
            case CHOICE::exit:
                printString_center("  Exit  ", mainMenu_exit);
                break;
            default:
                break;
            }
        }

    } while (!enter);

    return choice;
}

CHOICE pauseMenu(CHAR_INFO buffer[], COORD& bufferSize, COORD& cursorPos) {
    saveConsoleScreen(buffer, bufferSize, cursorPos);

    short coord_y = center_height - 1;

    bool ESC = false;
    bool ENTER = false;
    while (true) {
        do {
            printString_center("Press ENTER to resume, Press ESC to exit", coord_y);

            Sleep(INPUT_DELAY);
            if (GetAsyncKeyState(VK_RETURN)) {
                ENTER = true;
            }
            else if (GetAsyncKeyState(VK_ESCAPE)) {
                ESC = true;
            }
        } while (!ENTER && !ESC);

        if (ENTER) {
            for (int i = 3; i >= 1; i--) {
                restoreConsoleScreen(buffer, bufferSize, cursorPos);
                printString_center("[ " + to_string(i) + " ]", coord_y);
                Sleep(RESUME_DELAY);
            }
            restoreConsoleScreen(buffer, bufferSize, cursorPos);
            return CHOICE::gameResume;
        }

        else {
            system("cls");
            printString_center("Really want to quit?", center_height - 3);

            pair<int, int> coord_x = printString_center("NO", "YES", 8, center_height - 1);

            bool vanish = false;
            bool quitConfirmed = false;
            while (true) {
                Sleep(INPUT_DELAY);
                if (GetAsyncKeyState(VK_RIGHT) && !quitConfirmed) {
                    quitConfirmed = true;
                    vanish = false;
                }
                else if (GetAsyncKeyState(VK_LEFT) && quitConfirmed) {
                    quitConfirmed = false;
                    vanish = false;
                }
                else if (GetAsyncKeyState(VK_RETURN)) {
                    break;
                }

                if (quitConfirmed) {
                    if (!vanish) {
                        gotoCOORD(coord_x.second - 2, center_height - 1);
                        cout << '>';
                        gotoCOORD(coord_x.second + 4, center_height - 1);
                        cout << '<';
                        gotoCOORD(coord_x.first - 2, center_height - 1);
                        cout << ' ';
                        gotoCOORD(coord_x.first + 3, center_height - 1);
                        cout << ' ';
                    }
                    else {
                        gotoCOORD(coord_x.second - 2, center_height - 1);
                        cout << ' ';
                        gotoCOORD(coord_x.second + 4, center_height - 1);
                        cout << ' ';
                    }
                }
                else {
                    if (!vanish) {
                        gotoCOORD(coord_x.first - 2, center_height - 1);
                        cout << '>';
                        gotoCOORD(coord_x.first + 3, center_height - 1);
                        cout << '<';
                        gotoCOORD(coord_x.second - 2, center_height - 1);
                        cout << ' ';
                        gotoCOORD(coord_x.second + 4, center_height - 1);
                        cout << ' ';
                    }
                    else {
                        gotoCOORD(coord_x.first - 2, center_height - 1);
                        cout << ' ';
                        gotoCOORD(coord_x.first + 3, center_height - 1);
                        cout << ' ';
                    }
                }
                vanish ^= true;
            }

            if (quitConfirmed) {
                return CHOICE::backToMainMenu;
            }
            else {
                ESC = false;
                restoreConsoleScreen(buffer, bufferSize, cursorPos);
            }
        }
    }
}

void printApple(COORD apple) {
    gotoCOORD(apple.X, apple.Y);
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << APPLE;
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void printPoisonedApple(COORD poisonedApple) {
    gotoCOORD(poisonedApple.X, poisonedApple.Y);
    setConsoleTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << APPLE;
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

CHOICE endGame(int score) {
    system("cls");
    int coord_y = center_height - 6;
    printString_center("Game End!", coord_y); coord_y += 3;
    printString_center("Your Score : " + to_string(score), coord_y); coord_y += 5;

    bool vanish = false;
    while (true) {
        if (!vanish)
            printString_center("Press ENTER to restart, Press ESC to exit", coord_y);
        else {
            string str(42, ' ');
            printString_center(str, coord_y);
        }

        Sleep(INPUT_DELAY);
        if (GetAsyncKeyState(VK_RETURN)) {
            return CHOICE::gameRestart;
        }
        else if (GetAsyncKeyState(VK_ESCAPE)) {
            return CHOICE::gameEnd;
        }
        vanish ^= true;
    }
}

void updateScore(int score, COORD scordboardCOORD) {
    gotoCOORD(scordboardCOORD.X, scordboardCOORD.Y);
    cout << "Score : [      ";
    gotoCOORD(scordboardCOORD.X + 10, scordboardCOORD.Y);
    cout << score << " ]";
}

COORD createNewApple(vector<COORD>& apple, vector<COORD>& poisonedApple, COORD scordboardCOORD, queue<COORD>& snake) {
    random_device random;
    mt19937 gen(random());
    uniform_int_distribution<short> distWIDTH(0, CONSOLE_WIDTH - 1);
    uniform_int_distribution<short> distHEIGHT(0, CONSOLE_HEIGHT - 1);
    COORD newApple;

    bool overlap;
    do {
        overlap = false;
        newApple = { distWIDTH(gen), distHEIGHT(gen) };

        for (int i = 0; i < apple.size(); i++) {
            if (newApple.X == apple[i].X && newApple.Y == apple[i].Y) {
                overlap = true;
                break;
            }
        }

        for (int i = 0; i < poisonedApple.size(); i++) {
            if (newApple.X == poisonedApple[i].X && newApple.Y == poisonedApple[i].Y) {
                overlap = true;
            }
        }

        if (newApple.Y == 0) {
            overlap = true;
        }

        queue<COORD> tmpSnake = snake;
        COORD snakePart;
        while (!tmpSnake.empty()) {
            snakePart = tmpSnake.front();
            tmpSnake.pop();
            if (snakePart.X == newApple.X && snakePart.Y == newApple.Y) {
                overlap = true;
                break;
            }
        }
    } while (overlap);

    return newApple;
}

bool biteOwnTail(COORD newHead, queue<COORD> snake) {
    queue<COORD> tmpSnake = snake;
    COORD snakePart;
    while (!tmpSnake.empty()) {
        snakePart = tmpSnake.front();
        tmpSnake.pop();
        if (snakePart.X == newHead.X && snakePart.Y == newHead.Y) {
            return true;
        }
    }
    return false;
}

bool eatPoisonedApple(COORD newHead, vector<COORD> poisonedApple) {
    for (COORD& tmpApple : poisonedApple) {
        if (newHead.X == tmpApple.X && newHead.Y == tmpApple.Y) {
            return true;
        }
    }
    return false;
}

CHOICE startGame(int& score) {
    system("cls");

    CHAR_INFO buffer[CONSOLE_WIDTH * CONSOLE_HEIGHT];
    COORD bufferSize = { CONSOLE_WIDTH, CONSOLE_HEIGHT };
    COORD cursorCOORD;

    queue<COORD> snake;
    CHOICE choice;
    short dx = 1, dy = 0;

    COORD scordboardCOORD = { CONSOLE_WIDTH - 15, 0 };
    updateScore(score, scordboardCOORD);

    COORD currentHead = { center_width, CONSOLE_HEIGHT - 1 };
    snake.push(currentHead);
    gotoCOORD(currentHead.X, currentHead.Y);
    cout << SNAKE;

    vector<COORD> apple(5, { -1,-1 });
    vector<COORD> poisonedApple(1, { -1,-1 });
    for (int i = 0; i < apple.size(); i++) {
        apple[i] = createNewApple(apple, poisonedApple, scordboardCOORD, snake);
        printApple(apple[i]);
    }
    COORD newPoisonedApple = createNewApple(apple, poisonedApple, scordboardCOORD, snake);
    poisonedApple.push_back(newPoisonedApple);
    printPoisonedApple(newPoisonedApple);

    while (true) {
        Sleep(FRAMERATE_DELAY);

        if (GetAsyncKeyState(VK_ESCAPE)) {
            choice = pauseMenu(buffer, bufferSize, cursorCOORD);
            if (choice == CHOICE::backToMainMenu) {
                return choice;
            }
        }
        else if (GetAsyncKeyState(VK_LEFT)) {
            dx = -1, dy = 0;
        }
        else if (GetAsyncKeyState(VK_UP)) {
            dx = 0, dy = -1;
        }
        else if (GetAsyncKeyState(VK_RIGHT)) {
            dx = 1, dy = 0;
        }
        else if (GetAsyncKeyState(VK_DOWN)) {
            dx = 0, dy = 1;
        }

        COORD newHead = { static_cast<short>(currentHead.X + dx), static_cast<short>(currentHead.Y + dy) };

        bool bumpWall = newHead.X < 0 || newHead.X >= CONSOLE_WIDTH || newHead.Y < 0 || newHead.Y >= CONSOLE_HEIGHT;
        if (bumpWall || biteOwnTail(newHead, snake) || eatPoisonedApple(newHead, poisonedApple)) {
            return endGame(score);
        }

        snake.push(newHead);
        gotoCOORD(newHead.X, newHead.Y);
        cout << SNAKE;

        bool eat = false;
        for (int i = 0; i < apple.size(); i++) {
            if (apple[i].X == newHead.X && apple[i].Y == newHead.Y) {
                updateScore(++score, scordboardCOORD);
                eat = true;

                COORD newApple = createNewApple(apple, poisonedApple, scordboardCOORD, snake);
                printApple(newApple);
                apple[i] = newApple;

                newPoisonedApple = createNewApple(apple, poisonedApple, scordboardCOORD, snake);
                poisonedApple.push_back(newPoisonedApple);
                printPoisonedApple(newPoisonedApple);

                break;
            }
        }

        if (!eat) {
            COORD tail = snake.front();
            gotoCOORD(tail.X, tail.Y);
            cout << ' ';
            snake.pop();
        }

        currentHead = newHead;
    }

    return choice;
}

void displayInstruction() {
    system("cls");

    int coord_y = 4;
    printString_center("[ Instructions ]", coord_y); coord_y += 4;

    printString_center("<How to play>", coord_y);
    printString_center("Eat as many apples as you can.", coord_y); coord_y += 2;
    printString_center("Each time an apple is eaten,", coord_y);
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "(Red)"; coord_y++;
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printString_center("the snake will grow.", coord_y); coord_y += 3;

    printString_center("<Rules>", coord_y); coord_y++;
    printString_center("1. Don't hit a wall.", coord_y); coord_y++;
    printString_center("2. Don't bite your own tail.", coord_y); coord_y++;
    printString_center("3. Don't eat posioned apples.", coord_y);
    setConsoleTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "(Green)"; coord_y += 3;
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    printString_center("<Contorls>", coord_y); coord_y++;
    printString_center("Use arrow keys to change direction.", coord_y); coord_y++;
    printString_center("Press ESC to pause.", coord_y); coord_y += 4;

    Sleep(INPUT_DELAY);
    bool vanish = false;

    do {
        Sleep(INPUT_DELAY);
        if (!vanish)
            printString_center("press ESC to go back...", coord_y);
        else
            printString_center("                         ", coord_y);
        vanish ^= true;
    } while (!GetAsyncKeyState(VK_ESCAPE));
}

vector<pair<int, string>> loadHistory() {
    vector<pair<int, string>> history;
    ifstream file("history.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            size_t dashPos = line.find('-');
            size_t firstSlashPos = line.find('/');

            int score = stoi(line.substr(0, dashPos));
            string date = line.substr(dashPos + 1);
            history.push_back({ score, date });
        }
        file.close();
    }
    return history;
}

int getHighestScore(const vector<pair<int, string>>& history) {
    if (history.empty()) {
        return 0;
    }
    else {
        const pair<int, string>& lastHistory = history.back();
        return lastHistory.first;
    }
}

void saveHighesetScore(int highestScore) {
    ofstream file("history.txt", ios::app);
    if (file.is_open()) {
        time_t now = time(0);
        tm localtm;
        localtime_s(&localtm, &now);
        string date = to_string(localtm.tm_year + 1900 - 2000) + "/0" + to_string(localtm.tm_mon + 1) + "/" + to_string(localtm.tm_mday);
        file << highestScore << '-' << date << endl;
        file.close();
    }
}

void displayHistory(const vector<pair<int, string>>& history) {
    system("cls");

    int depth = 4;
    printString_center("[ History ]", depth); depth += 2;

    if (history.empty()) {
        printString_center("You haven't played yet!", center_height - 1);
    }
    else {
        int digits;
        gotoCOORD(center_width - 12, depth);
        cout << "<Score>";
        gotoCOORD(center_width + 3, depth);
        cout << "<Date>";
        depth += 2;

        int n = min(static_cast<int>(history.size()), 10);
        for (int i = static_cast<int>(history.size()) - n; i < history.size(); i++) {
            digits = to_string(history[i].first).size();

            gotoCOORD(center_width - 7 - digits, depth);
            cout << history[i].first;

            gotoCOORD(center_width - 2, depth);
            cout << '-';

            gotoCOORD(center_width + 3, depth);
            cout << history[i].second;

            depth += 2;
        }
    }

    bool vanish = false;
    do {
        Sleep(INPUT_DELAY);
        if (!vanish)
            printString_center("Press ESC to go back...", CONSOLE_HEIGHT - 3);
        else
            printString_center("                         ", CONSOLE_HEIGHT - 3);
        vanish ^= true;
    } while (!GetAsyncKeyState(VK_ESCAPE));
}

bool exitGame() {
    system("cls");

    pair<int, int> coord_x;

    printString_center("Really want to exit the game?", center_height - 4);
    int coord_y = center_height - 1;
    coord_x = printString_center("NO", "YES", 8, coord_y);

    bool vanish = false;
    bool exitConfirmed = false;
    while (true) {
        Sleep(INPUT_DELAY);
        if (GetAsyncKeyState(VK_RIGHT) && !exitConfirmed) {
            exitConfirmed = true;
            vanish = false;
        }
        else if (GetAsyncKeyState(VK_LEFT) && exitConfirmed) {
            exitConfirmed = false;
            vanish = false;
        }
        else if (GetAsyncKeyState(VK_RETURN)) {
            break;
        }

        if (exitConfirmed) {
            if (!vanish) {
                gotoCOORD(coord_x.second - 2, coord_y);
                cout << '>';
                gotoCOORD(coord_x.second + 4, coord_y);
                cout << '<';
                gotoCOORD(coord_x.first - 2, coord_y);
                cout << ' ';
                gotoCOORD(coord_x.first + 3, coord_y);
                cout << ' ';
            }
            else {
                gotoCOORD(coord_x.second - 2, coord_y);
                cout << ' ';
                gotoCOORD(coord_x.second + 4, coord_y);
                cout << ' ';
            }
        }
        else {
            if (!vanish) {
                gotoCOORD(coord_x.first - 2, coord_y);
                cout << '>';
                gotoCOORD(coord_x.first + 3, coord_y);
                cout << '<';
                gotoCOORD(coord_x.second - 2, coord_y);
                cout << ' ';
                gotoCOORD(coord_x.second + 4, coord_y);
                cout << ' ';
            }
            else {
                gotoCOORD(coord_x.first - 2, coord_y);
                cout << ' ';
                gotoCOORD(coord_x.first + 3, coord_y);
                cout << ' ';
            }
        }
        vanish ^= true;
    }

    return exitConfirmed;
}

void displayClosing() {
    system("cls");
    printString_center("Thanks for playing ^0^_Qja", center_height - 1);
    Sleep(CLOSING_DELAY);
}

int main() {
    setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    initConsole(CONSOLE_WIDTH, CONSOLE_HEIGHT, "Snake Game");

    displayOpening();

    int highestScore = getHighestScore(loadHistory());
    int score;
    bool exitConfirmed = false;

    do {
        system("cls");

        displayDefaultMainMenu(highestScore);

        CHOICE choice = activateMainMenu();
        switch (choice) {
        case CHOICE::gamestart:
            do {
                score = 0;
                choice = startGame(score);
                if (score > highestScore) {
                    highestScore = score;
                    saveHighesetScore(highestScore);
                }
            } while (choice == CHOICE::gameRestart);
            break;
        case CHOICE::instruction:
            displayInstruction();
            break;
        case CHOICE::history:
            displayHistory(loadHistory());
            break;
        case CHOICE::exit:
            exitConfirmed = exitGame();
            break;
        default:
            break;
        }
    } while (!exitConfirmed);

    displayClosing();

    return 0;
}