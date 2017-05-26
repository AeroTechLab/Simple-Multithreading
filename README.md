# Simple Multithreading

Utilities and abstractions for creating multiplatform multithreaded applications in C

It offers:

- Individual [threads](https://en.wikipedia.org/wiki/Thread_(computing)) management (start,stop)
- Thread synchornization: [locks/mutexes](https://en.wikipedia.org/wiki/Mutual_exclusion) and [semaphores](https://en.wikipedia.org/wiki/Semaphore_(programming))
- [Thread-safe](https://en.wikipedia.org/wiki/Thread_safety) data structures: [lists](https://en.wikipedia.org/wiki/List_(abstract_data_type)), [queues](https://en.wikipedia.org/wiki/Queue_(abstract_data_type)) and [maps/dictionaries/hash tables](https://en.wikipedia.org/wiki/Hash_table)

### Build dependencies

The only required dependencies are [Win32 threads](https://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx) when building on Windows and [Pthreads](https://en.wikipedia.org/wiki/POSIX_Threads) when building on [POSIX](https://en.wikipedia.org/wiki/POSIX) systems

Also, the thread safe maps library uses a built-in copy of [Klib](https://github.com/attractivechaos/klib)'s hash library

### Documentation

Descriptions of how the functions and data structures work are available at the [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)-generated [documentation pages](https://labdin.github.io/Simple-Multithreading/)
