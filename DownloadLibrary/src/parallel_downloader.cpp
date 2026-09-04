#include "download_library.hpp"
#include "parallel_downloader.hpp"
#include "curl_request.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <numeric>
#include <thread>
#include <vector>
DownloadLibrary::ParallelDownloader::ParallelDownloader(DownloadLibrary::ReqsType reqs):
reqs(reqs),threads(std::vector<std::shared_ptr<std::thread>>()),progress_handlers(std::vector<std::atomic<long> *>()){
    for(auto n: reqs){
        this->threads.push_back(std::make_shared<std::thread>(std::thread([n](){n->curlPerform();})));
        this->progress_handlers.push_back(n->pGetDownloaded());
    }

}
DownloadLibrary::ThreadsType DownloadLibrary::ParallelDownloader::get_threads(){
    return this->threads;
}
long DownloadLibrary::ParallelDownloader::total_download(){
    long total=0;
    for(std::vector<std::atomic<long> *>::iterator itr =this->progress_handlers.begin() ;itr  != this->progress_handlers.end() ; itr++ ){
        total +=  **itr;
    }
    return total;
}
void DownloadLibrary::ParallelDownloader::join_threads(){
    for(auto &n : this->threads){
        n->join();
    }
}
