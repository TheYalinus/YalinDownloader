#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "DownloadLibrary/download_library.hpp"

int main(){
    std::string user_agent= "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.10 Safari/605.1.1";
    std::shared_ptr<DownloadLibrary::CurlRequest> a (new DownloadLibrary::CurlRequest("https://files.nexus-cdn.com/1704/2347/-Skyrim%20202X%2010.5.2%20-%20Landscape%20PART%202-2347-10-5-2-1753884060.rar?md5=j4HCEI-9KxczyZ29Zeb9gQ&expires=1786141885&user_id=205216541",{"ignore",""},"./test.html",false,user_agent));
    std::thread b([&a](){a->curlPerform();});
    while(true){
        std::cout<<a->downloaded()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(a->downloaded()>20000000){
            a->stop();
            break;
        }
    }
    b.join();
    return 0;
}
