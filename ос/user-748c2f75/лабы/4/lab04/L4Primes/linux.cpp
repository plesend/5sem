thread_local int tlsBuf[MAX_PRIMES_PER_THREAD];
thread_local int localCount = 0;

void* Func(void* lpParam) {
    auto isPrime = [](int n) -> bool {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        for (int i = 3; i * i <= n; i += 2)
            if (n % i == 0) return false;
        return true;
        };

    Range* range = reinterpret_cast<Range*>(lpParam);
    if (!range) return nullptr;

    localCount = 0;

    for (int i = range->start; i <= range->end; i++) {
        if (isPrime(i)) {
            if (localCount < MAX_PRIMES_PER_THREAD)
                tlsBuf[localCount++] = i;
        }
    }

    for (int i = 0; i < localCount; i++)
        primes[primesCount++] = tlsBuf[i];

    delete range;
    return nullptr;
}