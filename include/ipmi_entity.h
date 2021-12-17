#ifndef IPMI_ENTITY_H
#define IPMI_ENTITY_H

#include "ipmi.h"

struct entity_id {
    uint8_t id;           /* physical entity id */
#if WORDS_BIGENDIAN
    uint8_t logical:1;    /* physical/logical */
    uint8_t instance:7;   /* instance number */
#else
    uint8_t instance:7;   /* instance number */
    uint8_t logical:1;    /* physical/logical */
#endif
} ATTRIBUTE_PACKING;

#endif /* IPMI_ENTITY_H */
