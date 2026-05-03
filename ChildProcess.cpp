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
    std::vector<char*> args;
    for (auto& s : argList) {
        args.push_back(&s[0]);
    }
    args.push_back(nullptr);
    execvp(args[0], &args[0]);
    throw std::runtime_error("Call to execvp failed!");
}

ChildProcess::ChildProcess() : childPid(-1) {}

ChildProcess::~ChildProcess() {}

int ChildProcess::forkNexec(const StrVec& strVec) {
    childPid = fork();
    if (childPid == 0) {
        myExec(strVec);
    }
    return childPid;
}

int ChildProcess::wait() const {
    int status = 0;
    waitpid(childPid, &status, 0);
    return WEXITSTATUS(status);
}