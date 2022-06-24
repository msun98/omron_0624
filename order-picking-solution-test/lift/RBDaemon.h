#ifndef DAEMON_H
#define DAEMON_H

#include <unistd.h>
#include <iostream>
//#include <alchemy/task.h>
#include "RBSharedMemory.h"
#include "RBCAN.h"
#include "RBMotorController.h"
#include "RBLANComm.h"

#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "RBMotorController.h"

void    RBCore_SMInitialize();
int     RBCore_DBInitialize();
int     RBCore_CANInitialize();
int     RBCore_LANInitialize();
int     RBCore_ThreadInitialize();
#ifdef WITH_XENO
void    RBCore_RTThreadCon(void *);
#else
void    *RBCore_RTThreadCon(void *);
#endif

inline int pthread_create_with_affinity(void* (*t_handler)(void *), int t_cpu_no, const char* t_name, pthread_t &thread_nrt, void* arg){
    pthread_attr_t attr;
    cpu_set_t cpuset;
    int err;
    int ret;

    // initialized with default attributes
    err = pthread_attr_init(&attr);
    if (err != 0) {
       return err;
    }
    // set cpu ID
    if (t_cpu_no >= 0) {
        CPU_ZERO(&cpuset);
        CPU_SET(t_cpu_no, &cpuset);
        err = pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
        if (err != 0){
          return err;
        }
    }
    // create a RT thread
    ret = pthread_create(&thread_nrt, &attr, t_handler, arg);
    if (ret != 0) {
        err = pthread_attr_destroy(&attr);
        return err;
    }


    err = pthread_setname_np(thread_nrt, t_name);
    if (err != 0) {
        return err;
    }


    return ret;
}

#endif // DAEMON_H
