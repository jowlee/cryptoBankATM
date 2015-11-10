#include <unistd.h>

// send command
ssize_t cwrite(int fd, const void *buf, size_t count);

// recv command
ssize_t cread(int fd, void *buf, size_t count);
