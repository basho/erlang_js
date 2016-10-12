/* author Kevin Smith <ksmith@basho.com>
   copyright 2009-2010 Basho Technologies

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#define COMMAND_SIZE 2

#include <string.h>
#include <strings.h>

#include <erl_driver.h>

#include "driver_comm.h"

inline int read_int32(char **data) {
  char *d = *data;
  int value = ((((int)(((unsigned char*) (d))[0]))  << 24) |
               (((int)(((unsigned char*) (d))[1]))  << 16) |
               (((int)(((unsigned char*) (d))[2]))  << 8)  |
               (((int)(((unsigned char*) (d))[3]))));
  (*data) += 4;
  return value;
}

char *read_command(char **data) {
  char *buf = ejs_alloc(COMMAND_SIZE + 1);
  memcpy(buf, (const char *) *data, COMMAND_SIZE);
  buf[COMMAND_SIZE] = '\0';
  (*data) += 2;
  return buf;
}

char *read_string(char **data) {
  int length = read_int32(data);
  char *buf = NULL;
  if (length > 0) {
    buf = ejs_alloc(length + 1);
    memcpy(buf, (const char *) *data, length);
    buf[length] = '\0';
    (*data) += length;
  }
  return buf;
}

void *ejs_alloc(ErlDrvSizeT size) {
    void *p = driver_alloc(size);
    if (p == NULL) {
        erts_exit(1, "erlang_js: Can't allocate %lu bytes of memory\n", size);
    }
    return p;
}
