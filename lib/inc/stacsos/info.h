static const size_t MAX_SIZE = 256;
static const size_t MAX_NAME_SIZE = 64;

// Struct to hold the information of a file or directory
struct info {
  char name[MAX_NAME_SIZE];
  char type;
  u64 size;
};
