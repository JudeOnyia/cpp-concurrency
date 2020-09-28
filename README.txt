# cpp-concurrency
Several implementations are involved in this project:

1)  A class template called queue is developed that provides the functionality of a concurrent bounded FIFO queue.
The queue class template has a single template parameter T, which corresponds to the type of the elements stored in
the queue.

2)  A class called thread_pool is developed that provides the functionality of a thread pool.

3)  A multithreaded algorithm is developed to compute an image representation of a Julia set (which is a type of
fractal set). The interface to this algorithm is through a function template called compute_julia_set. Internally,
this function utilizes a thread pool to perform the necessary computation.
