#ifndef LSBI_H
#define LSBI_H

#include <stdint.h>
#include <stdlib.h>

typedef enum {
        LSBI_Pattern_00,
        LSBI_Pattern_01,
        LSBI_Pattern_10,
        LSBI_Pattern_11,
        N_LSBI_Pattern
} LSBI_PatternMap;

#define LSBI_BYTE_PATTERN_MASK 0x06 // Bytes 2 and 3 = 0b00000110

void lsbi_apply_pattern_map_to_payload(uint8_t *payload, size_t size,
                                       size_t pattern_count[N_LSBI_Pattern],
                                       size_t pattern_changes[N_LSBI_Pattern]);
void lsbi_calculate_patterns(uint8_t *org, uint8_t *payload, size_t size,
                             size_t count[N_LSBI_Pattern], size_t changes[N_LSBI_Pattern]);

#endif /* LSBI_H */
