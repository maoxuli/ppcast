// 
// peer.cpp
//

#include "settings.h"
#include "rtspserver.h"
#include "channelmgr.h"
#include "os.h"

void usage()
{
    printf("PPCast Peer Server, Copyright 2011-2014 Limlabs.\n"
           "Usage: pppeer [-h] [-v] [-s]\n"
           "-h  : help information\n"
           "-v  : version\n"
           "-s  : running as service (daemon)\n"
           );
}

void version()
{
    printf("PPCast Peer Server, Copyright 2011-2014 Limlabs.\n"
           "Version : %s\n"
           "Build   : %s %s\n",
           PPCAST_PEER_VERSION_STRING,
           __DATE__, __TIME__
           );
}

void menu()
{
    std::cout <<
    "Commands:\n"
	"start url  : Start a channel with given url\n"
	"stop  cid  : Stop a channel with given channel id\n"
	"list       : List all channels\n"
	"exit       : Exit the application\n"
    "help       : Display the menu\n"
	"?          : Display the menu\n";
}

void exec_cmd()
{
    // Display menu
    menu();
    
    // Execuate commands
    std::string cmd;
    while (true)
    {
        std::cout << ">> ";
        getline(std::cin, cmd);
        if(cmd.size() >= 5 && cmd.substr(0, 5) == "start")
        {
            std::string url = cmd.substr(5);
            url.erase(url.find_last_not_of(' ') + 1); 
            url.erase(0, url.find_first_not_of(' ')); 
            
            if(url.empty())
            {
                menu();
                continue;
            }
            
            theChannelMgr.StartChannel(url);
        }
        else if(cmd.size() >= 4 && cmd.substr(0, 4) == "stop")
        {
            std::string cid = cmd.substr(4);
            cid.erase(cid.find_last_not_of(' ') + 1); 
            cid.erase(0, cid.find_first_not_of(' ')); 
            
            if(cid.empty())
            {
                menu();
                continue;
            }
            
            theChannelMgr.StopChannel(cid);
        }
        else if(cmd.size() >= 4 && cmd.substr(0, 4) == "list")
        {
            std::vector<std::string> channels = theChannelMgr.GetChannelList();
            for(std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
            {
                std::string media = *it;
                std::cout << media << "\n";
            }
        }
        else if(cmd.size() >= 4 && cmd.substr(0, 4) == "exit")
        {
            break;
        }
        else if(cmd.size() >= 4 && cmd.substr(0, 4) == "help")
        {
            menu();
        }
        else if(cmd.size() >= 1 && cmd.substr(0, 1) == "?")
        {
            menu();
        }
        else
        {
            std::cout << "Unknown command: " << cmd << std::endl;
            menu();
        }
    }
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
	signal(SIGTERM,SIG_IGN); // Termination signal
	signal(SIGUSR1,SIG_IGN); // User-defined signal 1
	signal(SIGUSR2,SIG_IGN); // User-defined signal 2
    
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
    signal(SIGINT,SIG_IGN); // Interrupt from keyboard
    
    // Wait for exit
    exec_cmd();
    
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
