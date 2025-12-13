#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cstring>

// Linux socket headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <poll.h>

using namespace std;

class SimpleJson {
private:
    map<string, string> data;
    vector<string> arrayData;
    
public:
    void add(string key, string value) {
        data[key] = value;
    }
    
    void add(string key, int value) {
        data[key] = to_string(value);
    }
    
    void add(string key, double value) {
        data[key] = to_string(value);
    }
    
    void addToArray(string item) {
        arrayData.push_back(item);
    }
    
    string toString() {
        string json = "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) json += ",";
            json += "\"" + pair.first + "\":\"" + pair.second + "\"";
            first = false;
        }
        json += "}";
        return json;
    }
    
    string arrayToString() {
        string json = "[";
        for (size_t i = 0; i < arrayData.size(); i++) {
            if (i > 0) json += ",";
            json += arrayData[i];
        }
        json += "]";
        return json;
    }
};

class HTTPServer {
private:
    int serverSocket;
    int port;
    bool running;
    
    map<string, string> parseRequest(const string& request) {
        map<string, string> result;
        istringstream stream(request);
        string line;
        
        getline(stream, line);
        size_t methodEnd = line.find(' ');
        if (methodEnd != string::npos) {
            result["method"] = line.substr(0, methodEnd);
            size_t pathEnd = line.find(' ', methodEnd + 1);
            if (pathEnd != string::npos) {
                result["path"] = line.substr(methodEnd + 1, pathEnd - methodEnd - 1);
            }
        }
        
        while (getline(stream, line) && line != "\r" && !line.empty()) {
            size_t colonPos = line.find(':');
            if (colonPos != string::npos) {
                string key = line.substr(0, colonPos);
                string value = line.substr(colonPos + 2);
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                result[key] = value;
            }
        }
        
        string body;
        while (getline(stream, line)) {
            if (!line.empty() && line != "\r") {
                body += line + "\n";
            }
        }
        if (!body.empty()) {
            result["body"] = body;
        }
        
        return result;
    }
    
    string createResponse(int status, const string& content, const string& contentType = "application/json") {
        string statusText;
        switch(status) {
            case 200: statusText = "OK"; break;
            case 400: statusText = "Bad Request"; break;
            case 404: statusText = "Not Found"; break;
            case 500: statusText = "Internal Server Error"; break;
            default: statusText = "OK";
        }
        
        string response = "HTTP/1.1 " + to_string(status) + " " + statusText + "\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        response += "Access-Control-Allow-Headers: Content-Type\r\n";
        response += "Content-Length: " + to_string(content.length()) + "\r\n";
        response += "Connection: close\r\n\r\n";
        response += content;
        
        return response;
    }
    
    string readFile(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) return "";
        
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        return content;
    }
    
    string handleRequest(const map<string, string>& request) {
        string method = request.at("method");
        string path = request.at("path");
        SimpleJson response;
        
        if (method == "OPTIONS") {
            return createResponse(200, "", "text/plain");
        }
        
        if (path == "/api/cafes" && method == "GET") {
            response.add("status", "success");
            
            SimpleJson cafe1, cafe2, cafe3;
            cafe1.add("id", 1);
            cafe1.add("name", "Чилл на Воскресенской");
            cafe1.add("address", "ул. Воскресенская, 8");
            cafe1.add("phone", "+7 (888) 888-88-88");
            cafe1.add("capacity", "до 30 человек");
            cafe1.add("description", "Основное кафе, 2 этажа, тихая зона и игровая");
            cafe1.add("workHours", "10:00-22:00");
            
            cafe2.add("id", 2);
            cafe2.add("name", "Чилл на Центральной");
            cafe2.add("address", "ул. Центральная, 25");
            cafe2.add("phone", "+7 (888) 777-77-77");
            cafe2.add("capacity", "до 20 человек");
            cafe2.add("description", "Центр города, бизнес-ланчи, конференц-зал");
            cafe2.add("workHours", "10:00-22:00");
            
            cafe3.add("id", 3);
            cafe3.add("name", "Чилл на Парковой");
            cafe3.add("address", "ул. Парковой, 12");
            cafe3.add("phone", "+7 (888) 666-66-66");
            cafe3.add("capacity", "до 25 человек");
            cafe3.add("description", "Рядом с парком, летняя веранда, настольные игры");
            cafe3.add("workHours", "10:00-22:00");
            
            SimpleJson cafes;
            cafes.addToArray(cafe1.toString());
            cafes.addToArray(cafe2.toString());
            cafes.addToArray(cafe3.toString());
            
            response.add("data", "{\"cafes\":" + cafes.arrayToString() + "}");
            
        } else if (path == "/api/tariffs" && method == "GET") {
            response.add("status", "success");
            
            SimpleJson tariff1, tariff2, tariff3, tariff4;
            tariff1.add("id", "250");
            tariff1.add("name", "Почасовой");
            tariff1.add("price", 250);
            tariff1.add("description", "250 руб. за каждый час");
            tariff1.add("type", "hourly");
            
            tariff2.add("id", "1500");
            tariff2.add("name", "Весь день");
            tariff2.add("price", 1500);
            tariff2.add("description", "1500 руб. за 12 часов");
            tariff2.add("type", "daily");
            
            tariff3.add("id", "5000");
            tariff3.add("name", "Месячный абонемент");
            tariff3.add("price", 5000);
            tariff3.add("description", "5000 руб. за 80 часов в месяц");
            tariff3.add("type", "subscription");
            
            tariff4.add("id", "9000");
            tariff4.add("name", "Месячный абонемент Премиум");
            tariff4.add("price", 9000);
            tariff4.add("description", "9000 руб. за 160 часов в месяц");
            tariff4.add("type", "subscription");
            
            SimpleJson tariffs;
            tariffs.addToArray(tariff1.toString());
            tariffs.addToArray(tariff2.toString());
            tariffs.addToArray(tariff3.toString());
            tariffs.addToArray(tariff4.toString());
            
            response.add("data", "{\"tariffs\":" + tariffs.arrayToString() + "}");
            
        } else if (path == "/api/bookings" && method == "POST") {
            response.add("status", "success");
            response.add("message", "Бронь создана успешно!");
            response.add("booking_id", rand() % 10000 + 1);
            response.add("amount", "2000");
            response.add("duration", "4");
            
        } else if (path == "/api/user/cabinet" && method == "GET") {
            response.add("status", "success");
            response.add("name", "Иван Петров");
            response.add("email", "ivan@mail.ru");
            response.add("phone", "+79123456789");
            response.add("registration_date", "2024-01-15");
            response.add("bonus_points", "1250");
            response.add("bonus_balance", "125.5");
            
        } else if (path == "/api/user/subscription/purchase" && method == "POST") {
            response.add("status", "success");
            response.add("message", "Абонемент успешно приобретен!");
            response.add("subscription_id", rand() % 1000 + 1);
            response.add("valid_until", "2024-12-31");
            
        } else if (path == "/api/test" && method == "GET") {
            response.add("status", "success");
            response.add("message", "Сервер работает корректно");
            response.add("timestamp", to_string(time(NULL)));
            
        } else {
            if (path == "/" || path == "/index.html") {
                string htmlContent = readFile("../frontend/index.html");
                if (!htmlContent.empty()) {
                    return createResponse(200, htmlContent, "text/html");
                }
            }
            response.add("status", "error");
            response.add("message", "Endpoint not found: " + path);
        }
        
        return createResponse(200, response.toString());
    }
    
    void handleClient(int clientSocket) {
        char buffer[16384] = {0};
        
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            close(clientSocket);
            return;
        }
        
        string request(buffer, bytesRead);
        auto parsedRequest = parseRequest(request);
        
        string response;
        try {
            response = handleRequest(parsedRequest);
        } catch (...) {
            SimpleJson error;
            error.add("status", "error");
            error.add("message", "Internal server error");
            response = createResponse(500, error.toString());
        }
        
        send(clientSocket, response.c_str(), response.length(), 0);
        close(clientSocket);
    }
    
public:
    HTTPServer(int port) : port(port), running(false), serverSocket(-1) {}
    
    bool start() {
        // Создание сокета
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            cerr << "Не удалось создать сокет" << endl;
            return false;
        }
        
        // Настройка опций сокета
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // Настройка адреса
        sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        
        // Привязка сокета
        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            cerr << "Не удалось привязать сокет к порту " << port << endl;
            close(serverSocket);
            return false;
        }
        
        // Начало прослушивания
        if (listen(serverSocket, 10) < 0) {
            cerr << "Не удалось начать прослушивание" << endl;
            close(serverSocket);
            return false;
        }
        
        running = true;
        cout << "✅ HTTP сервер запущен на http://localhost:" << port << endl;
        cout << "📋 Доступные endpoints:" << endl;
        cout << "   GET  /api/cafes                      - список кафе" << endl;
        cout << "   GET  /api/tariffs                    - список тарифов" << endl;
        cout << "   GET  /api/user/cabinet               - личный кабинет" << endl;
        cout << "   POST /api/bookings                   - создать бронь" << endl;
        cout << "   POST /api/user/subscription/purchase - купить абонемент" << endl;
        cout << "   GET  /                               - веб-интерфейс" << endl;
        cout << "==========================================" << endl;
        
        while (running) {
            sockaddr_in clientAddr;
            socklen_t clientSize = sizeof(clientAddr);
            
            int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
            if (clientSocket < 0) {
                if (running) {
                    cerr << "Ошибка принятия соединения" << endl;
                }
                continue;
            }
            
            thread clientThread([this, clientSocket]() {
                this->handleClient(clientSocket);
            });
            clientThread.detach();
        }
        
        return true;
    }
    
    void stop() {
        running = false;
        if (serverSocket >= 0) {
            close(serverSocket);
        }
    }
    
    ~HTTPServer() {
        stop();
    }
};

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    cout << "==========================================" << endl;
    cout << "   Антикафе 'Чилл' - Сервер бронирования  " << endl;
    cout << "==========================================" << endl;
    
    srand(static_cast<unsigned>(time(NULL)));
    
    HTTPServer server(8080);
    
    thread serverThread([&server]() {
        if (!server.start()) {
            cerr << "Не удалось запустить сервер" << endl;
            exit(1);
        }
    });
    
    cout << "🌐 Откройте в браузере: http://localhost:8080" << endl;
    cout << "⏸️  Нажмите Ctrl+C для остановки сервера..." << endl;
    
    // Ожидание Ctrl+C
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = [](int s) {
        cout << "\nПолучен сигнал SIGINT, останавливаю сервер..." << endl;
        exit(0);
    };
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    
    pause(); // Ожидание сигнала
    
    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
    
    cout << "🛑 Сервер остановлен" << endl;
    
    return 0;
}