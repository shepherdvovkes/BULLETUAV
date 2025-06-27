#!/usr/bin/env fish

#
# GitHub Repo Initializer for Fish Shell on macOS
#
# Этот скрипт автоматизирует настройку нового SSH-ключа для конкретного
# репозитория GitHub, добавляет конфигурацию в ~/.ssh/config и клонирует репозиторий.
# Он поддерживает URL как в формате HTTPS, так и в формате SSH.
#
# Использование:
# 1. Сохраните этот скрипт в файл, например, `gh-init.fish`.
# 2. Сделайте его исполняемым: `chmod +x gh-init.fish`.
# 3. Запустите его, передав URL репозитория в качестве аргумента:
#    ./gh-init.fish https://github.com/your-username/your-repo
#    ИЛИ
#    ./gh-init.fish git@github.com:your-username/your-repo.git
#

# --- Функции для цветного вывода ---
function echo_info
    set_color blue
    echo $argv
    set_color normal
end

function echo_success
    set_color green
    echo $argv
    set_color normal
end

function echo_warning
    set_color yellow
    echo $argv
    set_color normal
end

function echo_error
    set_color red
    echo $argv
    set_color normal
    exit 1
end
# ------------------------------------


# --- 1. Проверка аргументов ---
if test (count $argv) -ne 1
    echo_error "Ошибка: Не указан URL репозитория."
    echo_error "Пример использования: ./(basename (status --current-filename)) https://github.com/user/repo"
end

set original_repo_url $argv[1]
set repo_url ""
set repo_user_and_name ""

# --- 2. Валидация и парсинг URL ---
# Проверяем, это HTTPS URL?
if string match -q "https://github.com/*" -- $original_repo_url
    echo_info "Обнаружен HTTPS URL. Конвертируем в SSH формат."
    # Извлекаем 'user/repo', удаляя префикс и опциональный суффикс .git
    set repo_user_and_name (string replace -r '.git$' '' (string replace 'https://github.com/' '' $original_repo_url))
    # Создаем канонический SSH URL для дальнейшей работы
    set repo_url "git@github.com:$repo_user_and_name.git"
    echo_info "Сконвертированный SSH URL: $repo_url"

# Проверяем, это SSH URL?
else if string match -q "git@github.com:*" -- $original_repo_url
    echo_info "Обнаружен SSH URL."
    set repo_url $original_repo_url
    # Извлекаем 'user/repo', удаляя префикс и суффикс .git
    set repo_user_and_name (string replace -r '.git$' '' (string replace 'git@github.com:' '' $repo_url))

# Если ни один формат не подошел
else
    echo_error "Неверный формат URL репозитория."
    echo_error "Поддерживаются форматы: 'https://github.com/user/repo' и 'git@github.com:user/repo.git'"
end

# Извлечение имени репозитория для дальнейшего использования
set repo_name (basename $repo_user_and_name)

# Создание уникального хоста для SSH-конфигурации
set ssh_host "github.com-$repo_name"

echo_info "Начинаем настройку для репозитория: $repo_user_and_name"

# Путь для нового SSH-ключа
set ssh_key_path "$HOME/.ssh/id_ed25519_$repo_name"

# Проверка, существует ли уже ключ для этого репозитория
if test -f "$ssh_key_path"
    echo_warning "SSH-ключ для этого репозитория уже существует: $ssh_key_path."
    read -P "Хотите перезаписать его? (y/N): " confirm
    if test "$confirm" != "y" -a "$confirm" != "Y"
        echo_info "Скрипт прерван."
        exit 0
    end
end

# --- 3. Генерация нового SSH-ключа ---
echo_info "Генерация нового SSH-ключа..."
# -t ed25519: современный и безопасный тип ключа
# -f: путь к файлу ключа
# -N "": пустая парольная фраза для автоматизации
# -C: комментарий для идентификации ключа (например, на GitHub)
ssh-keygen -t ed25519 -f "$ssh_key_path" -N "" -C "$ssh_host"
if test $status -ne 0
    echo_error "Не удалось сгенерировать SSH-ключ."
end
echo_success "SSH-ключ успешно создан: $ssh_key_path"

# --- 4. Отображение публичного ключа и ожидание ---
echo_info "------------------------------------------------------------------"
echo_warning "Пожалуйста, скопируйте следующий публичный ключ."
echo_warning "Затем добавьте его в ваш репозиторий на GitHub как 'Deploy Key'."
echo_warning "Путь: Настройки репозитория -> Deploy Keys -> Add deploy key"
echo_info "------------------------------------------------------------------"
set_color cyan
cat "$ssh_key_path.pub"
set_color normal
echo_info "------------------------------------------------------------------"

read -P "После добавления ключа на GitHub, нажмите Enter для продолжения..."

# --- 5. Создание/обновление файла ~/.ssh/config ---
set ssh_config_path "$HOME/.ssh/config"
set ssh_dir (dirname $ssh_config_path)

# Убедимся, что директория ~/.ssh существует и имеет правильные права
if not test -d $ssh_dir
    mkdir -p $ssh_dir
    chmod 700 $ssh_dir
    echo_info "Создана директория: $ssh_dir"
end

# Убедимся, что файл config существует
if not test -f $ssh_config_path
    touch $ssh_config_path
    chmod 600 $ssh_config_path
    echo_info "Создан файл конфигурации: $ssh_config_path"
end

# Проверка, существует ли уже такая конфигурация хоста
if grep -q "Host $ssh_host" "$ssh_config_path"
    echo_warning "Хост '$ssh_host' уже существует в $ssh_config_path. Пропускаем добавление конфигурации."
else
    echo_info "Добавление конфигурации в $ssh_config_path..."
    # Блок конфигурации
    set config_block "\n# Конфигурация для репозитория $repo_user_and_name\nHost $ssh_host\n  HostName github.com\n  User git\n  IdentityFile $ssh_key_path\n  IdentitiesOnly yes\n"

    # Добавляем конфигурацию в конец файла
    echo -e $config_block >> $ssh_config_path
    echo_success "Конфигурация SSH успешно добавлена."
end

# --- 6. Клонирование репозитория ---
# Заменяем 'github.com' на наш специальный хост из SSH-конфига
set clone_url (string replace "github.com" "$ssh_host" $repo_url)
echo_info "Клонирование репозитория с использованием новой SSH-конфигурации..."
echo_info "Выполняется команда: git clone $clone_url"

git clone $clone_url

# Проверка статуса выполнения последней команды
if test $status -eq 0
    echo_success "Репозиторий успешно клонирован в папку './$repo_name'."
else
    echo_error "Не удалось клонировать репозиторий. Пожалуйста, проверьте:"
    echo_error "1. Правильно ли вы добавили Deploy Key в репозиторий на GitHub?"
    echo_error "2. Имеет ли ключ права на запись (если они необходимы)?"
    echo_error "3. Корректен ли URL репозитория '$original_repo_url'?"
end
