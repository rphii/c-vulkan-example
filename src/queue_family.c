#include <string.h>
#include "queue_family.h"

void queue_family_indices_clear(QueueFamilyIndices *indices) {
    assert_arg(indices);
    memset(indices, 0, sizeof(*indices));
}

bool queue_family_indices_is_complete(QueueFamilyIndices indices) {
    return indices.graphics_family.has_value;
}



