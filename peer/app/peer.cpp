// 
// peer.cpp
//

#include "settings.h"
#include "channelmgr.h"
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
    
    // Initialize peer 
    if(theChannelMgr.Initialize() == false)
    {
        return -1;
    }
    
    // Signal
    signal(SIGINT,sig); // Interrupt from keyboard
    
    // Wait for terminate
    while(quit == false)
    {
        OSSleep(1000);
    }
    
    // Exit
    theChannelMgr.Shutdown();
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
