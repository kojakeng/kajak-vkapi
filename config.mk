VERSION=0.0.1

# flags
#CFLAGS = -Wall -pedantic -std=c99 -Wno-deprecated-declarations -Os \
#				 `curl-config --cflags`
#CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=${VERSION}
LDFLAGS = `curl-config --libs`
# debug flags
CFLAGS = -Wall -g -pedantic -std=c99 -Wno-deprecated-declarations -O0 \
         `curl-config --cflags`
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=${VERSION} -DDEBUG

# compiler and linker
CC = cc
AR = ar
