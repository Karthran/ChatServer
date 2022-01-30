#include <iostream>

#include "Application.h"

#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

#include "Logger.h"

auto main() -> int
{

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // UTF8
#endif
    //std::string user_id, message;
    //Logger logger{"log.txt"};
    //logger.write_log("1", "אבגדהו¸ז");
    //logger.fileRef().seekg(0, std::ios_base::beg);  
    //while (!logger.fileRef().eof())
    //{
    //    logger.read_log(user_id, message);
    //    std::cout << "user ID: " << user_id << " Message: " << message << std::endl;
    //}

    Application app;
    app.run();

    return 0;
}
