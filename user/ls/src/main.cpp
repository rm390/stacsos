/*
 * StACSOS - ls utility
 */
#include <stacsos/console.h>
#include <stacsos/info.h>
#include <stacsos/memops.h>
#include <stacsos/objects.h>
#include <stacsos/string.h>
#include <stacsos/user-syscall.h>

int ARG_LIMIT = 2;
int TRUE = 1;
int FALSE = 0;

using namespace stacsos;

// Main ls function to read the directory and print the stats
void ls(int l_flag, int r_flag, char* directory, int padding = 0) {
  // Allocate memory for the stats
  info* stats = new info[MAX_SIZE];

  // Read the directory and store the stats in the stats array
  auto result = syscalls::read_dir((u64)directory, (void*)stats, MAX_SIZE);

  // Check if the syscall was successful
  if (result.code != syscall_result_code::ok) {
    console::get().write("error: failed to read directory\n");
    return;
  }

  // Padding for recursive call
  int stats_length = result.length;
  string str = "";
  string pad_str = str.pad(padding, ' ', pad_side::LEFT);

  // Print the stats
  for (int i = 0; i < stats_length; i++) {
    // If the -l flag is set, print the size of the file
    if (l_flag) {
      console::get().writef("%s[%c] %s", pad_str.c_str(), stats[i].type,
                            stats[i].name);

      // Add padding so the file size is displayed separate from name and type
      string size = string::to_string(stats[i].size);
      string name = string(stats[i].name);
      int pad_size = 20 - name.length();
      string padded_size = size.pad(pad_size, ' ', pad_side::LEFT);

      console::get().writef("%s\n", padded_size.c_str());
    } else {
      console::get().writef("%s%s\n", pad_str.c_str(), stats[i].name);
    }

    // If the entry is a directory and recurive flag enabled, call ls
    // recursively
    if (stats[i].type == 'D' && r_flag) {
      size_t dir_len = memops::strlen(directory);
      size_t name_len = memops::strlen(stats[i].name);
      int has_trailing_slash = directory[dir_len - 1] == '/';
      int new_dir_size = dir_len + name_len + (has_trailing_slash ? 0 : 1);
      char* new_directory = new char[new_dir_size];

      memops::memcpy(new_directory, directory, dir_len);
      if (!has_trailing_slash) {
        new_directory[dir_len] = '/';
        dir_len++;
      }
      memops::memcpy(new_directory + dir_len, stats[i].name, name_len);
      new_directory[new_dir_size] = 0;

      // Recursive call to ls
      ls(l_flag, r_flag, new_directory, 5 + padding);
      delete[] new_directory;
    }
  }
  delete[] stats;
}

// Main function to parse the command line arguments and call the ls function
int main(const char* cmdline) {
  // Check if the command line is empty
  if (!cmdline || memops::strlen(cmdline) == 0) {
    console::get().write("error: usage: ls <directory> or ls -l <directory>\n");
    return 1;
  }

  size_t start = 0;
  size_t end = 0;
  char* args[ARG_LIMIT];
  int argc = 0;

  // This command line parser was created before the discovery of the string
  // split function in the string class. So it is a bit more manual than it
  // needs to be.

  // Parse the command line arguments
  for (int i = 0; i < memops::strlen(cmdline); i++) {
    // Check for spaces to separate the arguments
    if (cmdline[i] == ' ') {
      end = i;
      args[argc] = (char*)memops::memcpy(new char[end - start + 1],
                                         cmdline + start, end - start);
      args[argc][end - start] = 0;
      start = end + 1;
      argc++;
    }
  }

  // Handle the last argument if it exists
  if (start < memops::strlen(cmdline)) {
    args[argc] =
        (char*)memops::memcpy(new char[memops::strlen(cmdline) - start + 1],
                              cmdline + start, memops::strlen(cmdline) - start);
    args[argc][memops::strlen(cmdline) - start] = 0;
    argc++;
  }

  // Check the number of arguments
  if (argc == 1) {
    // Check if the single argument is a directory or flags
    if (memops::strcmp(args[0], "-l") == 0 ||
        memops::strcmp(args[0], "-r") == 0 ||
        memops::strcmp(args[0], "-lr") == 0 ||
        memops::strcmp(args[0], "-rl") == 0) {
      console::get().write(
          "error: usage: ls <directory> or ls -l <directory> or ls -r "
          "<directory> or ls -lr <directory>\n");
      return 1;
    }

    // No flags, just the directory
    ls(FALSE, FALSE, args[0]);
  } else if (argc == 2) {
    // Single flag or directory with flags
    int longListing = FALSE;
    int recursive = FALSE;

    if (memops::strcmp(args[0], "-l") == 0) {
      longListing = TRUE;
    } else if (memops::strcmp(args[0], "-r") == 0) {
      recursive = TRUE;
    } else if (memops::strcmp(args[0], "-lr") == 0 ||
               memops::strcmp(args[0], "-rl") == 0) {
      longListing = TRUE;
      recursive = TRUE;
    } else {
      // If the first argument is not recognized, exit
      console::get().write(
          "error: usage: ls <directory> or ls -l <directory> or ls -r "
          "<directory> or ls -lr <directory>\n");
      return 1;
    }

    // The second argument is the directory
    ls(longListing, recursive, args[1]);
  } else {
    // Invalid number of arguments
    console::get().write(
        "error: usage: ls <directory> or ls -l <directory> or ls -r "
        "<directory> or ls -lr <directory>\n");
    return 1;
  }

  return 0;
}
