#pragma once

#include "quarkit_glue.h"
typedef struct {
    uint8_t* data;
    size_t size;
    int is_allocated;
} QuarkitPayloadContext;

extern QuarkitPayloadContext g_quarkit_payload;