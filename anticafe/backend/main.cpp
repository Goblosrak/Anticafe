#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Функция для создания HTTP ответа
string createResponse(int status, const string& content, const string& contentType = "application/json") {
    string response = "HTTP/1.1 " + to_string(status) + " OK\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response += "Content-Length: " + to_string(content.length()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += content;
    return response;
}

// Чтение HTML файла
string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) return "";
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

// Генерация простого JSON
string generateJSON(const string& status, const string& data = "") {
    if (data.empty()) {
        return "{\"status\":\"" + status + "\"}";
    }
    return "{\"status\":\"" + status + "\",\"data\":" + data + "}";
}

int main() {
    cout << "Запуск сервера антикафе 'Чилл'..." << endl;
    
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed" << endl;
        return 1;
    }
    
    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Не удалось создать сокет" << endl;
        WSACleanup();
        return 1;
    }
    
    // Настройка опций сокета
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    // Настройка адреса
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    
    // Привязка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Не удалось привязать сокет к порту 8080" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    // Начало прослушивания
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        cout << "Не удалось начать прослушивание" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    cout << "✅ Сервер запущен на http://localhost:8080" << endl;
    cout << "📋 Доступные endpoints:" << endl;
    cout << "   GET  /api/cafes     - список кафе" << endl;
    cout << "   GET  /api/tariffs   - список тарифов" << endl;
    cout << "   GET  /api/test      - тест сервера" << endl;
    cout << "   GET  /              - веб-интерфейс" << endl;
    cout << "======================================" << endl;
    
    while (true) {
        // Принятие соединения
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        
        if (clientSocket == INVALID_SOCKET) {
            cout << "Ошибка принятия соединения" << endl;
            continue;
        }
        
        // Чтение запроса
        char buffer[4096] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead > 0) {
            string request(buffer, bytesRead);
            
            // Парсинг запроса (простой вариант)
            string method, path;
            size_t firstSpace = request.find(' ');
            if (firstSpace != string::npos) {
                method = request.substr(0, firstSpace);
                size_t secondSpace = request.find(' ', firstSpace + 1);
                if (secondSpace != string::npos) {
                    path = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                }
            }
            
            string response;
            
            // Обработка запросов
            if (path == "/api/test") {
                response = createResponse(200, generateJSON("success", "{\"message\":\"Сервер работает!\"}"));
            }
            else if (path == "/api/cafes") {
                string cafesData = "["
                    "{\"id\":1,\"name\":\"Чилл на Воскресенской\",\"address\":\"ул. Воскресенская, 8\"},"
                    "{\"id\":2,\"name\":\"Чилл на Центральной\",\"address\":\"ул. Центральная, 25\"},"
                    "{\"id\":3,\"name\":\"Чилл на Парковой\",\"address\":\"ул. Парковой, 12\"}"
                "]";
                response = createResponse(200, generateJSON("success", cafesData));
            }
            else if (path == "/api/tariffs") {
                string tariffsData = "["
                    "{\"id\":\"250\",\"name\":\"Почасовой\",\"price\":250,\"type\":\"hourly\"},"
                    "{\"id\":\"1500\",\"name\":\"Весь день\",\"price\":1500,\"type\":\"daily\"},"
                    "{\"id\":\"5000\",\"name\":\"Месячный абонемент\",\"price\":5000,\"type\":\"subscription\"}"
                "]";
                response = createResponse(200, generateJSON("success", tariffsData));
            }
            else if (path == "/" || path == "/index.html") {
                string htmlContent = readFile("../../frontend/index.html");
                if (!htmlContent.empty()) {
                    response = createResponse(200, htmlContent, "text/html");
                } else {
                    response = createResponse(404, generateJSON("error", "{\"message\":\"Файл не найден\"}"));
                }
            }
            else if (path.find("/api/") != string::npos) {
                response = createResponse(404, generateJSON("error", "{\"message\":\"Endpoint not found: " + path + "\"}"));
            }
            else {
                // Пробуем отдать как статический файл
                string htmlContent = readFile("../../frontend/index.html");
                if (!htmlContent.empty()) {
                    response = createResponse(200, htmlContent, "text/html");
                } else {
                    response = createResponse(200, "<h1>Антикафе Чилл</h1><p>Сервер работает!</p>", "text/html");
                }
            }
            
            // Отправка ответа
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        // Закрытие соединения
        closesocket(clientSocket);
    }
    
    // Закрытие сокета (не достигнем этого кода в бесконечном цикле)
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}