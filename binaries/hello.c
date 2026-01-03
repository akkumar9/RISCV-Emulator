void _start() {
    // Simple computation
    int a = 10;
    int b = 20;
    int c = a + b;
    
    // Exit with result in a0 (x10)
    register int result asm("a0") = c;
    asm volatile("li a7, 93");   // syscall number for exit
    asm volatile("ecall");        // make syscall
    
    // Infinite loop (should never reach here)
    while(1);
}