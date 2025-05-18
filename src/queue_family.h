#ifndef QUEUE_FAMILY_H

#include "optional.h"

typedef struct QueueFamilyIndices {
    OptionalU32 graphics_family;
    OptionalU32 present_family;
} QueueFamilyIndices;

void queue_family_indices_clear(QueueFamilyIndices *indices);
bool queue_family_indices_is_complete(QueueFamilyIndices *indices);

#define QUEUE_FAMILY_H
#endif

