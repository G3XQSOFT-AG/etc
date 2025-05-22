#include <iostream>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>
#else
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#endif

// Кросс-платформенная поддержка UTF-8
class ConsoleConfig {
public:
    ConsoleConfig() {
        #ifdef _WIN32
        // Устанавливаем UTF-8 для консоли
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);
        
        // Включаем поддержку ANSI escape sequences
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
        
        // Устанавливаем режим вывода Unicode
        _setmode(_fileno(stdout), _O_U8TEXT);
        
        // Пытаемся установить шрифт с поддержкой кириллицы
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 0;
        cfi.dwFontSize.Y = 16;
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcscpy_s(cfi.FaceName, L"Lucida Console");
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
        #else
        // Устанавливаем локаль для Unix-систем
        std::setlocale(LC_ALL, "en_US.UTF-8");
        std::locale::global(std::locale("en_US.UTF-8"));
        #endif
    }
};

// Универсальный класс для работы с цветом
class ColorManager {
public:
    static void SetColor(int color) {
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        #else
        std::cout << "\033[1;" << color << "m";
        #endif
    }

    static void Reset() {
        #ifdef _WIN32
        SetColor(7);
        #else
        std::cout << "\033[0m";
        #endif
    }

    static void ClearScreen() {
        std::cout << "\033[2J\033[H";
    }
};

// Кросс-платформенный ввод символов
char GetKeyPress() {
    #ifdef _WIN32
    return _getch();
    #else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
    #endif
}

// Анимированный вывод текста с поддержкой UTF-8
void PrintWithEffect(const std::string& text, int delay = 50) {
    for (size_t i = 0; i < text.length();) {
        unsigned char c = text[i];
        size_t charSize = 1;
        
        if ((c & 0xE0) == 0xC0) charSize = 2;
        else if ((c & 0xF0) == 0xE0) charSize = 3;
        else if ((c & 0xF8) == 0xF0) charSize = 4;
        
        for (size_t j = 0; j < charSize && i < text.length(); ++j, ++i) {
            std::cout << text[i];
        }
        
        std::cout << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

// Класс для рисования сердца
class HeartAnimator {
public:
    static void DrawHeart(int color) {
        ColorManager::SetColor(color);
        const int size = 6;
        const char symbol = '*';
        
        for (int x = 0; x < size; x++) {
            for (int y = 0; y <= 4*size; y++) {
                double dist1 = sqrt(pow(x-size, 2) + pow(y-size, 2));
                double dist2 = sqrt(pow(x-size, 2) + pow(y-3*size, 2));
                std::cout << ((dist1 < size + 0.5 || dist2 < size + 0.5) ? symbol : ' ');
            }
            std::cout << '\n';
        }
        
        for (int x = 1; x < 2*size; x++) {
            std::cout << std::string(x, ' ');
            std::cout << std::string(4*size + 1 - 2*x, symbol) << '\n';
        }
        ColorManager::Reset();
    }
};

// Основной класс приложения
class InteractiveDialog {
    std::string name;
    
    void ShowMainMenu() {
        while(true) {
            ColorManager::SetColor(14);
            PrintWithEffect("\nВыбери действие:\n");
            ColorManager::SetColor(10);
            PrintWithEffect("1. Принять кофе-инвайт\n");
            PrintWithEffect("2. Запустить тестовый режим\n");
            PrintWithEffect("3. Экстренное объятие\n> ");

            char choice = GetKeyPress();
            ColorManager::ClearScreen();
            
            switch(choice) {
                case '1': 
                    ColorManager::SetColor(12);
                    PrintWithEffect("\nИнициализирую маршрут до лучшей кофейни...\n");
                    return;
                case '2':
                    ColorManager::SetColor(13);
                    PrintWithEffect("\nЗапускаю тестовый режим романтики...\n");
                    HeartAnimator::DrawHeart(13);
                    break;
                case '3':
                    ColorManager::SetColor(11);
                    PrintWithEffect("\nАктивирую протокол объятий!\n");
                    for(int i=0; i<5; ++i) {
                        HeartAnimator::DrawHeart(i%2 ? 12 : 13);
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        ColorManager::ClearScreen();
                    }
                    return;
                default:
                    ColorManager::SetColor(12);
                    PrintWithEffect("\nОшибка ввода! Повторите выбор...\n");
            }
        }
    }

public:
    InteractiveDialog(const std::string& name) : name(name) {}

    void Start() {
        ConsoleConfig _;
        ColorManager::ClearScreen();
        ColorManager::SetColor(11);
        PrintWithEffect("\n/* " + std::string(50, '=') + " */");
        
        ColorManager::SetColor(14);
        PrintWithEffect("\n// Инициализация системы распознавания совершенства...\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        for(int i = 0; i < 3; ++i) {
            ColorManager::ClearScreen();
            HeartAnimator::DrawHeart(i%2 ? 12 : 13);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }

        ColorManager::SetColor(10);
        PrintWithEffect("\n// Обнаружен объект: ");
        ColorManager::SetColor(13);
        PrintWithEffect(name + "\n");
        
        ColorManager::SetColor(11);
        PrintWithEffect("// Анализ характеристик...\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        ColorManager::SetColor(14);
        PrintWithEffect("Привет, " + name + "! Ты как идеально оптимизированный код:\n");
        
        ColorManager::SetColor(13);
        const std::string compliments[] = {
            "• Безупречная логика\n", 
            "• Элегантная структура\n",
            "• Исключительная эффективность\n"
        };
        
        for(const auto& c : compliments) {
            PrintWithEffect(c);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        ColorManager::SetColor(11);
        PrintWithEffect("\n// Инициализация протокола знакомства...\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        ShowMainMenu();
    }
};

int main() {
    #ifdef _WIN32
    setlocale(LC_ALL, "Russian");
    #else
    setlocale(LC_ALL, "");
    #endif
    
    ConsoleConfig config;
    
    ColorManager::SetColor(11);
    PrintWithEffect("Введи свое имя: ");
    ColorManager::SetColor(10);
    
    std::string name;
    std::getline(std::cin, name);
    
    InteractiveDialog dialog(name);
    dialog.Start();
    
    ColorManager::Reset();
    return 0;
}
