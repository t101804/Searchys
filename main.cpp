#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <curl/curl.h>
#include <regex>
#include <cstdlib>
// define a preprocessor macro to check the operating system
#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
#else
    #define CLEAR_SCREEN "clear"
#endif

// save results 
void writeStringToFile(const std::string& filePath, const std::string& data) {
    std::ofstream fileStream(filePath, std::ios::out | std::ios::app);
    if (fileStream.is_open()) {
        fileStream << data;
        fileStream.close();
    }
}

// read each line
std::vector<std::string> readLinesFromFile(const std::string& fileName) {
    std::vector<std::string> lines;
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    return lines;
}
// Callback function for libcurl to write received data
size_t writeDataCallback(void* buffer, size_t size, size_t nmemb, void* userp) {
    std::string* response = reinterpret_cast<std::string*>(userp);
    response->append(reinterpret_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

// http requests 
void makeRequests(const std::vector<std::string>& lines, int numThreads) {
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([&lines, i, numThreads]() {
            CURL* curl = curl_easy_init();

            if (!curl) {
                std::cerr << "Error initializing CURL" << std::endl;
                return;
            }
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            for (int j = i; j < lines.size(); j += numThreads) {
                 std::string url = lines[j];
                if (url.substr(0, 7) == "http://") {
                    url.replace(0, 7, "");
                }
                if (url.substr(0, 8) == "https://") {
                    url.replace(0, 8, "");
                }
                CURLcode res;
                std::string response;
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeDataCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                res = curl_easy_perform(curl);

                if (res != CURLE_OK) {
                    std::cerr << "Error making request: " << curl_easy_strerror(res) << std::endl;
                } else {
                     if (response.find("<script") != std::string::npos) {
                        std::cout << "This URL contains JavaScript : " << lines[j] << std::endl;
                        writeStringToFile("js.txt", lines[j] + "\n");
                    } else {
                        std::cout << "Not Have JS : "  << lines[j]  << std::endl;
                    }
                }
            }

            curl_easy_cleanup(curl);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}



int main() {
    system(CLEAR_SCREEN);
    std::string filename;
    int num_threads;

    std::cout << "Enter filename: ";
    std::cin >> filename;

    std::cout << "Enter number of threads: ";
    std::cin >> num_threads;
    std::vector<std::string> lines = readLinesFromFile(filename);
    makeRequests(lines, num_threads);
    return 0;
}
