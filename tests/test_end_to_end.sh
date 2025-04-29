#!/bin/bash
set -e

# Создание тестового каталога
mkdir -p test_dir/{dir1,dir2}
echo "test" > test_dir/file1.txt
echo "data" > test_dir/dir1/file2.log

# Проверка базовой работы
./mangen test_dir | grep "file1.txt : " > /dev/null

# Проверка исключения файлов
./mangen test_dir -e "*.log" | grep -v "file2.log" > /dev/null

# Проверка версии
./mangen -v | grep -q "git:"

echo "All tests passed!"