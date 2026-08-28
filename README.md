# YalinDownloader
A Crossplatform, feature rich , fast download manager with parallel downloading and bulk media downloading. Written fully in C++. Powered by Libcurl.

#####TODO:
######Implement more features in DownloadLibrary:
-HTTP Authentication
-Proxy support
-Other protocols support like FTP

######Browser Extension(for now, i'll just write it for firefox)
-Media capturing
-Download everything in the page
-Bulk media downloading that can also managed by extension(it should work like right click->add this page to download list)
-Manage download tasks from your browser too
-JDOWNLOADER links support

######GUI
-It maybe GTK or QT, but i'm more likely to write it in QT, because of Multiplatform support, but writing code in QT like writing in a other language than C++.
-Everything can be managed from there.
-Simple gui like any other gui nothing to say too more.

######TUI
-Tui written by ncurses would be good
-Like gui nothing to say too more 

######Non-Mandatory Background Service
-On linux it will be a SYS-V style daemon, but i want to put OpenRC support too, i'll do it somehow
-On Windows, i dont know much about Windows programming, but i just know it should be a service.
-Without this service, browser extension can not work, also TUI and GUI will not be syncronised.
