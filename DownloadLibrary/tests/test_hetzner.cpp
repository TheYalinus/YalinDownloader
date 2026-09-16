#include "download_task.hpp"
#include "download_factory.hpp"
#include "parallel_downloader.hpp"
#include <chrono>
#include <filesystem>
#include <functional>
#include <thread>
#define  FILESIZE 104857600
int main(){
    std::string user_agent="Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/134.0.0.0 Safari/537.3";
    DownloadLibrary::DownloadTask * task= DownloadLibrary::DownloadFactory::createTask("https://fsn1-speed.hetzner.com/100MB.bin","/home/cAg/test",4,3, user_agent);
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        DownloadLibrary::ParallelDownloader downloader(task->get_requests());
        downloader.join_threads();
        if(downloader.total_download() != FILESIZE)
            return 1;
        task->assemble();
        if(std::filesystem::file_size(task->getTaskPath() / task->getFinalFileName()) != FILESIZE)
            return 1;
    }


    delete task;

}
