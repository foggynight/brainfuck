#include <assert.h>
#include <stdio.h>

int main(void) {
    assert(sizeof(long) <= sizeof(size_t));
    return 0;
}
