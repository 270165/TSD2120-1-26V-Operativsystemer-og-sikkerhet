#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    char buffer[10];
    int fd = open("myfifo1", O_RDONLY);

    printf("Leser %s fra named pipe\n", buffer);
    close(fd);

    return 0;
}
