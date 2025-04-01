#include "../buildpp/build.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  struct project* self = project(argv[0]);
  add_source_file_project(self, "../buildpp/build.cpp");
  add_source_file_project(self, "./build.cpp");
  set_compiler_project(self, "musl-clang");
  add_extra_file_project(self, "../buildpp/build.h");
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
  // Proj
  struct project* _8086 = project("./program");
  set_compiler("g++");
  add_flag("-lstdc++");
  add_flag("-std=c++17");
  add_include_dir("./");
  add_source_file("./main.cpp");
  if (compile()) {
    printf("\n\n");
    execl("./program", "./program", NULL);
  }

  return 0;
}
