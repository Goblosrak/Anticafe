@echo off
chcp 65001 > nul
title Антикафе "Чилл" - Запуск с Docker

echo ================================================
echo     Антикафе Chill - Docker + PostgreSQL
echo ================================================
echo.

echo Проверяю Docker...
docker --version >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ Docker не установлен!
    echo Скачайте с: https://www.docker.com/products/docker-desktop
    pause
    exit /b 1
)

echo Проверяю Docker Compose...
docker-compose --version >nul 2>nul
if %errorlevel% neq 0 (
    echo ⚠️  Docker Compose не найден, проверяю docker compose...
    docker compose version >nul 2>nul
    if %errorlevel% neq 0 (
        echo ❌ Docker Compose не установлен!
        pause
        exit /b 1
    )
    set DOCKER_COMPOSE=docker compose
) else (
    set DOCKER_COMPOSE=docker-compose
)

echo.
echo [1/4] Запускаю PostgreSQL и pgAdmin...
cd docker
%DOCKER_COMPOSE% up -d

echo.
echo [2/4] Ожидаю запуск базы данных (10 секунд)...
timeout /t 10 /nobreak >nul

echo.
echo [3/4] Информация о контейнерах:
echo.
docker ps --filter "name=anticafe" --format "table {{.Names}}\t{{.Status}}\t{{.Ports}}"
echo.

echo [4/4] Подключение к базам данных:
echo.
echo 📊 PostgreSQL:
echo    Хост: localhost:5432
echo    База: anticafe
echo    Пользователь: admin
echo    Пароль: password123
echo.
echo 📊 pgAdmin (веб-интерфейс):
echo    URL: http://localhost:5050
echo    Email: admin@anticafe.com
echo    Пароль: admin123
echo.
echo 🌐 Веб-приложение:
echo    URL: http://localhost:8080
echo.
echo ================================================
echo    ИНСТРУКЦИЯ ПО ПОДКЛЮЧЕНИЮ:
echo ================================================
echo.
echo 1. В pgAdmin добавьте сервер:
echo    - Name: Anticafe DB
echo    - Host: postgres (или localhost)
echo    - Port: 5432
echo    - Username: admin
echo    - Password: password123
echo.
echo 2. Компиляция сервера:
echo    cd backend
echo    g++ -o server.exe server.cpp -lws2_32 -llibpq
echo.
echo 3. Запуск сервера:
echo    server.exe
echo.
echo 4. Откройте браузер: http://localhost:8080
echo.
echo ================================================
echo.

echo Для остановки контейнеров выполните:
echo   cd docker
echo   %DOCKER_COMPOSE% down
echo.
pause