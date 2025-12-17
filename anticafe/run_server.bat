@echo off
chcp 65001
title Антикафе - Запуск системы

echo ========================================
echo   Запуск системы Антикафе
echo ========================================

REM Проверка наличия Docker
where docker >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ Docker не установлен!
    echo Установите Docker Desktop: https://www.docker.com/products/docker-desktop/
    pause
    exit /b 1
)

REM Запуск базы данных
echo 🗄️  Запуск базы данных PostgreSQL...
cd docker
docker-compose up -d
timeout /t 5 /nobreak >nul

REM Проверка запуска БД
docker ps | findstr "anticafe_db" >nul
if %errorlevel% neq 0 (
    echo ❌ Не удалось запустить базу данных
    pause
    exit /b 1
)

echo ✅ База данных запущена

REM Запуск бэкенда
echo 🔧 Запуск сервера бэкенда...
cd ..\backend
start cmd /k "python server.py"
timeout /t 3 /nobreak >nul

REM Запуск фронтенда
echo 🌐 Запуск веб-интерфейса...
cd ..\frontend
start "" http://localhost:8080
start "" index.html

echo ========================================
echo ✅ Система запущена!
echo 🌐 Фронтенд: http://localhost:8080
echo 🗄️  База данных: localhost:5432
echo 📊 PGAdmin: http://localhost:5050 (admin@anticafe.com/admin123)
echo ========================================
echo Нажмите любую клавишу для завершения...
pause >nul

REM Остановка системы
echo Остановка системы...
cd ..\docker
docker-compose down
taskkill /F /IM python.exe 2>nul
echo Система остановлена