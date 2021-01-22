#include <ruby.h>

int ruby_time()
{
  ruby_init();
  ruby_init_loadpath();
  int status;
  rb_load_protect(rb_str_new2("./3/test.rb"), 0, &status);
  if (status) {
    printf("Error embedding ruby\n");
  };
  ruby_finalize();
  return status;
}