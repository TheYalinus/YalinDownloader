#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include "DownloadLibrary/download_library.hpp"

int main(){
    std::string user_agent= "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/134.0.0.0 Safari/537.36 Trailer/93.3.8652.5";

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

    //DownloadLibrary::DownloadTaskMultiple b("https://fsn1-speed.hetzner.com/100MB.bin","/home/cAg/test/",1,user_agent);
    //DownloadLibrary::DownloadTaskMultiple b("/home/cAg/test",user_agent);
    /*std::vector<std::thread> threads;
    auto parts = a.get_parts();
    for (auto n : parts){
        threads.push_back(std::thread([&n](){n.req->curlPerform();}));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    for(auto &n: threads){
        n.join();
        }
        */
    /*for(auto n : parts)
    {
        n.req->curlPerform();
        }*/
    //a.assemble();
    //DownloadLibrary::DownloadTask *b = DownloadLibrary::DownloadFactory::createTask("https://fsn1-speed.hetzner.com/100MB.bin", "/home/cAg/test", 4,user_agent);
    DownloadLibrary::DownloadTask *b = DownloadLibrary::DownloadFactory::createTask("/home/cAg/test",user_agent);
    //function to give vectors of downloader threads
    // automatic decide part count
    std::vector<std::thread> threads;

    std::cout<<"test"<<std::endl;
    DownloadLibrary::ReqsType req = b->get_requests();
    for(auto n: req){
        threads.push_back(std::thread([&n](){n->curlPerform();}));
        std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    for(auto &n: threads){
            n.join();
            }

        b->assemble();




    //delete a;
    return 0;
}
