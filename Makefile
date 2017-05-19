TARGET=main
SRCS=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRCS))
cc=arm-fsl-linux-gnueabi-gcc
$(TARGET):$(OBJ)
	$(cc)	$^ -o $@ 	-lpthread -L. -lTYRMCOMMU
%.o:%.c
	$(cc) -o $@ -c $< 	-lpthread -L. -lTYRMCOMMU

clean:
	rm *.o $(TARGET)
