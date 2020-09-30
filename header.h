#define NOT_READY -1
#define FILLED 0
#define TAKEN 1
#define END = -2

struct Memory {
    int status;
    int request[10];
    int number;
};

