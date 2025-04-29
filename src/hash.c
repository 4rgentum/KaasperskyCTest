#include "hash.h"
#include "sha256.h"
#include <stdlib.h>
#include <stdio.h>

void compute_file_sha256(const char* path, char hex_hash[65]) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", path);
        hex_hash[0] = '\0';
        return;
    }

    sha256_ctx ctx;
    sha256_init(&ctx);
    
    uint8_t buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        sha256_update(&ctx, buffer, bytes_read);
    }
    
    if (ferror(file)) {
        fprintf(stderr, "Error reading file: %s\n", path);
        fclose(file);
        hex_hash[0] = '\0';
        return;
    }
    
    uint8_t hash[32];
    sha256_final(&ctx, hash);
    sha256_to_hex(hash, hex_hash);
    
    fclose(file);
}