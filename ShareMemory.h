#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>

//共享内存类
class ShareMemory
{
public:

   //构造函数 打开共享内存
   ShareMemory(int key); 
   //构造函数 打开共享内存
   ShareMemory(char *pathName);
   //构造函数 打开或者创建共享内存
   ShareMemory(int key, int shmSize);
   //构造函数 打开或者创建共享内存
   ShareMemory(char *pathName, int shmSize);
   //析构函数
   virtual ~ShareMemory();

   //关联共享内存
   void *mapShm(void);
   //解除关联共享内存
   int unMapShm(void);
   //删除共享内存
   int delShm(void);

private:
   //共享内存ID
   int mShmId;
   //共享内存地址
   void *mShmAddr;
};
