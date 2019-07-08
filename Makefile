include config.mk

SRC = vkapi.c vkapi_alloc.c vkapi_opts.c vkapi_opts.c vkapi_session.c \
			vkapi_utils.c
OBJ = ${SRC:.c=.o}

all: options vkapi.a

options:
	@echo "\chktables build options:"
	@echo "CFLAGS = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC = ${CC}\n"

.c.o:
	${CC} -c ${CFLAGS} -o $@ $< 

${OBJ}: vkapi_config.h config.mk 

config.h:
	cp vkapi_config.def.h $@

vkapi.a: $(OBJ)
	${AR} rc $@ $?
	${RANLIB} $@

test_gen_request: test_gen_request.c vkapi.a
	${CC} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ test_gen_request.c vkapi.a

clean:
	rm -f chktables test_gen_request vkapi.a ${OBJ}

.PHONY: all options clean
