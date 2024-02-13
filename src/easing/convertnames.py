#!/usr/bin/env python3

import sys

def main() -> int:
    if (len(sys.argv) < 2):
        print("Usage: %s {enum|generator}" % sys.argv[0])
        return 1
    
    names = ["EaseInSine", "EaseOutSine", "EaseInOutSine", "EaseInQuad", "EaseOutQuad", "EaseInOutQuad", "EaseInCubic", "EaseOutCubic", "EaseInOutCubic", "EaseInQuart", "EaseOutQuart", "EaseInOutQuart", "EaseInQuint", "EaseOutQuint", "EaseInOutQuint", "EaseInExpo", "EaseOutExpo", "EaseInOutExpo", "EaseInCirc", "EaseOutCirc", "EaseInOutCirc", "EaseInBack", "EaseOutBack", "EaseInOutBack", "EaseInElastic", "EaseOutElastic", "EaseInOutElastic", "EaseInBounce", "EaseOutBounce", "EaseInOutBounce"]
    functionNames = []
    enumNames = []

    for name in names:
        functionName = ""
        enumName = ""
        for i in range(len(name)):
            char = name[i]

            if (i == 0):
                functionName += char.lower()
            else:
                functionName += char
            
            if (i != 0 and char.isupper()):
                enumName += "_"
            enumName += char.upper()
        functionNames.append(functionName)
        enumNames.append(enumName)
    
    if (sys.argv[1] == "enum"):
        print("typedef enum {")
        for name in enumNames:
            print(f"    {name},")
        print("} easingFunctionEnum;")
    elif (sys.argv[1] == "generator"):
        print("easingFunction getEasingFunction(easingFunctionEnum function) {")
        print("    switch (function) {")
        for i in range(len(names)):
            print(f"        case {enumNames[i]}: return {functionNames[i]};")
        print("    }")
        print("}")
    else:
        print("Incorrect option")
        print("Usage: %s {enum|generator}" % sys.argv[0])


if __name__ == "__main__":
    sys.exit(main())