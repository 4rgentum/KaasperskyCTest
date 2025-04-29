#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

// Контекст для вычисления хэша
typedef struct {
    uint32_t state[8];      // Текущее состояние хэша (A, B, C, D, E, F, G, H)
    uint64_t bit_count;     // Общее количество обработанных бит
    uint8_t buffer[64];     // Буфер для накопления блока данных (64 байта)
} sha256_ctx;

// Инициализация контекста
void sha256_init(sha256_ctx *ctx);

// Добавление данных в хэш
void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len);

// Завершение вычисления и получение хэша
void sha256_final(sha256_ctx *ctx, uint8_t hash[32]);

// Вспомогательная функция для вычисления хэша строки
void sha256(const uint8_t *data, size_t len, uint8_t hash[32]);

// Преобразование хэша в HEX-строку
void sha256_to_hex(const uint8_t hash[32], char hex[65]);

#endif // SHA256_H