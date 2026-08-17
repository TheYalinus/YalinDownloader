#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "DownloadLibrary/download_library.hpp"

int main(){
    std::string user_agent= "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.10 Safari/605.1.1";

    /*std::shared_ptr<DownloadLibrary::CurlRequest> a (new DownloadLibrary::CurlRequest("https://files.nexus-cdn.com/1704/2347/-Skyrim%20202X%2010.5.2%20-%20Architecture%20PART%201-2347-10-5-2-1753882692.rar?md5=ceDu4SbopD77kM0W_XcvQw&expires=1786188275&user_id=205216541","./test.html",false,user_agent));
    std::thread b([&a](){a->curlPerform();});
    while(true){
        std::cout<<a->downloaded()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(a->downloaded()>20000000){
            a->stop();
            break;
        }
    }
    b.join();*/
    DownloadLibrary::DownloadTask a("https://upload.wikimedia.org/wikipedia/commons/thumb/0/0d/Onur_An%C4%B1t%C4%B1.JPG/250px-Onur_An%C4%B1t%C4%B1.JPG?utm_source=tr.wikipedia.org&utm_campaign=parser&utm_content=thumbnail","/home/cAg/test/",4,user_agent);
    /*DownloadLibrary::DownloadTask b("/home/cAg/test/",user_agent);*/
    auto parts = a.get_parts();
    for (auto n : parts){
        n.req->curlPerform();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    a.assemble();

    return 0;
}
