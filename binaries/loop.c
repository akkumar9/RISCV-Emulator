void _start() {
    int sum = 0;
    
    // Simple loop: sum = 1+2+3+...+100
    for (int i = 1; i <= 100; i++) {
        sum = sum + i;
    }
    // sum should be 5050
    
    // Exit with result
    register int ret asm("a0") = sum;
    asm volatile("li a7, 93");
    asm volatile("ecall");
    
    while(1);
}