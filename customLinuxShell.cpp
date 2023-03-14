/**
 * Copyright (C) 2023 fischmrl@miamioh.edu
 * 
 * A program for a textual shell program analogous to bash used in Linux.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <boost/asio.hpp>
#include "ChildProcess.h"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

// A convenience shortcut to a vector-of-strings
using StrVec = std::vector<std::string>;

/**
 * Helper method to setup a TCP stream for downloading data from an
 * web-server.
 * 
 * @param host The host name of the web-server. Host names can be of
 * the from "www.miamioh.edu" or "ceclnx01.cec.miamioh.edu".  This
 * information is typically extracted from a given URL.
 *
 * @param path The path to the file being download.  An example of
 * this value is ""
 *
 * @param socket The TCP stream (aka socket) to be setup by this
 * method.  After a successful call to this method, this stream will
 * contain the response from the web-server to be processed.
 *
 * @param port An optional port number. The default port number is "80"
 *
 */
void setupDownload(const std::string& hostName, const std::string& path,
                   tcp::iostream& data, const std::string& port = "80") {
    // Create a boost socket and request the log file from the server.
    data.connect(hostName, port);
    data << "GET "   << path     << " HTTP/1.1\r\n"
         << "Host: " << hostName << "\r\n"
         << "Connection: Close\r\n\r\n";
}

/**
 * This function runs the user-specified command using fork, exec, and waitpid.
 * 
 * @param words The string vector of words entered by the user.
 * 
 */
void cp(std::vector<std::string>& words) {
    ChildProcess cp;
    cp.forkNexec(words);
    std::cout << "Running:";
    for (auto& i : words) {
        std::cout << " " << i;
    }
    std::cout << std::endl;
    int i = cp.wait();
    std::cout << "Exit code: " << i << std::endl;
}

/**
 * This function reads in a URL entered by the user and uses the setupDownload
 * helper method to setup a TCP stream for downloading data from a
 * web-server.
 * 
 * @param str the URL specified by the user.
 * 
 * @param url the TCP stream to be set up.
 * 
 */
void url(std::string& str, tcp::iostream& url) {
    std::string url1 = str;
    url1 = url1.substr(url1.find('/') + 2);
    int i = url1.find('/');
    std::string url2 = "/" + url1.substr(i + 1);
    url1 = url1.substr(0, i);
    setupDownload(url1, url2, url);
}

/**
 * This function runs the serial transmission of the specified processes.
 * 
 * @param str The URL entered by the user.
 * 
 */
void serial(std::string& str) {
    tcp::iostream is;
    url(str, is);
    std::string line;
    for (std::string hdr; std::getline(is, hdr) 
            && !hdr.empty() && hdr != "\r";) {}
    for (std::string line; std::getline(is, line);) {
        std::istringstream in(line);
        std::string word, tmp;
        std::vector<std::string> words;
        while (in >> std::quoted(word)) words.push_back(word);
        if (words.empty()) continue;
        tmp = words.at(0);
        if (tmp != "#" && tmp != "HTTP/1.1") {
            cp(words);
        }
    }
}

/**
 * This function runs the specified command in the para function.
 * 
 * @param nums A string vector of the numbers from the specified URL.
 * 
 * @param words A string vector of the commands from the specified URL.
 */
void paraCP(StrVec& nums, StrVec& words) {
    nums.push_back(words.at(1));
    ChildProcess cp;
    std::cout << "Running: " 
        << words.at(0) << " " << words.at(1) << std::endl;
    cp.forkNexec({words.at(0), words.at(1)});
}

/**
 * This function runs the specified processes in parallel.
 * 
 * @param str The URL entered by the user.
 * 
 */
void para(std::string& str) {
    tcp::iostream is;
    url(str, is);
    std::string line;
    StrVec nums;
    ChildProcess cp;
    for (std::string line; std::getline(is, line);) {
        std::istringstream in(line);
        std::string word; 
        StrVec words;
        while (in >> std::quoted(word)) words.push_back(word);
        if (words.empty()) continue;
        if (words.at(0) != "#") {
            if (words.at(0) == "sleep") {
                paraCP(nums, words);
            }
        }
    }
    for (auto& i : nums) {
        std::string p = i + " ";
        int j = cp.wait();
        std::cout << "Exit code: " << j << std::endl;
    }
}

/**
 * This function takes the user input and determines which command to execute
 * based on the command line arguments entered.
 *
 * @param is The user input in the terminal.
 * 
 * @param prompt The symbol to prompt the user to enter a line in the terminal.
 * 
 */
void process(std::istream& is = std::cin, const std::string& prompt = "> ") {
        std::string line;
        while (std::cout << prompt, std::getline(std::cin, line)) {
            std::istringstream is(line);
            std::string word, tmp;
            std::vector<std::string> words;
            while (is >> std::quoted(word)) words.push_back(word);
            if (words.empty()) continue;
            tmp = words.at(0);
            if (tmp != "#" && !tmp.empty()) {
                if (tmp == "exit") {
                    break;
                } else if (tmp == "SERIAL") {
                    serial(words.at(1));
                } else if (tmp == "PARALLEL") {
                    para(words.at(1));
                } else {
                    cp(words);
                }
            }
        }
}

/**
 * The main function that calls the process function.
 *
 * \param[in] argc The number of command-line arguments.  This program
 * requires exactly one command-line argument.
 *
 * \param[in] argv The actual command-line argument. This should be an URL.
 */
int main(int argc, char *argv[]) {
    process();
}
