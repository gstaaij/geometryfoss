#!/bin/env python3

with open("objdefsutil.h", "w") as file:
    file.writelines([
        "\n",
        "#define PAD(n) __PAD##n##__\n",
        "\n",
        "// Stupid stuff\n"
        "#define __PAD1__ {0}\n"
    ])
    for i in range(2, 101):
        file.write("#define __PAD" + str(i) + "__ {0}, __PAD" + str(i - 1) + "__\n")