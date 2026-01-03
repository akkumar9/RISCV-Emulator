int fib(int n) {
    int a = 0;
    int b = 1;
    for (int i = 0; i < n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return a;
}

void _start() {
    int result = fib(20);  // Compute fib(20)
    
    // Exit with result
    register int ret asm("a0") = result;
    asm volatile("li a7, 93");
    asm volatile("ecall");
    
    while(1);
}