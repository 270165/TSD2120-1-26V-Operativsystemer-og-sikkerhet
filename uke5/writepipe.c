#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    mkfifo("myfifo1", 0666);
    int fd = open("myfifo", O_WRONLY);
    
    write(fd, "Hello", 6);
    close(fd);

    return 0;
}
