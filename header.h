#define NOT_READY -1
#define FILLED 0
#define READY 1
#define END = -2

struct Memory {
    int status;
    int request[10];
    int numRequests;
    int current;
    int number;
};

