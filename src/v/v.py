vectors = [
        "VkLayerProperties",
        "VkPhysicalDevice",
        "VkQueueFamilyProperties",
        "VkDeviceQueueCreateInfo",
        "VkExtensionProperties",
        ]

for vector in vectors:
    print(f"Create vector file for : {vector}")

    header = f'#ifndef V_V{vector}_HEADER\n'
    header += '#include "v.h"\n'
    header += f'V_INCL({vector})\n'
    header += f'#define V_V{vector}_HEADER\n'
    header += f'#endif\n'

    source = f'#include "V{vector}.h"\n';
    source += f'V_IMPL({vector})\n'

    #print(f"{header}>>>V{vector}.h");
    with open(f"V{vector}.h", "w") as f:
        f.write(header)

    #print(f"{source}>>>V{vector}.c");
    with open(f"V{vector}.c", "w") as f:
        f.write(source)

print("\n=== build.ninja build ===")
for vector in vectors:
    print(f"build $bdir/v/V{vector}.o: cc $root/v/V{vector}.c")

print("\n=== build.ninja link ===")
for vector in vectors:
    print(f"    $bdir/v/V{vector}.o $")

