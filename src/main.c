#include "manifest.h"
#include "utils.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef VERSION
#define VERSION "1.0"
#endif

#ifndef GIT_HASH
#define GIT_HASH "unknown"
#endif

void print_help() {
    printf("Usage: mangen [DIR_PATH] [OPTIONS]\n"
           "Generate a manifest of files with SHA-256 checksums.\n\n"
           "Options:\n"
           "  -h          Show this help message\n"
           "  -v          Show version and Git commit hash\n"
           "  -e PATTERN  Exclude files matching PATTERN (wildcards: * and ?)\n"
           "  -c          Generate manifest integrity checksum\n");
}


void generate_manifest_with_checksum(const char* base_dir, const char* exclude) {
    FILE *original_stdout = stdout;

    // Перенаправляем вывод в файл
    FILE *tmp_file = fopen("manifest.tmp", "w");
    if (!tmp_file) {
        fprintf(stderr, "Ошибка: не удалось создать manifest.tmp\n");
        exit(EXIT_FAILURE);
    }
    stdout = tmp_file; // Переназначаем stdout

    generate_manifest(base_dir, exclude);
    fclose(tmp_file); // Закрываем файл

    // Восстанавливаем stdout
    stdout = original_stdout;

    // Вычисляем хэш
    char checksum[65];
    compute_file_sha256("manifest.tmp", checksum);

    // Выводим контрольную сумму
    printf("Контрольная сумма: %s\n", checksum);

    // Удаляем временный файл
    remove("manifest.tmp");
}

int main(int argc, char* argv[]) {
    char* dir = ".";
    char* exclude = NULL;
    int checksum_flag = 0;
    int version_flag = 0;
    int help_flag = 0;

    // Обработка аргументов
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            help_flag = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            version_flag = 1;
        } else if (strcmp(argv[i], "-c") == 0) {
            checksum_flag = 1;
        } else if (strcmp(argv[i], "-e") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -e requires a pattern argument\n");
                exit(EXIT_FAILURE);
            }
            exclude = argv[++i];
        } else if (argv[i][0] != '-') {
            dir = argv[i];
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    // Обработка флагов
    if (help_flag) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    if (version_flag) {
        printf("mangen %s (Git commit: %s)\n", VERSION, GIT_HASH);
        exit(EXIT_SUCCESS);
    }

    // Проверка существования каталога
    if (access(dir, F_OK) != 0) {
        fprintf(stderr, "Error: Directory '%s' does not exist\n", dir);
        exit(EXIT_FAILURE);
    }

    // Генерация манифеста
    if (checksum_flag) {
        generate_manifest_with_checksum(dir, exclude);
    } else {
        generate_manifest(dir, exclude);
    }

    return EXIT_SUCCESS;
}