#ifndef LSBI_H
#define LSBI_H

typedef enum {
        LSBI_Pattern_00,
        LSBI_Pattern_01,
        LSBI_Pattern_10,
        LSBI_Pattern_11,
        N_LSBI_Pattern
} LSBI_PatternMap;

#define LSBI_BYTE_PATTERN_MASK 0x06 // Bytes 2 and 3 = 0b00000110

#endif /* LSBI_H */
