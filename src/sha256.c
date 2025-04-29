#include "sha256.h"
#include <string.h>

// Начальные значения хэша (первые 32 бита дробных частей квадратных корней первых 8 простых чисел)
static const uint32_t INITIAL_STATE[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

// Константы раундов (первые 32 бита дробных частей кубических корней первых 64 простых чисел)
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Циклический сдвиг вправо
static inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

// Основные логические функции
static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

static inline uint32_t sigma0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static inline uint32_t sigma1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

static inline uint32_t gamma0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static inline uint32_t gamma1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

// Обработка одного блока данных (64 байта)
static void sha256_transform(sha256_ctx *ctx) {
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t w[64];
    uint8_t *block = ctx->buffer;

    // Преобразование блока в массив 32-битных слов (big-endian)
    for (int i = 0; i < 16; i++) {
        w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }

    // Расширение сообщения до 64 слов
    for (int i = 16; i < 64; i++) {
        w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
    }

    // Инициализация рабочих переменных
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    // 64 раунда преобразований
    for (int i = 0; i < 64; i++) {
        uint32_t t1 = h + sigma1(e) + ch(e, f, g) + K[i] + w[i];
        uint32_t t2 = sigma0(a) + maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // Обновление состояния
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// Инициализация контекста
void sha256_init(sha256_ctx *ctx) {
    memcpy(ctx->state, INITIAL_STATE, sizeof(INITIAL_STATE));
    ctx->bit_count = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

// Добавление данных
void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len) {
    uint64_t bit_len = len * 8;
    uint32_t buffer_pos = ctx->bit_count % 512 / 8;

    // Обновление счетчика битов
    ctx->bit_count += bit_len;

    // Обработка данных по блокам
    while (len > 0) {
        uint32_t copy_len = 64 - buffer_pos;
        if (copy_len > len) copy_len = len;

        memcpy(ctx->buffer + buffer_pos, data, copy_len);
        buffer_pos += copy_len;
        data += copy_len;
        len -= copy_len;

        if (buffer_pos == 64) {
            sha256_transform(ctx);
            buffer_pos = 0;
        }
    }
}

// Завершение вычисления
void sha256_final(sha256_ctx *ctx, uint8_t hash[32]) {
    uint32_t buffer_pos = ctx->bit_count % 512 / 8;
    ctx->buffer[buffer_pos++] = 0x80; // Добавление бита '1'

    // Заполнение нулями до конца блока
    if (buffer_pos > 56) {
        memset(ctx->buffer + buffer_pos, 0, 64 - buffer_pos);
        sha256_transform(ctx);
        buffer_pos = 0;
    }

    memset(ctx->buffer + buffer_pos, 0, 56 - buffer_pos);

    // Добавление длины сообщения (в битах, big-endian)
    uint64_t bit_count = ctx->bit_count;
    for (int i = 0; i < 8; i++) {
        ctx->buffer[56 + i] = (bit_count >> (56 - i * 8)) & 0xFF;
    }

    sha256_transform(ctx);

    // Сохранение хэша в big-endian
    for (int i = 0; i < 8; i++) {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}

// Вспомогательная функция для вычисления хэша
void sha256(const uint8_t *data, size_t len, uint8_t hash[32]) {
    sha256_ctx ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, hash);
}

// Преобразование в HEX-строку
void sha256_to_hex(const uint8_t hash[32], char hex[65]) {
    const char *hex_chars = "0123456789abcdef";
    for (int i = 0; i < 32; i++) {
        hex[i * 2] = hex_chars[hash[i] >> 4];
        hex[i * 2 + 1] = hex_chars[hash[i] & 0x0F];
    }
    hex[64] = '\0';
}