// 
// peer.cpp
//

#include "settings.h"
#include "rtspserver.h"
#include "os.h"

void usage()
{
    printf("PPCast Peer Server, Copyright 2011-2014 Limlabs.\n"
           "Usage: pppeer [-h] [-v] [-s]\n"
           "\t -h  : help information\n"
           "\t -v  : version\n"
           "\t -s  : running as service (daemon)\n"
           );
}

void version()
{
    printf("PPCast Peer Server, Copyright 2011-2014 Limlabs.\n"
           "\t version : %s\n"
           "\t Build   : %s %s\n",
           PPCAST_PEER_VERSION_STRING,
           __DATE__, __TIME__
           );
}

void HatApp::menu()
{
    cout <<
	"\n"
	"Usage:\n"
	"\n"
	"select [where] \t\t:Query photos with SQL syntax\n"
	"score [id] \t\t:Score to find duplicated and like photos\n"
	"refresh [path] \t:Refresh metadata of photos in a directory\n"
	"exit \t\t:Exit the application\n"
	"? \t\t:Display the menu\n"
	"\n";
}

void cmd()
{
    // Execuate commands
    std::string cmd;
    do
    {
        std::cout << ">> ";
        getline(std::cin, cmd);
        if(cmd.size() >= 6 && cmd.substr(0, 6) == "select")
        {
            string where = cmd.substr(6);
            where.erase(where.find_last_not_of(' ')+1); 
            where.erase(0,where.find_first_not_of(' ')); 
            
            hat::FilterPrx filter = hat::FilterPrx::checkedCast(communicator()->propertyToProxy("Filter.Proxy"));
            hat::FileInfoSeq files = filter->select(where);
            int i=0;
            for (hat::FileInfoSeq::iterator p = files.begin(); p != files.end(); ++p) 
            {
                cout << ++i << ":\t" << p->id << "\t" << p->uri << "\t" << p->size << endl;
            }
            filter = NULL;           
        }
        else if(cmd.size() >= 5 && cmd.substr(0, 5) == "score")
        {
            string id = cmd.substr(5);
            id.erase(id.find_last_not_of(' ')+1); 
            id.erase(0,id.find_first_not_of(' ')); 
            int nid = -1;
            if(!id.empty())
            {
                istringstream iss(id);
                iss >> nid;
            }
            
            hat::FilterPrx filter = hat::FilterPrx::checkedCast(communicator()->propertyToProxy("Filter.Proxy"));
            hat::FileInfoSeq files = filter->score(nid);
            int i=0;
            for (hat::FileInfoSeq::iterator p = files.begin(); p != files.end(); ++p) 
            {
                cout << ++i << ":\t" << p->id << "\t" << p->uri << "\t[" << p->score << "]"<< endl;
            }
            filter = NULL; 
        }
        else if(cmd.size() >= 7 && cmd.substr(0, 7) == "refresh")
        {
            string path = cmd.substr(7);
            path.erase(path.find_last_not_of(' ')+1); 
            path.erase(0,path.find_first_not_of(' ')); 
            
            string prefix = "/Users/Shared/Photos";
            if(((path.size() >= prefix.size()) &&  (path.substr(0, prefix.size()) != prefix))
               || ((path.size() > 0) && (path.size() < prefix.size())))
            {
                cout << "Wrong path: " << path << endl;
            }
            else
            {
                hat::SynchronizerPrx synchronizer = hat::SynchronizerPrx::checkedCast(communicator()->propertyToProxy("Synchronizer.Proxy"));
                synchronizer->refresh(path);
                synchronizer = NULL;
            }
        }
        else if(cmd == "exit")
        {
            // Nothing to do
        }
        else if(cmd == "?")
        {
            menu();
        }
        else
        {
            cout << "Unknown command: " << cmd << endl;
            menu();
        }
    }
    while(cin.good() && cmd != "exit");
}
    
// Signals
bool quit = false;
void sig(int v_sig)
{
    quit = true;
}

int run()
{
    
#ifdef _OS_LINUX
    // grow our pool of file descriptors to the max!
    struct rlimit rl;
    // set it to the absolute maximum that the operating system allows
    // have to be superuser to do this
    rl.rlim_cur = 8192;
    rl.rlim_max = 8192;
    setrlimit(RLIMIT_NOFILE, &rl);
    
    // Signals
	signal(SIGTERM,sig); // Termination signal
	signal(SIGUSR1,sig); // User-defined signal 1
	signal(SIGUSR2,sig); // User-defined signal 2
    
	// Ignore signals
	signal(SIGTTOU,SIG_IGN); //Terminal output for background process
	signal(SIGTTIN,SIG_IGN); //Terminal input for background process
	signal(SIGTSTP,SIG_IGN); //Stop typed at terminal
	signal(SIGPIPE,SIG_IGN); //Broken pipe: write to pipe with no readers
#endif
    
    // Load settings
    theSettings.Load();
    
    // Init socket
    OSInitialSocket();
    
    // Start rtsp server
    if(theRtspServer.Start() == false)
    {
        return -1;
    }
    
    // Signal
    signal(SIGINT,sig); // Interrupt from keyboard
    
    // Wait for terminate
    while(quit == false)
    {
        cmd();
    }
    
    // Exit
    theRtspServer.Stop();
    OSUninitialSocket();
    
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i],"-h") == 0) // help
            {
                usage();
                return 0;
            }
            else if (strcmp(argv[i],"-v") == 0) // version
            {
                version();
                return 0;
            }
            else if (strcmp(argv[i],"-s") == 0) // running as service/daemon
            {
#ifdef _OS_LINUX
                // Keep working dir
                // Redirect input/output/error to /dev/null.
                if(daemon(1,0) != 0)
                {	
                    printf("Change to daemon failed\n");
                    return -1;
                }
#endif
            }
        }
    }

    version();
    return run();
}
