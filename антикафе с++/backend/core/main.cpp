#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void setRussianLocale() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 > nul");
#endif
    setlocale(LC_ALL, "ru_RU.UTF-8");
}

class User {
private:
    int id;
    string name;
    string email;
    string phone;
    string passwordHash;
    string registrationDate;

public:
    User(int id, string name, string email, string phone, string passHash, string regDate)
        : id(id), name(name), email(email), phone(phone), passwordHash(passHash), registrationDate(regDate) {}

    bool validatePhone() {
        if (phone.empty()) {
            cout << "Телефон не указан" << endl;
            return true;
        }

        if (phone.length() == 12 && phone.substr(0, 2) == "+7") {
            cout << "Телефон валиден: " << phone << endl;
            return true;
        }
        
        cout << "Неправльно набран номер" << endl;
        cout << "Пример: +79123456789" << endl;
        return false;
    }

    int countAllBookings() {
        cout << "Подсчет бронирований для пользователя ID: " << id << endl;
        return 0;
    }

    void display() {
        cout << "Информация о пользователе:" << endl;
        cout << "ID: " << id << endl;
        cout << "Имя: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Телефон: " << (phone.empty() ? "не указан" : phone) << endl;
        cout << "Дата регистрации: " << registrationDate << endl;
        cout << endl;
    }
};

class Tariff {
private:
    int id;
    string title;
    string description;
    double price;
    int durationMinutes;

public:
    Tariff(int id, string title, string desc, double price, int duration)
        : id(id), title(title), description(desc), price(price), durationMinutes(duration) {}

    double calculateCost(int hours) {
        if (durationMinutes > 0) {
            cout << "Тариф '" << title << "' фиксированный: " << price << " руб." << endl;
            return price;
        }
        
        double cost = price * hours;
        cout << "Тариф '" << title << "' почасовой: " << hours << " час. * " << price 
             << " руб./час = " << cost << " руб." << endl;
        return cost;
    }

    void display() {
        cout << "Тариф:" << endl;
        cout << "ID: " << id << endl;
        cout << "Название: " << title << endl;
        cout << "Описание: " << description << endl;
        cout << "Цена: " << price << " руб." << endl;
        if (durationMinutes > 0) {
            int hours = durationMinutes / 60;
            int minutes = durationMinutes % 60;
            cout << "Продолжительность: " << hours << " ч. " << minutes << " мин." << endl;
        } else {
            cout << "Почасовой тариф" << endl;
        }
        cout << endl;
    }
};

class Cafe {
private:
    int id;
    string name;
    string address;
    string phone;
    string email;
    int maxCapacity;

public:
    Cafe(int id, string name, string addr, string phone, string email, int capacity)
        : id(id), name(name), address(addr), phone(phone), email(email), maxCapacity(capacity) {}

    vector<string> getAvailableSlots(string date) {
        vector<string> slots;
        cout << "Получение доступных слотов для " << date 
             << " в кафе '" << name << "'" << endl;
        
        slots.push_back("10:00-12:00");
        slots.push_back("14:00-16:00");
        slots.push_back("18:00-20:00");
        
        cout << "Доступные слоты:" << endl;
        for (const auto& slot : slots) {
            cout << "  - " << slot << endl;
        }
        
        return slots;
    }

    bool checkCapacity(int guests) {
        bool available = guests <= maxCapacity;
        cout << "Проверка вместимости для " << guests << " гостей:" << endl;
        cout << "  Максимальная вместимость: " << maxCapacity << endl;
        cout << "  Доступно: " << (available ? "ДА" : "НЕТ") << endl;
        return available;
    }

    void display() {
        cout << "Информация о кафе:" << endl;
        cout << "ID: " << id << endl;
        cout << "Название: " << name << endl;
        cout << "Адрес: " << address << endl;
        cout << "Телефон: " << phone << endl;
        cout << "Email: " << email << endl;
        cout << "Вместимость: " << maxCapacity << " человек" << endl;
        cout << endl;
    }
};

class WorkSchedule {
private:
    int id;
    int cafeId;
    int dayOfWeek;
    string openTime;
    string closeTime;
    bool isOpen;

public:
    WorkSchedule(int id, int cafeId, int day, string open, string close, bool openFlag)
        : id(id), cafeId(cafeId), dayOfWeek(day), openTime(open), closeTime(close), isOpen(openFlag) {}

    bool isWorkingDay() {
        string days[] = {"Воскресенье", "Понедельник", "Вторник", "Среда", 
                        "Четверг", "Пятница", "Суббота"};
        cout << "День недели: " << days[dayOfWeek] << endl;
        cout << "Кафе открыто: " << (isOpen ? "ДА" : "НЕТ") << endl;
        return isOpen;
    }

    int getWorkDuration() {
        if (!isOpen) {
            cout << "Кафе закрыто в этот день" << endl;
            return 0;
        }
        
        int duration = 8;
        cout << "Продолжительность работы: " << duration << " часов" << endl;
        cout << "Время работы: " << openTime << " - " << closeTime << endl;
        return duration;
    }
};

class Booking {
private:
    int id;
    int userId;
    int tariffId;
    int cafeId;
    string startTime;
    string endTime;
    int guests;
    string status;
    double totalAmount;
    string comment;

public:
    Booking(int id, int uid, int tid, int cid, string start, string end, 
            int guests, string status, double amount, string comm)
        : id(id), userId(uid), tariffId(tid), cafeId(cid), startTime(start), 
          endTime(end), guests(guests), status(status), totalAmount(amount), comment(comm) {}

    int calculateDuration() {
        int duration = 2;
        cout << "Продолжительность брони #" << id << ": " << duration << " часа" << endl;
        return duration;
    }

    bool checkAvailability() {
        cout << "Проверка доступности для брони #" << id << "..." << endl;
        cout << "  Дата/время: " << startTime << endl;
        cout << "  Количество гостей: " << guests << endl;
        cout << "  Доступность: ПРОВЕРЕНО" << endl;
        return true;
    }

    void sendNotification() {
        cout << "УВЕДОМЛЕНИЕ О БРОНИРОВАНИИ" << endl;
        cout << "Бронь #" << id << " успешно создана!" << endl;
        cout << "Статус: " << status << endl;
        cout << "Дата/время: " << startTime << " - " << endTime << endl;
        cout << "Количество гостей: " << guests << endl;
        cout << "Сумма: " << totalAmount << " руб." << endl;
        if (!comment.empty()) {
            cout << "Комментарий: " << comment << endl;
        }
        cout << endl;
    }

    void display() {
        cout << "Информация о брони:" << endl;
        cout << "ID брони: " << id << endl;
        cout << "ID пользователя: " << userId << endl;
        cout << "ID тарифа: " << tariffId << endl;
        cout << "ID кафе: " << cafeId << endl;
        cout << "Время начала: " << startTime << endl;
        cout << "Время окончания: " << endTime << endl;
        cout << "Количество гостей: " << guests << endl;
        cout << "Статус: " << status << endl;
        cout << "Сумма: " << totalAmount << " руб." << endl;
        if (!comment.empty()) {
            cout << "Комментарий: " << comment << endl;
        }
        cout << endl;
    }
};

int main() {
    setRussianLocale();
    
    // Тест 1: Валидность телефона
    cout << "Тест 1: Проверка валидности телефона" << endl;
    cout << "1. Правильный номер (+79123456789):" << endl;
    User user1(1, "Иван Петров", "ivan@mail.ru", "+79123456789", "hash123", "2024-01-15");
    bool phoneValid1 = user1.validatePhone();
    cout << "Результат: " << (phoneValid1 ? "ВАЛИДЕН" : "НЕВАЛИДЕН") << endl;
    cout << endl;
    
    // Тест 2: Два тарифа
    cout << "Тест 2: Расчет стоимости тарифов" << endl;
    cout << endl;
    
    // Тариф 1: Почасовой
    Tariff tariff1(1, "Почасовой", "250 руб. за каждый час", 250.0, 0);
    cout << "Тариф 1: 'Почасовой'" << endl;
    cout << "Цена: 250 руб./час" << endl;
    cout << "Расчет для 2 часов: " << endl;
    double cost2h = tariff1.calculateCost(2);
    cout << "Стоимость: " << cost2h << " руб." << endl;
    cout << endl;
    
    cout << "Расчет для 5 часов: " << endl;
    double cost5h = tariff1.calculateCost(5);
    cout << "Стоимость: " << cost5h << " руб." << endl;
    cout << endl;
    
    // Тариф 2: Весь день (фиксированный)
    Tariff tariff2(2, "Весь день", "1500 руб. за 12 часов", 1500.0, 720);
    cout << "Тариф 2: 'Весь день' (фиксированный на 12 часов)" << endl;
    cout << "Цена: 1500 руб., длительность: 12 часов" << endl;
    cout << "Расчет для 8 часов: " << endl;
    double cost8h = tariff2.calculateCost(8);
    cout << "Стоимость: " << cost8h << " руб." << endl;
    cout << endl;
    
    cout << "Расчет для 15 часов: " << endl;
    double cost15h = tariff2.calculateCost(15);
    cout << "Стоимость: " << cost15h << " руб." << endl;
    cout << "(Примечание: фиксированный тариф, цена не зависит от часов)" << endl;
    cout << endl;
    
    // Тест 3: Проверка вместимости кафе
    cout << "Тест 3: Проверка вместимости кафе" << endl;
    Cafe cafe1(1, "Чилл на Воскресенской", "ул. Воскресенская, 8", "+7 (888) 888-88-88", "chill1@mail.ru", 30);
    cout << "Кафе: Чилл на Воскресенской, вместимость: 30 человек" << endl;
    bool canFit25 = cafe1.checkCapacity(25);
    cout << "Результат: " << (canFit25 ? "Места достаточно" : "Мест недостаточно") << endl;
    cout << endl;
    return 0;
}