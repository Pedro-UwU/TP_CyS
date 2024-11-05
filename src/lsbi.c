// embed.c (versión completa)
#ifndef LSBI
#define LSBI

#include <def.h>
#include <lsbi.h>

static void lsbi_flip_last_bit(uint8_t *payload, size_t size, size_t pattern_index);

void lsbi_apply_pattern_map_to_payload(uint8_t *payload, size_t size,
                                       size_t pattern_count[N_LSBI_Pattern],
                                       size_t pattern_changes[N_LSBI_Pattern])
{
        /* PAPER
         *
         * Finally, we inverse the last bit of the stego-
         * image, if the number of pixels that have 
         * changed in specific patterns are greater than
         * the number of pixels that are not changed.
         */

        for (size_t i = 0; i < BYTE * N_LSBI_Pattern; i++) {
                if (pattern_changes[i] > pattern_count[i] - pattern_changes[i]) {
                        lsbi_flip_last_bit(payload, size, i);
                        payload[i] |= 0x01;
                } else {
                        payload[i] &= 0xFF ^ 0x01;
                }
        }
}

void lsbi_calculate_patterns(uint8_t *org, uint8_t *payload, size_t size,
                             size_t count[N_LSBI_Pattern], size_t changes[N_LSBI_Pattern])
{
        for (size_t i = N_LSBI_Pattern; i < size; i++) {
                uint32_t p = (org[i] & LSBI_BYTE_PATTERN_MASK) >> 1;
                count[p]++;

                if (org[i] != payload[i]) {
                        changes[p]++;
                }
        }
}

static void lsbi_flip_last_bit(uint8_t *payload, size_t size, size_t pattern_index)
{
        for (size_t j = N_LSBI_Pattern; j < size; j++) {
                uint32_t p = (payload[j] & LSBI_BYTE_PATTERN_MASK) >> 1;

                if ((j + 1) % 3 == 0 || p != pattern_index) {
                        continue;
                }
                // Invert last bit
                payload[j] ^= 0x01;
        }
}

#endif /* LSBI */
