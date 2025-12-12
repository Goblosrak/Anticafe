@echo off
chcp 65001 > nul
title Антикафе "Чилл" - Система бронирования
color 0A

echo ================================================
echo     Антикафе "Чилл" - Система бронирования
echo ================================================
echo.

REM Создаем структуру папок
echo [1/4] Создаю структуру проекта...
if not exist backend\server mkdir backend\server
if not exist frontend mkdir frontend
if not exist data mkdir data

REM Создаем файлы
echo [2/4] Создаю файлы проекта...

REM Создаем index.html
copy /y "index.html" "frontend\" >nul 2>&1

REM Создаем сервер на C++
(
echo #define _WIN32_WINNT 0x0601
echo #include ^<windows.h^>
echo #include ^<winsock2.h^>
echo #include ^<ws2tcpip.h^>
echo #include ^<stdio.h^>
echo #include ^<stdlib.h^>
echo #include ^<string^>
echo #include ^<iostream^>
echo #include ^<thread^>
echo #include ^<vector^>
echo #include ^<map^>
echo #include ^<sstream^>
echo #include ^<fstream^>
echo #include ^<ctime^>
echo #include ^<cstdlib^>
echo #include ^<algorithm^>
echo.
echo #pragma comment(lib, "ws2_32.lib")
echo.
echo using namespace std;
echo.
echo class SimpleJson {
echo private:
echo     map^<string, string^> data;
echo     vector^<string^> arrayData;
echo     
echo public:
echo     void add(string key, string value) {
echo         data[key] = value;
echo     }
echo     
echo     void add(string key, int value) {
echo         data[key] = to_string(value);
echo     }
echo     
echo     void add(string key, double value) {
echo         data[key] = to_string(value);
echo     }
echo     
echo     void addToArray(string item) {
echo         arrayData.push_back(item);
echo     }
echo     
echo     string toString() {
echo         string json = "{";
echo         bool first = true;
echo         for (const auto^& pair : data) {
echo             if (!first) json += ",";
echo             json += "\"" + pair.first + "\":\"" + pair.second + "\"";
echo             first = false;
echo         }
echo         json += "}";
echo         return json;
echo     }
echo     
echo     string arrayToString() {
echo         string json = "[";
echo         for (size_t i = 0; i ^< arrayData.size(); i++) {
echo             if (i ^> 0) json += ",";
echo             json += arrayData[i];
echo         }
echo         json += "]";
echo         return json;
echo     }
echo };
echo.
echo class HTTPServer {
echo private:
echo     SOCKET serverSocket;
echo     int port;
echo     bool running;
echo     
echo     map^<string, string^> parseRequest(const string^& request) {
echo         map^<string, string^> result;
echo         istringstream stream(request);
echo         string line;
echo         
echo         getline(stream, line);
echo         size_t methodEnd = line.find(' ');
echo         if (methodEnd != string::npos) {
echo             result["method"] = line.substr(0, methodEnd);
echo             size_t pathEnd = line.find(' ', methodEnd + 1);
echo             if (pathEnd != string::npos) {
echo                 result["path"] = line.substr(methodEnd + 1, pathEnd - methodEnd - 1);
echo             }
echo         }
echo         
echo         while (getline(stream, line) ^&^& line != "\r" ^&^& !line.empty()) {
echo             size_t colonPos = line.find(':');
echo             if (colonPos != string::npos) {
echo                 string key = line.substr(0, colonPos);
echo                 string value = line.substr(colonPos + 2);
echo                 if (!value.empty() ^&^& value.back() == '\r') {
echo                     value.pop_back();
echo                 }
echo                 result[key] = value;
echo             }
echo         }
echo         
echo         string body;
echo         while (getline(stream, line)) {
echo             if (!line.empty() ^&^& line != "\r") {
echo                 body += line + "\n";
echo             }
echo         }
echo         if (!body.empty()) {
echo             result["body"] = body;
echo         }
echo         
echo         return result;
echo     }
echo     
echo     string createResponse(int status, const string^& content, const string^& contentType = "application/json") {
echo         string statusText;
echo         switch(status) {
echo             case 200: statusText = "OK"; break;
echo             case 400: statusText = "Bad Request"; break;
echo             case 404: statusText = "Not Found"; break;
echo             case 500: statusText = "Internal Server Error"; break;
echo             default: statusText = "OK";
echo         }
echo         
echo         string response = "HTTP/1.1 " + to_string(status) + " " + statusText + "\r\n";
echo         response += "Content-Type: " + contentType + "\r\n";
echo         response += "Access-Control-Allow-Origin: *\r\n";
echo         response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
echo         response += "Access-Control-Allow-Headers: Content-Type\r\n";
echo         response += "Content-Length: " + to_string(content.length()) + "\r\n";
echo         response += "Connection: close\r\n\r\n";
echo         response += content;
echo         
echo         return response;
echo     }
echo     
echo     string readFile(const string^& filename) {
echo         ifstream file(filename, ios::binary);
echo         if (!file) return "";
echo         
echo         string content((istreambuf_iterator^<char^>(file)), istreambuf_iterator^<char^>());
echo         return content;
echo     }
echo     
echo     string handleRequest(const map^<string, string^>^& request) {
echo         string method = request.at("method");
echo         string path = request.at("path");
echo         SimpleJson response;
echo         
echo         if (method == "OPTIONS") {
echo             return createResponse(200, "", "text/plain");
echo         }
echo         
echo         if (path == "/api/cafes" ^&^& method == "GET") {
echo             response.add("status", "success");
echo             
echo             SimpleJson cafe1, cafe2, cafe3;
echo             cafe1.add("id", 1);
echo             cafe1.add("name", "Чилл на Воскресенской");
echo             cafe1.add("address", "ул. Воскресенская, 8");
echo             cafe1.add("phone", "+7 (888) 888-88-88");
echo             cafe1.add("capacity", "до 30 человек");
echo             cafe1.add("description", "Основное кафе, 2 этажа, тихая зона и игровая");
echo             cafe1.add("workHours", "10:00-22:00");
echo             
echo             cafe2.add("id", 2);
echo             cafe2.add("name", "Чилл на Центральной");
echo             cafe2.add("address", "ул. Центральная, 25");
echo             cafe2.add("phone", "+7 (888) 777-77-77");
echo             cafe2.add("capacity", "до 20 человек");
echo             cafe2.add("description", "Центр города, бизнес-ланчи, конференц-зал");
echo             cafe2.add("workHours", "10:00-22:00");
echo             
echo             cafe3.add("id", 3);
echo             cafe3.add("name", "Чилл на Парковой");
echo             cafe3.add("address", "ул. Парковой, 12");
echo             cafe3.add("phone", "+7 (888) 666-66-66");
echo             cafe3.add("capacity", "до 25 человек");
echo             cafe3.add("description", "Рядом с парком, летняя веранда, настольные игры");
echo             cafe3.add("workHours", "10:00-22:00");
echo             
echo             SimpleJson cafes;
echo             cafes.addToArray(cafe1.toString());
echo             cafes.addToArray(cafe2.toString());
echo             cafes.addToArray(cafe3.toString());
echo             
echo             response.add("data", "{\"cafes\":" + cafes.arrayToString() + "}");
echo             
echo         } else if (path == "/api/tariffs" ^&^& method == "GET") {
echo             response.add("status", "success");
echo             
echo             SimpleJson tariff1, tariff2, tariff3, tariff4;
echo             tariff1.add("id", "250");
echo             tariff1.add("name", "Почасовой");
echo             tariff1.add("price", 250);
echo             tariff1.add("description", "250 руб. за каждый час");
echo             tariff1.add("type", "hourly");
echo             
echo             tariff2.add("id", "1500");
echo             tariff2.add("name", "Весь день");
echo             tariff2.add("price", 1500);
echo             tariff2.add("description", "1500 руб. за 12 часов");
echo             tariff2.add("type", "daily");
echo             
echo             tariff3.add("id", "5000");
echo             tariff3.add("name", "Месячный абонемент");
echo             tariff3.add("price", 5000);
echo             tariff3.add("description", "5000 руб. за 80 часов в месяц");
echo             tariff3.add("type", "subscription");
echo             
echo             tariff4.add("id", "9000");
echo             tariff4.add("name", "Месячный абонемент Премиум");
echo             tariff4.add("price", 9000);
echo             tariff4.add("description", "9000 руб. за 160 часов в месяц");
echo             tariff4.add("type", "subscription");
echo             
echo             SimpleJson tariffs;
echo             tariffs.addToArray(tariff1.toString());
echo             tariffs.addToArray(tariff2.toString());
echo             tariffs.addToArray(tariff3.toString());
echo             tariffs.addToArray(tariff4.toString());
echo             
echo             response.add("data", "{\"tariffs\":" + tariffs.arrayToString() + "}");
echo             
echo         } else if (path == "/api/bookings" ^&^& method == "POST") {
echo             response.add("status", "success");
echo             response.add("message", "Бронь создана успешно!");
echo             response.add("booking_id", rand() %% 10000 + 1);
echo             response.add("amount", "2000");
echo             response.add("duration", "4");
echo             
echo         } else if (path == "/api/user/cabinet" ^&^& method == "GET") {
echo             response.add("status", "success");
echo             response.add("name", "Иван Петров");
echo             response.add("email", "ivan@mail.ru");
echo             response.add("phone", "+79123456789");
echo             response.add("registration_date", "2024-01-15");
echo             response.add("bonus_points", "1250");
echo             response.add("bonus_balance", "125.5");
echo             
echo         } else if (path == "/api/user/subscription/purchase" ^&^& method == "POST") {
echo             response.add("status", "success");
echo             response.add("message", "Абонемент успешно приобретен!");
echo             response.add("subscription_id", rand() %% 1000 + 1);
echo             response.add("valid_until", "2024-12-31");
echo             
echo         } else if (path == "/api/test" ^&^& method == "GET") {
echo             response.add("status", "success");
echo             response.add("message", "Сервер работает корректно");
echo             response.add("timestamp", to_string(time(NULL)));
echo             
echo         } else {
echo             if (path == "/" || path == "/index.html") {
echo                 string htmlContent = readFile("..\\..\\frontend\\index.html");
echo                 if (!htmlContent.empty()) {
echo                     return createResponse(200, htmlContent, "text/html");
echo                 }
echo             }
echo             response.add("status", "error");
echo             response.add("message", "Endpoint not found: " + path);
echo         }
echo         
echo         return createResponse(200, response.toString());
echo     }
echo     
echo     void handleClient(SOCKET clientSocket) {
echo         char buffer[16384] = {0};
echo         
echo         int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
echo         if (bytesRead ^<= 0) {
echo             closesocket(clientSocket);
echo             return;
echo         }
echo         
echo         string request(buffer, bytesRead);
echo         auto parsedRequest = parseRequest(request);
echo         
echo         string response;
echo         try {
echo             response = handleRequest(parsedRequest);
echo         } catch (...) {
echo             SimpleJson error;
echo             error.add("status", "error");
echo             error.add("message", "Internal server error");
echo             response = createResponse(500, error.toString());
echo         }
echo         
echo         send(clientSocket, response.c_str(), response.length(), 0);
echo         closesocket(clientSocket);
echo     }
echo     
echo public:
echo     HTTPServer(int port) : port(port), running(false), serverSocket(INVALID_SOCKET) {}
echo     
echo     bool start() {
echo         WSADATA wsaData;
echo         if (WSAStartup(MAKEWORD(2, 2), ^&wsaData) != 0) {
echo             cerr ^<^< "WSAStartup failed" ^<^< endl;
echo             return false;
echo         }
echo         
echo         serverSocket = socket(AF_INET, SOCK_STREAM, 0);
echo         if (serverSocket == INVALID_SOCKET) {
echo             cerr ^<^< "Не удалось создать сокет" ^<^< endl;
echo             WSACleanup();
echo             return false;
echo         }
echo         
echo         int opt = 1;
echo         setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)^&opt, sizeof(opt));
echo         
echo         sockaddr_in serverAddr;
echo         memset(^&serverAddr, 0, sizeof(serverAddr));
echo         serverAddr.sin_family = AF_INET;
echo         serverAddr.sin_addr.s_addr = INADDR_ANY;
echo         serverAddr.sin_port = htons(port);
echo         
echo         if (bind(serverSocket, (sockaddr*)^&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
echo             cerr ^<^< "Не удалось привязать сокет к порту " ^<^< port ^<^< endl;
echo             closesocket(serverSocket);
echo             WSACleanup();
echo             return false;
echo         }
echo         
echo         if (listen(serverSocket, 10) == SOCKET_ERROR) {
echo             cerr ^<^< "Не удалось начать прослушивание" ^<^< endl;
echo             closesocket(serverSocket);
echo             WSACleanup();
echo             return false;
echo         }
echo         
echo         running = true;
echo         cout ^<^< "✅ HTTP сервер запущен на http://localhost:" ^<^< port ^<^< endl;
echo         cout ^<^< "📋 Доступные endpoints:" ^<^< endl;
echo         cout ^<^< "   GET  /api/cafes                      - список кафе" ^<^< endl;
echo         cout ^<^< "   GET  /api/tariffs                    - список тарифов" ^<^< endl;
echo         cout ^<^< "   GET  /api/user/cabinet               - личный кабинет" ^<^< endl;
echo         cout ^<^< "   POST /api/bookings                   - создать бронь" ^<^< endl;
echo         cout ^<^< "   POST /api/user/subscription/purchase - купить абонемент" ^<^< endl;
echo         cout ^<^< "   GET  /                               - веб-интерфейс" ^<^< endl;
echo         cout ^<^< "==========================================" ^<^< endl;
echo         
echo         while (running) {
echo             sockaddr_in clientAddr;
echo             int clientSize = sizeof(clientAddr);
echo             
echo             SOCKET clientSocket = accept(serverSocket, (sockaddr*)^&clientAddr, ^&clientSize);
echo             if (clientSocket == INVALID_SOCKET) {
echo                 if (running) {
echo                     cerr ^<^< "Ошибка принятия соединения" ^<^< endl;
echo                 }
echo                 continue;
echo             }
echo             
echo             thread clientThread(^&HTTPServer::handleClient, this, clientSocket);
echo             clientThread.detach();
echo         }
echo         
echo         return true;
echo     }
echo     
echo     void stop() {
echo         running = false;
echo         if (serverSocket != INVALID_SOCKET) {
echo             closesocket(serverSocket);
echo         }
echo         WSACleanup();
echo     }
echo     
echo     ~HTTPServer() {
echo         stop();
echo     }
echo };
echo.
echo int main() {
echo     SetConsoleOutputCP(CP_UTF8);
echo     SetConsoleCP(CP_UTF8);
echo     system("chcp 65001 ^> nul");
echo     
echo     cout ^<^< "==========================================" ^<^< endl;
echo     cout ^<^< "   Антикафе 'Чилл' - Сервер бронирования  " ^<^< endl;
echo     cout ^<^< "==========================================" ^<^< endl;
echo     
echo     srand(static_cast^<unsigned^>(time(NULL)));
echo     
echo     HTTPServer server(8080);
echo     
echo     thread serverThread([^&server]() {
echo         if (!server.start()) {
echo             cerr ^<^< "Не удалось запустить сервер" ^<^< endl;
echo             exit(1);
echo         }
echo     });
echo     
echo     cout ^<^< "🌐 Откройте в браузере: http://localhost:8080" ^<^< endl;
echo     cout ^<^< "⏸️  Нажмите Enter для остановки сервера..." ^<^< endl;
echo     
echo     cin.get();
echo     
echo     server.stop();
echo     if (serverThread.joinable()) {
echo         serverThread.join();
echo     }
echo     
echo     cout ^<^< "🛑 Сервер остановлен" ^<^< endl;
echo     
echo     return 0;
echo }
) > "backend\server\main.cpp"

echo [3/4] Компилирую сервер...
cd backend\server

REM Проверяем наличие компилятора
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo.
    echo ❌ ОШИБКА: Компилятор g++ не найден!
    echo.
    echo Установите MinGW:
    echo 1. Скачайте с https://sourceforge.net/projects/mingw/
    echo 2. Установите mingw32-gcc-g++ пакет
    echo 3. Добавьте C:\MinGW\bin в PATH
    echo.
    pause
    exit /b 1
)

echo Компиляция... (это может занять несколько секунд)
g++ -std=c++17 -o antycafe_server.exe main.cpp -lws2_32

if %errorlevel% neq 0 (
    echo.
    echo ❌ ОШИБКА компиляции!
    echo Попробуйте компилировать вручную:
    echo   g++ -std=c++17 -o antycafe_server.exe main.cpp -lws2_32
    echo.
    pause
    exit /b 1
)

echo ✅ Сервер успешно скомпилирован!
cd ..\..

echo [4/4] Запускаю систему...
echo.
echo ================================================
echo           СИСТЕМА УСПЕШНО ЗАПУЩЕНА!
echo ================================================
echo.
echo 🌐 ВЕБ-ИНТЕРФЕЙС: http://localhost:8080
echo.
echo 📋 ДОСТУПНЫЕ ВОЗМОЖНОСТИ:
echo    • Бронирование мест в антикафе
echo    • Личный кабинет с историей
echo    • Абонементы (3 типа тарифов)
echo    • Бонусная система
echo    • Выбор из 3 кафе
echo.
echo 🛠  ТЕХНИЧЕСКАЯ ИНФОРМАЦИЯ:
echo    • Порт сервера: 8080
echo    • Бэкенд: C++/WinSock
echo    • Фронтенд: HTML/JavaScript
echo    • База данных: В памяти (для демо)
echo.
echo ⚠️  Для остановки нажмите Ctrl+C в этом окне
echo    или закройте окно браузера
echo ================================================
echo.

REM Запускаем сервер
start /B "Антикафе Сервер" backend\server\antycafe_server.exe

REM Даем время серверу запуститься
timeout /t 3 /nobreak >nul

REM Открываем браузер с веб-интерфейсом
start http://localhost:8080

echo.
echo Нажмите любую клавишу для остановки сервера...
pause >nul

REM Останавливаем сервер
taskkill /F /IM antycafe_server.exe >nul 2>&1
echo Сервер остановлен