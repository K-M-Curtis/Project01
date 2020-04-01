SRC:=$(wildcard ./*.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(SRC))
GCC:=g++
INC:=./

TARGET:=keyMngClient

all:$(TARGET)
$(TARGET):$(OBJS)
	$(GCC) $^ -I$(INC) -lcrypto -pthread  -o $@

%.o:%.cpp
	$(GCC) -c -std=c++11 $< -o $@

.PHONY:clean
clean:
	$(RM) $(OBJS) $(TARGET)
