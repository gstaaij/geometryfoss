// Adapted from https://github.com/nicolausYes/easing-functions (no license)
// Some of the code has undefined behaviour and I don't understand why some of these are the way they are,
// so I have replaced some of the functions with ones from https://easings.net

#include "easing.h"
#include <stddef.h>
#include <math.h>
#include "util.h"

#ifndef PI
#define PI 3.1415926545
#endif

double easeInSine(double t) {
    return sin(1.5707963 * t);
}

double easeOutSine(double t) {
    return 1 + sin(1.5707963 * (--t));
}

double easeInOutSine(double t) {
    return 0.5 * (1 + sin(PI * (t - 0.5)));
}

double easeInQuad(double t) {
    return t * t;
}

double easeOutQuad(double t) { 
    return t * (2 - t);
}

double easeInOutQuad(double t) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

double easeInCubic(double t) {
    return t * t * t;
}

double easeOutCubic(double t) {
    double invt = 1 - t;
    return 1 - invt * invt * invt;
    // This code is bad (-Wsequence-point), so I replaced it with something that works
    // return 1 + (--t) * t * t;
}

double easeInOutCubic(double t) {
    double powt = -2 * t + 2;
    return t < 0.5 ? 4 * t * t * t : 1 - (powt * powt * powt) / 2;
    // return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
}

double easeInQuart(double t) {
    t *= t;
    return t * t;
}

double easeOutQuart(double t) {
    t = 1 - t;
    return 1 - t * t * t * t;
    // t = (--t) * t;
    // return 1 - t * t;
}

double easeInOutQuart(double t) {
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    } else {
        t = -2 * t + 2;
        return 1 - (t * t * t * t) / 2;
        // t = (--t) * t;
        // return 1 - 8 * t * t;
    }
}

double easeInQuint(double t) {
    double t2 = t * t;
    return t * t2 * t2;
}

double easeOutQuint(double t) {
    t = 1 - t;
    double t2 = t * t;
    return 1 - t * t2 * t2;
    // double t2 = (--t) * t;
    // return 1 + t * t2 * t2;
}

double easeInOutQuint(double t) {
    double t2;
    if (t < 0.5) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    } else {
        t = -2 * 2 + 2;
        t2 = t * t;
        return 1 - (t * t2 * t2) / 2;
        // t2 = (--t) * t;
        // return 1 + 16 * t * t2 * t2;
    }
}

double easeInExpo(double t) {
    return (pow(2, 8 * t) - 1) / 255;
}

double easeOutExpo(double t) {
    return 1 - pow(2, -8 * t);
}

double easeInOutExpo(double t) {
    if (t < 0.5) {
        return (pow(2, 16 * t) - 1) / 510;
    } else {
        return 1 - 0.5 * pow(2, -16 * (t - 0.5));
    }
}

double easeInCirc(double t) {
    return 1 - sqrt(1 - t);
}

double easeOutCirc(double t) {
    return sqrt(t);
}

double easeInOutCirc(double t) {
    if (t < 0.5) {
        return (1 - sqrt(1 - 2 * t)) * 0.5;
    } else {
        return (1 + sqrt(2 * t - 1)) * 0.5;
    }
}

double easeInBack(double t) {
    return t * t * (2.70158 * t - 1.70158);
}

double easeOutBack(double t) {
    const double c1 = 1.70158;
    // const double c1 = 1.2;
    const double c3 = c1 + 1;

    t = t - 1;
    return 1 + c3 * t * t * t + c1 * t * t;
    // return 1 + (--t) * t * (2.70158 * t + 1.70158);
}

double easeInOutBack(double t) {
    if (t < 0.5) {
        return t * t * (7 * t - 2.5) * 2;
    } else {
        const double c1 = 1.70158;
        const double c2 = c1 + 1.525;

        t = 2 * t - 2;
        return (t * t * ((c2 + 1) * t + c2) + 2) / 2;
        // return 1 + (--t) * t * 2 * (7 * t + 2.5);
    }
}

double easeInElastic(double t) {
    double t2 = t * t;
    return t2 * t2 * sin(t * PI * 4.5);
}

double easeOutElastic(double t) {
    double t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cos(t * PI * 4.5);
}

double easeInOutElastic(double t) {
    double t2;
    if (t < 0.45) {
        t2 = t * t;
        return 8 * t2 * t2 * sin(t * PI * 9);
    } else if (t < 0.55) {
        return 0.5 + 0.75 * sin(t * PI * 4);
    } else {
        t2 = (t - 1) * (t - 1);
        return 1 - 8 * t2 * t2 * sin(t * PI * 9);
    }
}

double easeInBounce(double t) {
    return pow(2, 6 * (t - 1)) * dabs(sin(t * PI * 3.5));
}

double easeOutBounce(double t) {
    return 1 - pow(2, -6 * t) * dabs(cos(t * PI * 3.5));
}

double easeInOutBounce(double t) {
    if (t < 0.5) {
        return 8 * pow(2, 8 * (t - 1)) * dabs(sin(t * PI * 7));
    } else {
        return 1 - 8 * pow(2, -8 * t) * dabs(sin(t * PI * 7));
    }
}

easingFunction getEasingFunction(easingFunctionEnum function) {
    switch (function) {
        case EASE_IN_SINE: return easeInSine;
        case EASE_OUT_SINE: return easeOutSine;
        case EASE_IN_OUT_SINE: return easeInOutSine;
        case EASE_IN_QUAD: return easeInQuad;
        case EASE_OUT_QUAD: return easeOutQuad;
        case EASE_IN_OUT_QUAD: return easeInOutQuad;
        case EASE_IN_CUBIC: return easeInCubic;
        case EASE_OUT_CUBIC: return easeOutCubic;
        case EASE_IN_OUT_CUBIC: return easeInOutCubic;
        case EASE_IN_QUART: return easeInQuart;
        case EASE_OUT_QUART: return easeOutQuart;
        case EASE_IN_OUT_QUART: return easeInOutQuart;
        case EASE_IN_QUINT: return easeInQuint;
        case EASE_OUT_QUINT: return easeOutQuint;
        case EASE_IN_OUT_QUINT: return easeInOutQuint;
        case EASE_IN_EXPO: return easeInExpo;
        case EASE_OUT_EXPO: return easeOutExpo;
        case EASE_IN_OUT_EXPO: return easeInOutExpo;
        case EASE_IN_CIRC: return easeInCirc;
        case EASE_OUT_CIRC: return easeOutCirc;
        case EASE_IN_OUT_CIRC: return easeInOutCirc;
        case EASE_IN_BACK: return easeInBack;
        case EASE_OUT_BACK: return easeOutBack;
        case EASE_IN_OUT_BACK: return easeInOutBack;
        case EASE_IN_ELASTIC: return easeInElastic;
        case EASE_OUT_ELASTIC: return easeOutElastic;
        case EASE_IN_OUT_ELASTIC: return easeInOutElastic;
        case EASE_IN_BOUNCE: return easeInBounce;
        case EASE_OUT_BOUNCE: return easeOutBounce;
        case EASE_IN_OUT_BOUNCE: return easeInOutBounce;
    }
    return NULL;
}
