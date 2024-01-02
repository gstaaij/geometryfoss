#include "util.h"

double dabs(double in) {
    return in < 0 ? -in : in;
}

long labs(long in) {
    return in < 0 ? -in : in;
}