#include <unistd.h>      // imports
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the functions from the first file
int sum (int a, int b) {
    return a + b;
}

int sub (int a, int b) {
    return a - b;
}

int multiply (int a, int b) {
    return a * b;
}

// Define the functions from the second file
int add (int a, int b);

int kerel_main () {

    const char *green = "\033[0;32m";
    const char *white = "\033[0;37m";
    const char *reset = "\033[0m";
    const char *red = "\033[0;31m";

    // Use the syscall function to invoke a system call by its number
    // For example, 1 is the system call number for write on Linux
    // See https://syscalls.kernelgrok.com/ for a list of system call numbers
    syscall(1, STDOUT_FILENO, "%s[ OK ]%s Loaded kernel logger\n", green, white, 13);

    // Use the fork system call to create a new child process
    pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        // Child process
        // Use the execve system call to replace the current process image with a new one
        // For example, run the /bin/ls program with some arguments and environment variables
        char *argv[] = {"/bin/ls", "-l", "-a", NULL};
        char *envp[] = {"PATH=/bin", "USER=root", NULL};
        execve("/bin/ls", argv, envp);
        // If execve returns, it means an error occurred
        perror("execve");
        exit(1);
    }
    else {
        // Parent process
        // Use the waitpid system call to wait for the child process to terminate
        int status;
        waitpid(pid, &status, 0);
        // Check the exit status of the child process
        if (WIFEXITED(status)) {
            printf("[-] Child process exited with code %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status)) {
            printf("[-] Child process killed by signal %d\n", WTERMSIG(status));
        }
        else {
            printf("[!] Child process terminated abnormally\n");
        }
        // Use the exit system call to terminate the parent process
        exit(0);
    }

    // Use the socket system call to create a new socket
    // For example, create a TCP socket using the IPv4 protocol
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("%s[ FAILED ]%s Loading sockfd system call\n", red, white)
        perror("socket");
        exit(1);
    }

    // Use the bind system call to assign a local address to the socket
    // For example, bind the socket to any IP address and port 8080
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("%s[ FAILED ]%s Loading bind system call\n", red, white")
        perror("bind");
        exit(1);
    }

    // Use the listen system call to mark the socket as passive
    // For example, allow up to 10 pending connections
    if (listen(sockfd, 10) == -1) {
        "%s[ FAILED ]%s Loading sockfd system call as passive\n", red, white
        perror("listen");
        exit(1);
    }

    // Use the accept system call to accept a connection from a client
    // For example, accept the first incoming connection and get its address
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (connfd == -1) {
        "%s[ FAILED ]%s Loading accept system call\n", red, white
        perror("accept");
        exit(1);
    }
    // Print the client's IP address and port
    printf("[-] Connected to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Use the send and recv system calls to exchange data with the client
    // For example, send a welcome message and echo back whatever the client sends
    char buffer[1024];
    int n;
    // Send a welcome message
    strcpy(buffer,"%s[ OK ]%s Kernel core loaded!\n", green, white);
    n = send(connfd, buffer, strlen(buffer), 0);
    if (n == -1) {
        // Send failed
        perror("send");
        exit(1);
    }
    // Receive and echo data until the client closes the connection
    while ((n = recv(connfd, buffer, sizeof(buffer), 0)) > 0) {
        // Echo back the data
        n = send(connfd, buffer, n, 0);
        if (n == -1) {
            "%s[ FAILED ]%s Loading send data\n", red, white
            perror("send");
            exit(1);
        }
    }
    if (n == -1) {
        "%s[ FAILED ]%s Receiving send data\n", red, white
        perror("recv");
        exit(1);
    }
    printf("[-] closed connection\n")
    close(connfd);

    // Use the brk and sbrk system calls to change the location of the program break
    // For example, allocate 4096 bytes of memory using sbrk
    void *ptr = sbrk(4096);
    if (ptr == (void *)-1) {
        /"%s[ FAILED ]%s Loading sbrk system call\n", red, white
        perror("sbrk");
        exit(1);
    }
    // Use the allocated memory
    strcpy(ptr, "%s[ OK ]%s Kernel memory allocation set\n", green, white);
    printf("[",KGRN,"%sOK\n",KWHT"]","Data at %p: %s\n", ptr, (char *)ptr);
    // Deallocate the memory using brk
    if (brk(ptr) == -1) {
        /"%s[ FAILED ]%s Loading brk system call\n", red, white
        perror("brk");
        exit(1);
    }

    // Use the mmap and munmap system calls to map or unmap files or devices into memory
    // For example, map the /etc/passwd file into memory
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd == -1) {
        "%s[ FAILED ]%s Open failed\n", red, white
        perror("open");
        exit(1);
    }
    // Get the file size
    struct stat st;
    if (fstat(fd, &st) == -1) {
        "%s[ FAILED ]%s Loading fstat failed\n", red, white
        perror("fstat");
        exit(1);
    }
    size_t size = st.st_size;
    // Map the file into memory
    char *data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        "%s[ FAILED ]%s Loading mmap failed\n", red, white
        perror("mmap");
        exit(1);
    }
    // Use the mapped data
    printf("[-] File contents:\n%s\n", data);
    // Unmap the file from memory
    if (munmap(data, size) == -1) {
        "%s[ FAILED ]%s Loading unmap failed\n", red, white
        perror("munmap");
        exit(1);
    }
    printf("[-] file closed")
    close(fd);

    // Use the mprotect system call to change the protection of memory pages
    // For example, change the protection of the first page of the program to read-only
    void *page = (void *)((unsigned long)main & ~(getpagesize() - 1)); // Get the page address of main
    if (mprotect(page, getpagesize(), PROT_READ) == -1) {
        "%s[ FAILED ]%s Loading mprotect\n", red, white
        perror("mprotect");
        exit(1);
    }

    // Try to write to the page
    strcpy(page, "This will cause a segmentation fault.");
    // This line will not be reached
    printf("This will not be printed.\n");

    return 0;
}
