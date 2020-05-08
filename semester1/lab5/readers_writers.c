#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "cert-msc30-c"

#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define READER_LOG_START "-----------------------------------------"
#define CREATE_LOG_START ".............................................................................."

#define SH_MEM_KEY 101
#define SEM_GROUP_KEY 103

#define PERMS S_IRWXU | S_IRWXG | S_IRWXO

#define COUNT 20
#define WRITERS 3
#define READERS 5

#define ACTIVE_WR 0
#define ACTIVE_RR 1
#define WAITING_W 2
#define WAITING_R 3

#define INC 1
#define DEC -1
#define CHK 0

#define SEM_ACTIVE_WR 0
#define SEM_ACTIVE_RR 1
#define SEM_WAITING_W 2
#define SEM_WAITING_R 3

struct sembuf start_reading[] = {
    {SEM_WAITING_R, INC, SEM_UNDO},
    {SEM_ACTIVE_WR, CHK, SEM_UNDO},
    {SEM_WAITING_W, CHK, SEM_UNDO},
    {SEM_ACTIVE_RR, INC, SEM_UNDO},
    {SEM_WAITING_R, DEC, SEM_UNDO}
};
struct sembuf stop_read[] = {
    {SEM_ACTIVE_RR, DEC, SEM_UNDO}
};

struct sembuf start_writing[] = {
    {SEM_WAITING_W, INC, SEM_UNDO},
    {SEM_ACTIVE_RR, CHK, SEM_UNDO},
    {SEM_ACTIVE_WR, CHK, SEM_UNDO},
    {SEM_ACTIVE_WR, INC, SEM_UNDO},
    {SEM_WAITING_W, DEC, SEM_UNDO}
};
struct sembuf stop_write[] = {
    {SEM_ACTIVE_WR, DEC, SEM_UNDO}
};

int *shared_value;

pid_t *child_pids;

#define CHECK_ERR(val, err, tag) {if (val == (err)) { \
    perror(tag);\
    exit(1);}\
}

void kill_writers() {
    for (int i = 0; i < WRITERS; i++) {
        if (child_pids[i] == getpid()) {
            continue;
        }
        kill(child_pids[i], SIGTERM);
    }
}

int init_sh_mem() {
    int fd = shmget(SH_MEM_KEY, sizeof(int), IPC_CREAT | PERMS);
    CHECK_ERR(fd, -1, "shmget")
    return fd;
}

int *get_sh_mem_addr(const int fd) {
    int *addr = (int *) shmat(fd, NULL, 0);
    CHECK_ERR(addr, (int *) -1, "shmat")
    return addr;
}

int init_semaphores() {
    int sem_gr_fd = semget(SEM_GROUP_KEY, 4, IPC_CREAT | PERMS);
    CHECK_ERR(sem_gr_fd, -1, "semget")

    int mem_ctrl    = semctl(sem_gr_fd, WAITING_R, SETVAL, 0);
    int writer_ctrl = semctl(sem_gr_fd, ACTIVE_WR, SETVAL, 0);
    int reader_ctrl = semctl(sem_gr_fd, ACTIVE_RR, SETVAL, 0);
    int wait_ctrl   = semctl(sem_gr_fd, WAITING_W, SETVAL, 0);

    if (mem_ctrl == -1 || writer_ctrl == -1 || reader_ctrl == -1 || wait_ctrl == -1) {
        perror("semctl");
        exit(1);
    }

    return sem_gr_fd;
}

void writer(int sem_gr_fd, int number) {
    int can = semop(sem_gr_fd, start_writing, 5);
    CHECK_ERR(can, -1, "start_writing")
    // check condition for stopping all writers
    if (*shared_value >= COUNT) {
        kill_writers();
        int sem_op_stop = semop(sem_gr_fd, stop_write, 1);
        CHECK_ERR(sem_op_stop, -1, "stop_write")
        exit(0);
    }
    // write
    (*shared_value)++;
    printf("Writer #%d, pid=%d wrote value %d\n", number, getpid(), *shared_value);

    int sem_op_stop = semop(sem_gr_fd, stop_write, 1);
    CHECK_ERR(sem_op_stop, -1, "stop_write")

    sleep((unsigned int) (rand() % 4 + 1));
}

void reader(int sem_gr_fd, int number) {
    int can = semop(sem_gr_fd, start_reading, 5);
    CHECK_ERR(can, -1, "start_reading")
    // read
    int val = *shared_value;
    printf(READER_LOG_START"Reader #%d, pid=%d read value: %d\n", number, getpid(), val);

    int sem_op_stop = semop(sem_gr_fd, stop_read, 1);
    CHECK_ERR(sem_op_stop, -1, "stop_read")
    // check condition for stopping all readers
    if (val >= COUNT) {
        exit(0);
    }

    sleep((unsigned int) (rand() % 4) + 1);
}

void init_writer(int number, int sem_id) {
    pid_t pid;

    if ((pid = fork()) == -1) {
        printf("Can't fork");
        exit(1);
    }

    if (pid == 0) {
        printf(CREATE_LOG_START"Writer #%d is running, pid: %d\n", number, getpid());
        while (1) {
            writer(sem_id, number);
        }
    }
    else {
        child_pids[number] = pid;
    }
}

void init_reader(int number, const int sem_id) {
    pid_t pid;

    if ((pid = fork()) == -1) {
        printf("Can't fork");
        exit(1);
    }

    if (pid == 0) {
        printf(CREATE_LOG_START"Reader #%d created, pid: %d\n", number, getpid());
        while (1) {
            reader(sem_id, number);
        }
    }
    else {
        child_pids[WRITERS + number] = pid;
    }
}

int main() {
    srand((unsigned int) time(NULL)); // NOLINT(cert-msc32-c,cert-msc51-cpp)

    int sh_mem_fd = init_sh_mem();
    int *sh_mem = get_sh_mem_addr(sh_mem_fd);

    shared_value = sh_mem;
    *shared_value = 0;
    child_pids = shared_value + 1;

    int sem_gr_fd = init_semaphores();

    for (int i = 0; i < WRITERS; i++) {
        init_writer(i, sem_gr_fd);
    }

    for (int i = 0; i < READERS; i++) {
        init_reader(i, sem_gr_fd);
    }

    for (int i = 0; i < WRITERS + READERS; i++) {
        int *status;
        wait(status);
    }
}
#pragma clang diagnostic pop