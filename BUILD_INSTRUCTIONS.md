# BULLET UAV - Инструкции по сборке с OpenCV

## Обзор

Проект BULLET UAV теперь включает интеграцию с OpenCV для компьютерного зрения и оптического трекинга целей.

## Зависимости

### Обязательные зависимости
- **CMake** (версия 3.16 или выше)
- **C++17** компилятор
- **OpenCV** (версия 4.x)

### Установка зависимостей

#### macOS (Homebrew)
```bash
# Установка OpenCV
brew install opencv

# Установка CMake (если не установлен)
brew install cmake
```

#### Ubuntu/Debian
```bash
# Установка зависимостей
sudo apt update
sudo apt install -y cmake build-essential git
sudo apt install -y libopencv-dev libeigen3-dev
```

## Сборка проекта

### 1. Клонирование репозитория
```bash
git clone <repository-url>
cd BULLETUAV
```

### 2. Создание директории сборки
```bash
mkdir build
cd build
```

### 3. Конфигурация CMake
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### 4. Сборка
```bash
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
```

## Структура сборки

После успешной сборки в директории `build/` будут созданы:

- `test_opencv` - простой тест OpenCV
- `example_opencv` - пример использования OpenCV в проекте
- `src/targeting/libtargeting_core.a` - библиотека системы наведения
- `src/sensors/liboptical_tracker.a` - библиотека оптического трекинга

## Тестирование

### Базовый тест OpenCV
```bash
./test_opencv
```

### Полный пример интеграции
```bash
./example_opencv
```

Этот пример демонстрирует:
- Инициализацию камеры
- Трекинг целей
- Создание и сохранение изображений
- Интеграцию с системой наведения

## Компоненты с OpenCV

### 1. Optical Tracker (`src/sensors/optical/`)
- **optical_tracker.h** - заголовочный файл
- **optical_tracker.cpp** - реализация
- Функциональность:
  - Инициализация камеры
  - Трекинг целей с помощью OpenCV
  - Расчет скорости движения целей
  - Восстановление потерянных целей

### 2. Targeting Core (`src/targeting/core/`)
- Интегрирован с OpenCV для обработки данных от оптических сенсоров
- Поддерживает различные типы целей (Shahed дроны, вертолеты, наземные цели)

## Использование в коде

### Подключение OpenCV
```cpp
#include <opencv2/opencv.hpp>
#include "src/sensors/optical/optical_tracker.h"
```

### Создание трекера
```cpp
bullet::sensors::OpticalTracker tracker;
tracker.initializeCamera();
```

### Трекинг целей
```cpp
bullet::targeting::Target target;
if (tracker.trackTarget(target)) {
    // Цель обнаружена и отслеживается
    std::cout << "Target at: " << target.lat << ", " << target.lon << std::endl;
}
```

## Устранение неполадок

### Ошибка "Could not find OpenCV"
Убедитесь, что OpenCV установлен:
```bash
# macOS
brew list opencv

# Ubuntu
dpkg -l | grep libopencv
```

### Ошибки компиляции
1. Проверьте версию CMake: `cmake --version`
2. Убедитесь, что используется C++17: `g++ --version`
3. Проверьте, что OpenCV найден: `pkg-config --modversion opencv4`

### Проблемы с камерой
- На macOS может потребоваться разрешение на доступ к камере
- На Linux убедитесь, что пользователь в группе `video`

## Дополнительные возможности

### Включение дополнительных модулей
```bash
cmake .. -DENABLE_GUIDANCE=ON -DENABLE_ENGINE=ON
```

### Сборка документации
```bash
make doc
```

## Лицензия

Проект использует военную лицензию v2.0. См. файл LICENSE для подробностей. 