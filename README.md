# Demonstration Project for std::shared_ptr Concurrency problem

The project demonstrates what can go wrong if you pass a shared_ptr
by reference in a multi-threaded program. 

It uses the [Clang ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html).

On Fedora this can be installed via 
`sudo dnf install libtsan

