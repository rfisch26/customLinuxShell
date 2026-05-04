/**
 * A program for a textual shell program analogous to bash used in Linux.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <boost/asio.hpp>
#include "ChildProcess.h"

using namespace boost::asio;
using namespace boost::asio::ip;

using StrVec = std::vector<std::string>;

/**
 * Helper method to setup a TCP stream for downloading data from a
 * web-server.
 *
 * @param hostName The host name of the web-server.
 * @param path The path to the file being downloaded.
 * @param data The TCP stream to be set up by this method.
 * @param port An optional port number. Defaults to "80".
 */
void setupDownload(const std::string& hostName, const std::string& path,
                   tcp::iostream& data, const std::string& port = "80") {
    data.connect(hostName, port);
    data << "GET "   << path     << " HTTP/1.1\r\n"
         << "Host: " << hostName << "\r\n"
         << "Connection: Close\r\n\r\n";
}

/**
 * Forks and executes the given command, then prints its exit code.
 *
 * @param words The command and its arguments.
 */
void cp(std::vector<std::string>& words) {
    ChildProcess cp;
    cp.forkNexec(words);
    int exitCode = cp.wait();
    if (exitCode != 0) {
        std::cout << "Exit code: " << exitCode << std::endl;
    }
}

/**
 * Parses a URL string and sets up a TCP stream to the web-server.
 *
 * @param str The URL specified by the user.
 * @param stream The TCP stream to be set up.
 */
void url(std::string& str, tcp::iostream& stream) {
    std::string host = str.substr(str.find('/') + 2);
    size_t slash = host.find('/');
    std::string path = "/" + host.substr(slash + 1);
    host = host.substr(0, slash);
    setupDownload(host, path, stream);
}

/**
 * Fetches a list of commands from the given URL and runs them serially.
 *
 * @param str The URL to fetch commands from.
 */
void serial(std::string& str) {
    tcp::iostream is;
    url(str, is);
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
 * Forks a child process for the given command and records it for later waiting.
 *
 * @param procs A vector to store the forked ChildProcess objects.
 * @param words The command and its arguments.
 */
void paraCP(std::vector<ChildProcess>& procs, StrVec& words) {
    std::cout << "Running:";
    for (auto& w : words) std::cout << " " << w;
    std::cout << std::endl;
    ChildProcess cp;
    cp.forkNexec(words);
    procs.push_back(std::move(cp));
}

/**
 * Fetches a list of commands from the given URL and runs them in parallel.
 *
 * @param str The URL to fetch commands from.
 */
void para(std::string& str) {
    tcp::iostream is;
    url(str, is);
    std::vector<ChildProcess> procs;
    for (std::string line; std::getline(is, line);) {
        std::istringstream in(line);
        std::string word;
        StrVec words;
        while (in >> std::quoted(word)) words.push_back(word);
        if (words.empty()) continue;
        if (words.at(0) != "#") {
            paraCP(procs, words);
        }
    }
    for (auto& proc : procs) {
        int exitCode = proc.wait();
        if (exitCode != 0) {
            std::cout << "Exit code: " << exitCode << std::endl;
        }
    }
}

/**
 * Runs the main shell loop, reading and dispatching commands from the given
 * input stream.
 *
 * @param is The input stream to read commands from. Defaults to std::cin.
 * @param prompt The prompt string displayed to the user. Defaults to "> ".
 */
void process(std::istream& is = std::cin, const std::string& prompt = "> ") {
    std::string line;
    while (std::cout << prompt, std::getline(is, line)) {
        std::istringstream ss(line);
        std::string word, tmp;
        std::vector<std::string> words;
        while (ss >> std::quoted(word)) words.push_back(word);
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

int main() {
    process();
    return 0;
}