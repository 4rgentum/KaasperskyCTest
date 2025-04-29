#include "../include/manifest.h"
#include "../include/hash.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

static int matches_pattern(const char* filename, const char* pattern) {
    // Реализация сопоставления с шаблоном (.* замены)
    // Упрощённая версия для примера
    return strstr(filename, pattern) != NULL;
}

static void process_entry(const char* base_path, const char* entry_name, const char* exclude) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry_name);

    struct stat st;
    if (lstat(full_path, &st) == -1) {
        perror("stat error");
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        DIR* dir = opendir(full_path);
        if (!dir) {
            perror("Failed to open directory");
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            process_entry(full_path, entry->d_name, exclude);
        }
        closedir(dir);
    } else {
        if (exclude && matches_pattern(entry_name, exclude)) return;
        
        char hash[65];
        compute_file_sha256(full_path, hash);
        printf("%s : %s\n", full_path + strlen(base_path) + 1, hash);
    }
}

void generate_manifest(const char* base_dir, const char* exclude_pattern) {
    process_entry(base_dir, "", exclude_pattern);
}