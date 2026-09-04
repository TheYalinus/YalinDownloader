#ifndef PARALLEL_DOWNLOADER_HPP
#define PARALLEL_DOWNLOADER_HPP
#include "download_library.hpp"
namespace DownloadLibrary {
    class ParallelDownloader{
        public:
            ParallelDownloader(ReqsType reqs);
            long total_download();
            ThreadsType get_threads();
            void join_threads();
        private:
            ReqsType reqs;
            ThreadsType threads;
            std::vector<std::atomic<long> *> progress_handlers;
    };
}
#endif
