#include "buildpp/build.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  // === SELF ===
  bool run = false;
  if (argc == 2) {
    if (0 == strcmp(argv[1], "self")) {
      struct project* self = project(argv[0]);
      add_source_file_project(self, "./buildpp/build.cpp");
      add_source_file_project(self, "./build.cpp");
      set_compiler_project(self, "musl-clang");
      add_extra_file_project(self, "./buildpp/build.h");
      add_flag_project(self, "-static");
      setbuf(stdout, NULL);
      if (rebuild_needed_project(self)) {
        printf("Rebuild needed!\n");
        if (compile_project(self)) {
          free_project(self);
          execl(argv[0], argv[0], argv[1], NULL);
          perror("execl failed");
          exit(1);
        } else {
          perror("Rebuild failed!\n");
          exit(1);
        }
      }
      exit(0);
    } else if (0 == strcmp(argv[1], "r")) {
      run = true;
      goto comp;
    }
    // === SELF ===

    // struct project* haversine = project("./haversine/program");
    // set_compiler("clang");
    // add_flag("-lstdc++");
    // add_src_dir("./haversine/");
    // compile();
  } else {
  comp:
    struct project* haversine = project("./program");
    set_compiler("clang");
    add_flag("-lstdc++");
    add_flag("-std=c++17");
    add_src_dir("./src");
    add_include_dir("./include");
    add_source_file("./main.cpp");
    if (compile()) {
      if (run) execl("./program", "./program", NULL);
    }
  }
  return 0;
}
