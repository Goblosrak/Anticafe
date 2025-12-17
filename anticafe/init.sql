-- Создание таблицы кафе
CREATE TABLE IF NOT EXISTS cafes (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    address VARCHAR(200) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    capacity INTEGER,
    description TEXT,
    work_hours VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Создание таблицы тарифов
CREATE TABLE IF NOT EXISTS tariffs (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    price INTEGER NOT NULL,
    description TEXT,
    type VARCHAR(20) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Создание таблицы пользователей
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    phone VARCHAR(20) UNIQUE NOT NULL,
    password_hash VARCHAR(255),
    registration_date DATE DEFAULT CURRENT_DATE,
    bonus_points INTEGER DEFAULT 0,
    bonus_balance DECIMAL(10,2) DEFAULT 0.00,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Создание таблицы бронирований
CREATE TABLE IF NOT EXISTS bookings (
    id SERIAL PRIMARY KEY,
    cafe_id INTEGER REFERENCES cafes(id),
    user_id INTEGER REFERENCES users(id),
    name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    date DATE NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    guests INTEGER DEFAULT 1,
    tariff VARCHAR(50) REFERENCES tariffs(id),
    duration INTEGER,
    cost INTEGER,
    comment TEXT,
    status VARCHAR(20) DEFAULT 'pending',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_bookings_date (date),
    INDEX idx_bookings_user (user_id)
);

-- Создание таблицы абонементов
CREATE TABLE IF NOT EXISTS subscriptions (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id),
    tariff_id VARCHAR(50) REFERENCES tariffs(id),
    purchase_date DATE DEFAULT CURRENT_DATE,
    valid_until DATE NOT NULL,
    status VARCHAR(20) DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Вставка тестовых данных кафе
INSERT INTO cafes (name, address, phone, capacity, description, work_hours) VALUES
('Чилл на Воскресенской', 'ул. Воскресенская, 8', '+7 (888) 888-88-88', 30, 'Основное кафе, 2 этажа, тихая зона и игровая', '10:00-22:00'),
('Чилл на Центральной', 'ул. Центральная, 25', '+7 (888) 777-77-77', 20, 'Центр города, бизнес-ланчи, конференц-зал', '10:00-22:00'),
('Чилл на Парковой', 'ул. Парковой, 12', '+7 (888) 666-66-66', 25, 'Рядом с парком, летняя веранда, настольные игры', '10:00-22:00')
ON CONFLICT DO NOTHING;

-- Вставка тестовых данных тарифов
INSERT INTO tariffs (id, name, price, description, type) VALUES
('hourly_250', 'Почасовой', 250, '250 руб. за каждый час', 'hourly'),
('daily_1500', 'Весь день', 1500, '1500 руб. за 12 часов', 'daily'),
('monthly_5000', 'Месячный абонемент', 5000, '5000 руб. за 80 часов в месяц', 'subscription'),
('premium_9000', 'Месячный абонемент Премиум', 9000, '9000 руб. за 160 часов в месяц', 'subscription')
ON CONFLICT DO NOTHING;

-- Вставка тестового пользователя
INSERT INTO users (name, email, phone, bonus_points, bonus_balance) VALUES
('Иван Петров', 'ivan@mail.ru', '+79123456789', 1250, 125.50)
ON CONFLICT DO NOTHING;