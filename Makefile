
#CROSS_COMPILE = arm-none-linux-gnueabi-
CROSS_COMPILE = /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

AS			= $(CROSS_COMPILE)as
LD			= $(CROSS_COMPILE)ld
CC			= $(CROSS_COMPILE)gcc
CPP			= $(CC) -E
AR			= $(CROSS_COMPILE)ar
NM			= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

# export导出的变量是给子目录下的Makefile使用的
export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP

# c编译器在编译时的参数设置
CFLAGS := -Wall -O2 -g 
# 添加头文件路径，不添加的话include目录下的头文件编译时找不到
CFLAGS += -I $(shell pwd)/include 
CFLAGS += -I/opt/libdecode/include

# 链接器的链接参数设置
LDFLAGS := -ljpeg -lz -lpng -L/opt/libdecode/lib

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

# 定义将来编译生成的可执行程序的名字
TARGET := picture_player

# 添加项目中所有用到的源文件，有顶层目录下的.c文件，和子文件夹
# 添加顶层目录下的.c文件
obj-y += main.o

# 添加顶层目录下的子文件夹（注意目录名后面加一个/）
obj-y += display/
obj-y += manage/
obj-y += file/
obj-y += page/
obj-y += input/

all:
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) -o $(TARGET) built-in.o

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)
	
cp:
	cp ../album_proj/ /root/rootfs/rootfs -rf

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)
	
