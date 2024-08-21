// The function generator was inspired by https://github.com/nicolausYes/easing-functions (no license)
// All of the functions are from easings.net, some of them rewritten so they don't use ternary stuff

/// TODO: make them like GD easing functions: ease, elastic, bounce, exponential, sine and back.
///       Ease and elastic have easing rates (my hypothesis is that for normal ease,
///       easing rate 2 = easeQuad, easing rate 3 = easeCubic, etc. So t^{easing rate})

#include "easing.h"
#include <stddef.h>
#include <math.h>

#ifndef PI
#define PI 3.1415926545
#endif

double easeInSine(double t) {
    return 1 - cos((t * PI) / 2);
}

double easeOutSine(double t) {
    return sin((t * PI) / 2);
}

double easeInOutSine(double t) {
    return -(cos(t * PI) - 1) / 2;
}

double easeInQuad(double t) {
    return t * t;
}

double easeOutQuad(double t) {
    return 1 - (1 - t) * (1 - t);
}

double easeInOutQuad(double t) {
    if (t < 0.5) {
        return 2 * t * t;
    } else {
        t = -2 * t + 2;
        return 1 - (t * t) / 2;
    }
}

double easeInCubic(double t) {
    return t * t * t;
}

double easeOutCubic(double t) {
    t = 1 - t;
    return 1 - t * t * t;
}

double easeInOutCubic(double t) {
    if (t < 0.5) {
        return 4 * t * t * t;
    } else {
        t = -2 * t + 2;
        return 1 - (t * t * t) / 2;
    }
}

double easeInQuart(double t) {
    t *= t; // t * t
    return t * t; // (t * t) * (t * t) = t^4
}

double easeOutQuart(double t) {
    t = 1 - t;
    return 1 - (t * t * t * t);
}

double easeInOutQuart(double t) {
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    } else {
        t = -2 * t + 2;
        return 1 - (t * t * t * t) / 2;
    }
}

double easeInQuint(double t) {
    double t2 = t * t;
    return t * t2 * t2; // t * (t * t) * (t * t) = t^5
}

double easeOutQuint(double t) {
    t = 1 - t;
    double t2 = t * t;
    return 1 - t * t2 * t2;
}

double easeInOutQuint(double t) {
    double t2;
    if (t < 0.5) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    } else {
        t = -2 * t + 2;
        t2 = t * t;
        return 1 - (t * t2 * t2) / 2;
    }
}

double easeInExpo(double t) {
    if (t == 0.0)
        return 0.0;
    return pow(2, 10 * t - 10);
}

double easeOutExpo(double t) {
    if (t == 1.0)
        return 1.0;
    return 1 - pow(2, -10 * t);
}

double easeInOutExpo(double t) {
    if (t == 0.0)
        return 0.0;
    if (t == 1.0)
        return 1.0;
    if (t < 0.5) {
        return pow(2, 20 * t - 10) / 2;
    } else {
        return (2 - pow(2, -20 * t + 10)) / 2;
    }
}

double easeInCirc(double t) {
    return 1 - sqrt(1 - (t * t));
}

double easeOutCirc(double t) {
    t = t - 1;
    return sqrt(1 - (t * t));
}

double easeInOutCirc(double t) {
    if (t < 0.5) {
        t = t * 2;
        return (1 - sqrt(1 - (t * t))) / 2;
    } else {
        t = -2 * t + 2;
        return (sqrt(1 - (t * t)) + 1) / 2;
    }
}

double easeInBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1;
    return c3 * t * t * t - c1 * t * t;
}

double easeOutBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1;

    t = t - 1;
    return 1 + c3 * t * t * t + c1 * t * t;
}

double easeInOutBack(double t) {
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;

    if (t < 0.5) {
        t *= 2;
        return (t*t * ((c2 + 1) * t - c2)) / 2;
    } else {
        t = 2 * t - 2;
        return (t*t * ((c2 + 1) * t + c2) + 2) / 2;
    }
}

double easeInElastic(double t) {
    if (t == 0.0)
        return 0.0;
    if (t == 1.0)
        return 1.0;
    
    const double c4 = (2 * PI) / 3;

    return -pow(2, 10 * t - 10) * sin((t * 10 - 10.75) * c4);
}

double easeOutElastic(double t) {
    if (t == 0.0)
        return 0.0;
    if (t == 1.0)
        return 1.0;
    
    const double c4 = (2 * PI) / 3;

    return pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1;
}

double easeInOutElastic(double t) {
    if (t == 0.0)
        return 0.0;
    if (t == 1.0)
        return 1.0;
    
    const double c5 = (2 * PI) / 4.5;

    if (t < 0.5) {
        return -(pow(2, 20 * t - 10) * sin((20 * t - 11.125) * c5)) / 2;
    } else {
        return (pow(2, -20 * t + 10) * sin((20 * t - 11.125) * c5)) / 2 + 1;
    }
}

double easeOutBounce(double t);

double easeInBounce(double t) {
    return 1 - easeOutBounce(1 - t);
}

double easeOutBounce(double t) {
    const double n1 = 7.5625;
    const double d1 = 2.75;

    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2 / d1) {
        // easings.net also had horrendously unreadable JavaScript code here:
        // return n1 * (x -= 1.5 / d1) * x * 0.75;
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    } else if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    } else {
        t -= 2.625 / d1;
        return n1 * t * t + 0.984375;
    }
}

double easeInOutBounce(double t) {
    if (t < 0.5) {
        return (1 - easeOutBounce(1 - 2 * t)) / 2;
    } else {
        return (1 + easeOutBounce(2 * t - 1)) / 2;
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
