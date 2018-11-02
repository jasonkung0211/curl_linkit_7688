# What to call the final executable
TARGET = getipc

CC = /home/jason-kung/Downloads/OpenWrt-SDK-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-gcc

CFLAGS = -I.
CFLAGS+=-I/home/jason-kung/Downloads/OpenWrt-SDK-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/include

# libraries
LDFLAGS =-L.
LDFLAGS+=-L/home/jason-kung/Downloads/OpenWrt-SDK-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/lib -lcurl -pthread -ldl
#LDFLAGS += -L/usr/local/ssl/lib -L/usr/lib/happycoders/

LIBS = -lcurl #-lnsl -lssl -lcrypto -lsocket

# Link the target with all objects and libraries
#$(TARGET) : $(OBJS)
#	$(CC) $(CFLAGS) -o $(TARGET).run $(OBJS) $(LDFLAGS) $(TARGET).c $(LIBS)
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET).run $(TARGET).c $(LDFLAGS)

# Compile the source files into object files
# str-replace.o : str-replace.c str-replace.h
# 	$(CC) $(CFLAGS) $<

clean:
	rm -f $(TARGET).run
