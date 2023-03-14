#ifndef CHILD_PROCESS_CPP
#define CHILD_PROCESS_CPP

/**
 * This source file contains the implementation for the methods 
 * in the ChildProcess class.
 *
 * Copyright (C) 2023 rfischmar26@gmail.com
 */

#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>
#include <vector>
#include "ChildProcess.h"

void
ChildProcess::myExec(StrVec argList) {
    std::vector<char*> args;    // list of pointers to args
    for (auto& s : argList) {
        args.push_back(&s[0]);  // address of 1st character
    }
    // nullptr is very important
    args.push_back(nullptr);
    // Make execvp system call to run desired process
    execvp(args[0], &args[0]);
    // In case execvp ever fails, we throw a runtime execption
    throw std::runtime_error("Call to execvp failed!");
}

// Implement the constructor
ChildProcess::ChildProcess() : childPid(-1) {}

// Implement the destructor.  The destructor is an empty method
// because this class does not have any resources to release.
ChildProcess::~ChildProcess() {}

int ChildProcess::forkNexec(const StrVec& strVec) {
    childPid = fork();
    if (childPid == 0) {
        myExec(strVec);
    }
    return childPid;
}

int ChildProcess::wait() const {
    int exitCode = 0;
    waitpid(childPid, &exitCode, 0);
    return exitCode;
}

#endif

