#include <stdio.h>

void a();
void b();

void a() {
    printf("a\n");
    b();
}

void b() {
    printf("b\n");
    a();
}

int main() {
    a();
    
    return 0;
}
