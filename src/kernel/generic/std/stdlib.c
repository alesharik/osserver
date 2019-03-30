#include <stdint.h>
#include "std/stdlib.h"

div_t div(int numer, int denom) {
    div_t ret = {0, 0};
    if(!denom)
        return ret;//TODO throw exception
    for (int i = numer - 1; i > 0; --i) {
        ret.rem <<= 1;
        uint8_t bit = (numer >> i) & 1;
        if(bit)
            ret.rem |= 1;
        else
            ret.rem &= ~1;
        if(ret.rem > denom) {
            ret.rem = ret.rem - denom;
            ret.quot |= 1 << i;
        }
    }
    return ret;
}