#!/bin/bash

MAX_SIZE=$((100 * 1024)) # Максимальный размер контекста в байтах
CONSTANTS_FILE_MINSIZE=$((10 * 1024))
ROOT_DIR=$1
INPUT_FILE=$2

# Функция для проверки размера файла
check_size() {
    local file=$1
    local size=$(stat -c%s "$file")
    if [ $size -gt $2 ]; then
        return 1
    fi
    return 0
}

find_and_read_file() {
    # Проверяем, передан ли аргумент (название файла)
    if [ -z "$1" ]; then
        echo "Ошибка: Не указано имя файла." >&2
        return 1
    fi

    # Поиск файла в текущем каталоге и его подкаталогах
    file_path=$(find $ROOT_DIR -name "$1" -exec echo \{\} \; -quit)
    if [ -z "$file_path" ]; then
        return 1
    fi

    if (! check_size "$file_path" $CONSTANTS_FILE_MINSIZE ); then
        echo "Skipping large file (constants?): $file" >&2
        return 1
    fi

    # Вывод содержимого файла
    cat "$file_path"
}

# Создаем ассоциативный массив для сета значений
declare -A fileSet

# Функция для добавления значения в сет
add_to_set() {
    local value=$1
    if [[ -z "${fileSet[$value]}" ]]; then
        fileSet[$value]=1
    fi
}

# Функция для проверки наличия значения в сете
check_value() {
    local value=$1
    if [[ -n "${fileSet[$value]}" ]]; then
        return 1
    else
        return 0
    fi
}

# Проверка корректности параметров
if (! test -e "$ROOT_DIR"); then
    echo "Ошибка: директория не существует: $ROOT_DIR"
    exit 1
fi
if (! test -d "$ROOT_DIR"); then
    echo "Ошибка: не директория: $ROOT_DIR"
    exit 1
fi

# Проверка наличия входного файла
if [ -z "$INPUT_FILE" ] || [ ! -f "$INPUT_FILE" ]; then
    echo "Ошибка: указанный файл не найден или не является файлом." >&2
    exit 1
fi

# Чтение входного файла
CONTENT=$(cat $INPUT_FILE)

# Поиск всех связанных файлов
RELATED_FILES=$(grep -rE '^\s*#\s*include\s*[<"]' "$ROOT_DIR" | awk -F'"' '{print $2}' | sort | uniq)

# Создание файла контекста
OFILE_PATH="context.txt"
truncate --size=0 $OFILE_PATH
for file in $RELATED_FILES; do
    dir=$(dirname "$file")

    # Check if the directory contains 'antlr'
    if [[ "$dir" == *"antlr"* || "$dir" == *"common"* || "$dir" == *"runtime"* ]]; then
        echo "Skipping useless file: $file" >&2
        continue
    fi

    if (! check_value "$file" ); then
        echo "File ignored (already exist): $file" >&2
        continue;
    fi

    echo -e "\n// FILE: $file\n" >> $OFILE_PATH
    if (test -e "$file"); then
        if (! check_size "$file" $CONSTANTS_FILE_MINSIZE ); then
            echo "Skipping large file (constants?): $file" >&2
            continue
        fi
        cat "$ROOT_DIR/$file" >> $OFILE_PATH
    else
        find_and_read_file "$(basename "$file")" >> $OFILE_PATH
    fi
done

echo -e "// CONTEXT FILE: $(realpath $OFILE_PATH)\n" >> $OFILE_PATH
echo "$CONTENT" >> $OFILE_PATH

# Проверка размера файла контекста
if (! check_size "$OFILE_PATH" $MAX_SIZE ); then
    echo "Файл контекста превышает максимальный размер (макс размер: $((MAX_SIZE / 1024)) Кб)." >&2
fi
