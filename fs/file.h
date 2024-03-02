enum fd_status {
    CLOSED,
    RESIDENT, // open and in cache dir
    NON_RESIDENT, // open and not in cache
};

struct file_descriptor {
    int fdnum;
    int flags;
    enum fd_status status;
    char* path;
};

struct file_descriptor* lazy_open(const char* path, int flags);