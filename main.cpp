#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <ctime>
#include <sys/stat.h>
#include <direct.h>
#include <vector>

using namespace std;

int step = 0;

vector<std::string>marker = {
        "Output: ",
        "Error: ",
        "Progress: "
};

std::string getCurrentTimeString() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tstruct);
    return buf;
}

void logResult(const std::string& message) {
    // 检查并创建Logs文件夹
    struct stat info;
    if (stat("Logs", &info) != 0) {
        // 文件夹不存在，创建文件夹
        _mkdir("Logs");
    }

    std::string timeStr = getCurrentTimeString();
    std::string fileName = "Logs/solver_" + timeStr + ".log";

    std::ofstream logFile(fileName, std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    } else {
        std::cout << marker[1]<<"Unable to open log file: " << fileName << std::endl;
        //std::cerr << "Unable to open log file: " << fileName << std::endl;
        std::cout.flush();
    }
}

void signalHandler(int signal) {
    std::cout << marker[0]<<"Interrupt by User." << std::endl;
    std::cout.flush();
    logResult("Interrupt by User. Stop at Step "+std::to_string(step) + ".");
    // sleep(1);
    exit(signal);
}

void simulateLongRunningFunction() {
    try {
        for (int i = 1; i <= 10; ++i) {
            std::cout << marker[2] << i << std::endl;
            std::cout.flush(); // progress test
            std::cout << marker[0] << "Output message catch test: " << i << std::endl;
            std::cout.flush(); // output test
            std::cout << marker[1] << "Error message catch test: " << i << std::endl;
            std::cout.flush(); // error test
            sleep(1); // 模拟耗时操作
            step = i;
        }
        std::cout << marker[0]<<"Success" << std::endl;
        logResult("Success");
    }
    catch (const std::runtime_error& e) {
        std::cout << marker[1]<<"Fail: " << e.what() << std::endl;
        std::cout.flush();
        logResult(std::string("Fail: ") + e.what()+", Stop at Step "+std::to_string(step)+ ".");
        exit(0);
    }
}

void readFile(const std::string& path) {
    std::ifstream inFile(path);
    if (!inFile.is_open()) {

        std::cout <<marker[1]<< "Unable to open file: " << path << std::endl;
        //std::cerr << "Unable to open file: " << path << std::endl;
        logResult("Unable to open file: " + path);
        exit(1);
    }
    inFile.close();
}


int main(int argc, char* argv[]) {
    std::string filePath;
    bool isAbsolutePath = false;
    bool hasFilePath = false;

    // 解析命令行参数
    if (argc == 3) {
        if (strcmp(argv[1], "-d") == 0) {
            filePath = argv[2];
            isAbsolutePath = true;
            hasFilePath = true;
        } else if (strcmp(argv[1], "-r") == 0) {
            filePath = argv[2];
            isAbsolutePath = false;
            hasFilePath = true;
        } else {
            std::cout <<marker[1]<< "Invalid argument. Use -d for absolute path or -r for relative path." << std::endl;
            //std::cerr << "Invalid argument. Use -d for absolute path or -r for relative path." << std::endl;
            return 1;
        }
    } else if (argc != 1) {
        std::cout <<marker[1]<< "Usage: " << argv[0] << " [-d <absolute_path> | -r <relative_path>]" << std::endl;
        //std::cerr << "Usage: " << argv[0] << " [-d <absolute_path> | -r <relative_path>]" << std::endl;
        return 1;
    }

    // 注册信号处理函数
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    //signal(SIGKILL, signalHandler);

    // 如果有指定文件路径，处理路径并读取文件
    if (hasFilePath) {
        if (!isAbsolutePath) {
            char currentPath[FILENAME_MAX];
            if (!getcwd(currentPath, sizeof(currentPath))) {
                std::cout <<marker[1]<< "Error getting current working directory." << std::endl;
                //std::cerr << "Error getting current working directory." << std::endl;
                return 1;
            }
            filePath = std::string(currentPath) + "/" + filePath;
        }
        readFile(filePath);
    }

    step = 0;
    // 调用模拟耗时长的函数
    simulateLongRunningFunction();

    return 0;
}
