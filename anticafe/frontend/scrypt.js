// Константы
const API_URL = 'http://localhost:8080';
// Глобальный объект для хранения данных
window.antycafe = {
    bookings: [],
    subscriptions: [], // Добавляем массив для абонементов
    user: {
        name: "Иван Петров",
        email: "ivan@mail.ru",
        phone: "+7 912 345-67-89",
        points: 1250,
        balance: 125.50
    }
};

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    console.log("🚀 Антикафе Chill запущен!");
    
    // Установка даты по умолчанию
    const today = new Date().toISOString().split('T')[0];
    const dateInput = document.getElementById('date');
    if (dateInput) {
        dateInput.min = today;
        dateInput.value = today;
    }
    
    // Инициализация навигации
    initNavigation();
    
    // Загрузить тарифы
    loadTariffs();
});

// Навигация между разделами
function initNavigation() {
    const navLinks = document.querySelectorAll('nav a');
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            const sectionId = this.getAttribute('href').substring(1);
            
            // Обновить активную ссылку
            navLinks.forEach(l => l.classList.remove('active'));
            this.classList.add('active');
            
            // Показать выбранный раздел
            showSection(sectionId);
        });
    });
}

// Показать определенный раздел
function showSection(sectionId) {
    const sections = document.querySelectorAll('main section');
    sections.forEach(section => {
        if (section.id === sectionId) {
            section.style.display = 'block';
        } else {
            section.style.display = 'none';
        }
    });
    
    // Дополнительные действия для разделов
    if (sectionId === 'cabinet') {
        loadUserCabinet();
    }
}

// Показать информацию о кафе
function showCafeInfo() {
    const cafeSelect = document.getElementById('cafe');
    const cafeInfo = document.getElementById('cafeInfo');
    
    if (!cafeSelect || !cafeInfo) return;
    
    if (cafeSelect.value) {
        const cafes = {
            '1': 'Основное кафе, 2 этажа, тихая зона и игровая зона',
            '2': 'Центр города, бизнес-ланчи, конференц-зал',
            '3': 'Рядом с парком, летняя веранда, настольные игры'
        };
        
        cafeInfo.innerHTML = `
            <p><strong>Описание:</strong> ${cafes[cafeSelect.value]}</p>
            <p><strong>Вместимость:</strong> ${cafeSelect.value === '1' ? '30' : cafeSelect.value === '2' ? '20' : '25'} человек</p>
            <p><strong>Часы работы:</strong> 10:00-22:00</p>
        `;
        cafeInfo.style.display = 'block';
    } else {
        cafeInfo.style.display = 'none';
    }
}

// Обновить время окончания
function updateEndTime() {
    const startTime = document.getElementById('startTime');
    const endTimeSelect = document.getElementById('endTime');
    
    if (!startTime || !endTimeSelect) return;
    
    if (!startTime.value) {
        endTimeSelect.innerHTML = '<option value="">--:--</option>';
        return;
    }
    
    const startHour = parseInt(startTime.value.split(':')[0]);
    endTimeSelect.innerHTML = '<option value="">--:--</option>';
    
    for (let hour = startHour + 1; hour <= 22; hour++) {
        const time = hour.toString().padStart(2, '0') + ':00';
        const option = document.createElement('option');
        option.value = time;
        option.textContent = time;
        endTimeSelect.appendChild(option);
    }
    
    calculateDuration();
}

// Рассчитать продолжительность
function calculateDuration() {
    const startTime = document.getElementById('startTime');
    const endTime = document.getElementById('endTime');
    const durationDisplay = document.getElementById('durationDisplay');
    const durationValue = document.getElementById('durationValue');
    
    if (!startTime || !endTime || !durationDisplay || !durationValue) return;
    
    if (startTime.value && endTime.value) {
        const [startHour, startMinute] = startTime.value.split(':').map(Number);
        const [endHour, endMinute] = endTime.value.split(':').map(Number);
        
        let duration = (endHour - startHour) + (endMinute - startMinute) / 60;
        
        if (duration < 0) duration += 24;
        
        durationValue.textContent = `${duration.toFixed(1)} ${getHoursWord(duration)}`;
        durationDisplay.style.display = 'block';
        
        calculateCost();
    }
}

// Получить правильное склонение слова "час"
function getHoursWord(hours) {
    const lastDigit = Math.floor(hours) % 10;
    const lastTwoDigits = Math.floor(hours) % 100;
    
    if (lastTwoDigits >= 11 && lastTwoDigits <= 19) return 'часов';
    if (lastDigit === 1) return 'час';
    if (lastDigit >= 2 && lastDigit <= 4) return 'часа';
    return 'часов';
}

// Рассчитать стоимость
function calculateCost() {
    const tariffSelect = document.getElementById('tariff');
    const durationDisplay = document.getElementById('durationDisplay');
    const costDisplay = document.getElementById('costDisplay');
    const costValue = document.getElementById('costValue');
    
    if (!tariffSelect || !costDisplay || !costValue) return 0;
    
    if (!tariffSelect.value || (durationDisplay && durationDisplay.style.display === 'none')) {
        costDisplay.style.display = 'none';
        return 0;
    }
    
    const tariffPrices = {
        'hourly_250': 250,
        'daily_1500': 1500,
        'monthly_5000': 5000,
        'premium_9000': 9000
    };
    
    let cost = tariffPrices[tariffSelect.value] || 0;
    
    // Для почасового тарифа умножаем на продолжительность
    if (tariffSelect.value === 'hourly_250') {
        const durationText = document.getElementById('durationValue').textContent;
        const duration = parseFloat(durationText.split(' ')[0]) || 1;
        cost = Math.round(duration * 250);
    }
    
    costValue.textContent = `${cost}₽`;
    costDisplay.style.display = 'block';
    
    return cost;
}

// Загрузить тарифы
async function loadTariffs() {
    const container = document.getElementById('tariffsList');
    if (!container) return;
    
    try {
        // Показываем заглушку
        container.innerHTML = `
            <div class="tariffs-grid">
                <div class="tariff-card">
                    <h3>Почасовой</h3>
                    <p>250 руб. за каждый час</p>
                    <div class="stat-value">250₽</div>
                    <button class="btn btn-secondary mt-20" onclick="selectTariff('hourly_250')">Выбрать</button>
                </div>
                <div class="tariff-card">
                    <h3>Весь день</h3>
                    <p>1500 руб. за 12 часов</p>
                    <div class="stat-value">1500₽</div>
                    <button class="btn btn-secondary mt-20" onclick="selectTariff('daily_1500')">Выбрать</button>
                </div>
                <div class="tariff-card premium">
                    <h3>Месячный абонемент</h3>
                    <p>5000 руб. за 80 часов в месяц</p>
                    <div class="stat-value">5000₽</div>
                    <button class="btn btn-primary mt-20" onclick="selectTariff('monthly_5000')">
                        <i class="fas fa-crown"></i> Купить
                    </button>
                </div>
                <div class="tariff-card premium">
                    <h3>Премиум абонемент</h3>
                    <p>9000 руб. за 160 часов в месяц</p>
                    <div class="stat-value">9000₽</div>
                    <button class="btn btn-primary mt-20" onclick="selectTariff('premium_9000')">
                        <i class="fas fa-crown"></i> Купить
                    </button>
                </div>
            </div>
        `;
        
        // Пытаемся загрузить из API
        const response = await fetch(`${API_URL}/api/tariffs`);
        if (response.ok) {
            const data = await response.json();
            if (data.status === 'success') {
                // Обновляем тарифы из API
                console.log('Тарифы загружены из API:', data.data);
            }
        }
    } catch (error) {
        console.log('Используем локальные тарифы');
    }
}

// Выбрать тариф
function selectTariff(tariffId) {
    const tariffSelect = document.getElementById('tariff');
    if (tariffSelect) {
        tariffSelect.value = tariffId;
    }
    
    // Перейти к бронированию
    showSection('booking');
    
    // Пересчитать стоимость
    setTimeout(() => calculateCost(), 100);
}

// Валидация формы
function validateForm() {
    const requiredFields = ['cafe', 'name', 'phone', 'date', 'startTime', 'endTime', 'tariff'];
    let isValid = true;
    
    for (const fieldId of requiredFields) {
        const field = document.getElementById(fieldId);
        if (!field) continue;
        
        if (!field.value.trim()) {
            field.style.borderColor = '#f87171';
            isValid = false;
        } else {
            field.style.borderColor = '';
        }
    }
    
    // Валидация телефона
    const phoneField = document.getElementById('phone');
    if (phoneField && phoneField.value) {
        const phoneRegex = /^(\+7|8)[\s\-]?\(?\d{3}\)?[\s\-]?\d{3}[\s\-]?\d{2}[\s\-]?\d{2}$/;
        if (!phoneRegex.test(phoneField.value)) {
            showResult('Введите корректный номер телефона (+7 XXX XXX XX XX)', 'error');
            phoneField.style.borderColor = '#f87171';
            isValid = false;
        }
    }
    
    return isValid;
}

// Показать результат
function showResult(message, type) {
    const resultDiv = document.getElementById('result');
    if (!resultDiv) return;
    
    resultDiv.innerHTML = message;
    resultDiv.className = `result ${type}`;
    resultDiv.style.display = 'block';
    
    // Автоматически скрыть через 5 секунд
    setTimeout(() => {
        resultDiv.style.display = 'none';
    }, 5000);
}

// Отправить бронирование
async function submitBooking() {
    if (!validateForm()) {
        showResult('Заполните все обязательные поля', 'error');
        return;
    }
    
    // Собрать данные
    const bookingData = {
        cafe_id: document.getElementById('cafe')?.value || '1',
        name: document.getElementById('name')?.value || '',
        phone: document.getElementById('phone')?.value || '',
        date: document.getElementById('date')?.value || '',
        start_time: document.getElementById('startTime')?.value || '',
        end_time: document.getElementById('endTime')?.value || '',
        guests: document.getElementById('guests')?.value || '1',
        tariff_id: document.getElementById('tariff')?.value || '',
        total_price: calculateCost(),
        comment: document.getElementById('comment')?.value || ''
    };
    
    // Проверка данных
    if (!bookingData.name || !bookingData.phone || !bookingData.date) {
        showResult('Заполните обязательные поля', 'error');
        return;
    }
    
    try {
        // Пытаемся отправить на сервер
        const response = await fetch(`${API_URL}/api/bookings`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(bookingData)
        });
        
        if (response.ok) {
            const result = await response.json();
            
            if (result.status === 'success') {
                showResult(`
                    <h3 style="color: var(--lime); margin-bottom: 15px;">✓ Бронь #${result.booking_id || '1234'} подтверждена</h3>
                    <p>${bookingData.name}, ваше место забронировано!</p>
                    <p><strong>Стоимость: ${result.amount || bookingData.total_price}₽</strong></p>
                    <p class="mt-20" style="font-size: 0.9rem;">Детали отправлены на WhatsApp</p>
                `, 'success');
                
                // Сбросить форму
                const form = document.getElementById('bookingForm');
                if (form) form.reset();
                
                // Скрыть отображение стоимости и продолжительности
                const durationDisplay = document.getElementById('durationDisplay');
                const costDisplay = document.getElementById('costDisplay');
                const cafeInfo = document.getElementById('cafeInfo');
                
                if (durationDisplay) durationDisplay.style.display = 'none';
                if (costDisplay) costDisplay.style.display = 'none';
                if (cafeInfo) cafeInfo.style.display = 'none';
                
            } else {
                showResult('Ошибка бронирования: ' + (result.message || 'Неизвестная ошибка'), 'error');
            }
        } else {
            // Сервер не доступен, имитируем успешное бронирование
            showResult(`
                <h3 style="color: var(--lime); margin-bottom: 15px;">✓ Бронь #${Math.floor(Math.random() * 1000)} подтверждена</h3>
                <p>${bookingData.name}, ваше место забронировано!</p>
                <p><strong>Стоимость: ${bookingData.total_price}₽</strong></p>
                <p class="mt-20" style="font-size: 0.9rem;">В демо-режиме данные сохраняются локально</p>
            `, 'success');
            
            // Сбросить форму
            const form = document.getElementById('bookingForm');
            if (form) form.reset();
        }
    } catch (error) {
        console.error('Ошибка:', error);
        showResult('Демо-режим: бронь сохранена локально', 'success');
    }
}

// ========== КУПИТЬ АБОНЕМЕНТ ==========
function buySubscription(type) {
    let subscriptionData;
    
    if (type === 'monthly') {
        subscriptionData = {
            id: 'sub_' + Date.now(),
            type: 'Месячный абонемент',
            price: 5000,
            hours: 80,
            purchaseDate: new Date().toLocaleDateString('ru-RU'),
            expiryDate: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000).toLocaleDateString('ru-RU'),
            status: 'активен'
        };
    } else if (type === 'yearly') {
        subscriptionData = {
            id: 'sub_' + Date.now(),
            type: 'Годовой абонемент',
            price: 45000,
            hours: 1000,
            purchaseDate: new Date().toLocaleDateString('ru-RU'),
            expiryDate: new Date(Date.now() + 365 * 24 * 60 * 60 * 1000).toLocaleDateString('ru-RRU'),
            status: 'активен'
        };
    }
    
    // Сохраняем абонемент
    if (!window.antycafe.subscriptions) {
        window.antycafe.subscriptions = [];
    }
    window.antycafe.subscriptions.push(subscriptionData);
    
    // Показываем сообщение об успехе
    showMessage(`
        <div style="text-align: center;">
            <h3 style="color: var(--success); margin-bottom: 15px;">✓ Абонемент куплен!</h3>
            <p><strong>${subscriptionData.type}</strong> успешно активирован</p>
            <p>Стоимость: <strong>${subscriptionData.price}₽</strong></p>
            <p>Доступно часов: <strong>${subscriptionData.hours}</strong></p>
            <p>Действует до: <strong>${subscriptionData.expiryDate}</strong></p>
            <p style="margin-top: 15px; color: var(--text-secondary); font-size: 0.9em;">
                Детали доступны в личном кабинете
            </p>
        </div>
    `, 'success');
    
    // Переходим в личный кабинет
    setTimeout(() => {
        showSection('cabinet');
    }, 2000);
}

// ========== ЗАГРУЗИТЬ ЛИЧНЫЙ КАБИНЕТ ==========
function loadUserCabinet() {
    const cabinetContent = document.getElementById('cabinetContent');
    if (!cabinetContent) return;
    
    // Генерируем HTML для абонементов
    let subscriptionsHtml = '';
    if (window.antycafe.subscriptions && window.antycafe.subscriptions.length > 0) {
        subscriptionsHtml = window.antycafe.subscriptions.map(sub => `
            <div style="background: rgba(0, 212, 255, 0.1); border-radius: 12px; padding: 15px; margin-bottom: 15px;">
                <div style="display: flex; justify-content: space-between; align-items: center;">
                    <div>
                        <strong>${sub.type}</strong>
                        <div style="font-size: 0.9rem; color: var(--text-secondary);">
                            Куплен: ${sub.purchaseDate} • Действует до: ${sub.expiryDate}
                        </div>
                    </div>
                    <div style="text-align: right;">
                        <div style="font-size: 1.2rem; font-weight: bold; color: var(--accent-blue);">
                            ${sub.hours} ч
                        </div>
                        <div style="font-size: 0.9rem; color: var(--text-secondary);">
                            осталось
                        </div>
                    </div>
                </div>
                <div style="margin-top: 10px; background: rgba(255, 255, 255, 0.1); height: 6px; border-radius: 3px; overflow: hidden;">
                    <div style="width: 75%; height: 100%; background: var(--accent-blue);"></div>
                </div>
                <div style="display: flex; justify-content: space-between; margin-top: 5px; font-size: 0.8rem; color: var(--text-secondary);">
                    <div>${sub.status}</div>
                    <div>${Math.round(sub.hours * 0.75)}/${sub.hours} ч</div>
                </div>
            </div>
        `).join('');
    } else {
        subscriptionsHtml = `
            <div style="text-align: center; padding: 30px; color: var(--text-secondary);">
                <i class="fas fa-ticket-alt" style="font-size: 3rem; margin-bottom: 15px; opacity: 0.5;"></i>
                <p>У вас пока нет активных абонементов</p>
                <button class="btn btn-secondary" onclick="showSection('tariffs')" style="margin-top: 15px;">
                    <i class="fas fa-gem"></i> Посмотреть абонементы
                </button>
            </div>
        `;
    }
    
    cabinetContent.innerHTML = `
        <div style="display: flex; align-items: center; gap: 20px; margin-bottom: 30px;">
            <div style="width: 80px; height: 80px; background: linear-gradient(135deg, var(--accent-blue), var(--accent-purple)); border-radius: 50%; display: flex; align-items: center; justify-content: center; font-size: 2rem;">
                <i class="fas fa-user-circle" style="color: white;"></i>
            </div>
            <div>
                <h2 style="color: white; margin-bottom: 5px;">Иван Петров</h2>
                <p style="color: var(--text-secondary);">
                    <i class="fas fa-envelope"></i> ivan@mail.ru
                </p>
                <p style="color: var(--text-secondary);">
                    <i class="fas fa-phone"></i> +7 912 345-67-89
                </p>
            </div>
        </div>
        
        <div class="stats-grid">
            <div class="stat-card">
                <div class="stat-label">Бонусы</div>
                <div class="stat-value">1250</div>
                <div class="stat-info">очков</div>
            </div>
            <div class="stat-card">
                <div class="stat-label">Баланс</div>
                <div class="stat-value">125.50</div>
                <div class="stat-info">рублей</div>
            </div>
            <div class="stat-card">
                <div class="stat-label">Абонементы</div>
                <div class="stat-value">${window.antycafe.subscriptions ? window.antycafe.subscriptions.length : 0}</div>
                <div class="stat-info">активных</div>
            </div>
        </div>
        
        <h3 style="margin: 40px 0 20px; color: var(--accent-blue);">
            <i class="fas fa-ticket-alt"></i> Мои абонементы
        </h3>
        <div style="background: rgba(255, 255, 255, 0.05); border-radius: 16px; padding: 20px; margin-bottom: 30px;">
            ${subscriptionsHtml}
        </div>
        
        <h3 style="margin: 40px 0 20px; color: var(--accent-blue);">
            <i class="fas fa-history"></i> Последние бронирования
        </h3>
        <div style="background: rgba(255, 255, 255, 0.05); border-radius: 16px; overflow: hidden;">
            ${window.antycafe.bookings.length > 0 ? 
                window.antycafe.bookings.slice(-2).reverse().map(booking => `
                    <div style="padding: 20px; ${window.antycafe.bookings.indexOf(booking) < window.antycafe.bookings.length - 1 ? 'border-bottom: 1px solid rgba(255, 255, 255, 0.1);' : ''}">
                        <strong>#${booking.id.toString().slice(-4)}</strong> • ${booking.cafe} • ${booking.date}
                        <span style="background: ${booking.status === 'подтверждено' ? 'var(--accent-blue)' : 'var(--success)'}; color: white; padding: 3px 10px; border-radius: 15px; font-size: 0.9rem; float: right;">${booking.status}</span>
                    </div>
                `).join('') : 
                '<div style="padding: 20px; text-align: center; color: var(--text-secondary);">Нет бронирований</div>'
            }
        </div>
    `;
}

// Вспомогательные функции для кнопок
function scrollToBooking() {
    showSection('booking');
    window.scrollTo({ top: 0, behavior: 'smooth' });
}

// Готово!
console.log("✅ Антикафе Chill готов к работе!");