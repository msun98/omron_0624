#include "telnetclient.h"


telnetclient::telnetclient(QObject *parent) : QThread(parent)
{
    cout << "Hi telnet" << endl;
}

void telnetclient::run()
{
    RunTelnetClient();
}

int i=0;
void telnetclient::RunTelnetClient()
{
    //    while(true)
    //    {
    //        cout << "telnet" << endl;
    //        sleep(1);
    //    }


    // on Unix POXIS based systems, turn off line buffering of input, so cin.get() returns after every keypress
    // On other systems, you'll need to look for an equivalent
#ifdef POSIX
    termios stored_settings;
    tcgetattr(0, &stored_settings);
    termios new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ISIG); // don't automatically handle control-C
    tcsetattr(0, TCSANOW, &new_settings);
#endif

    std::string dest_ip;
    std::string dest_port;

    //    if (argc != 3)
    //    {
    //#ifdef WIN32
    //        dest_ip = "192.168.0.1";
    //        dest_port = "23";
    //#else
    //        std::cerr << "Usage: telnet <host> <port>\n";
    //        return 1;
    //#endif
    //    }
    //    else
    //    {
    //        dest_ip = argv[1];
    //        dest_port = argv[2];
    //    }

    dest_ip = "192.168.1.20";//"192.168.1.32";
    dest_port = "7171";

    try
    {
        std::cout << "SimpleTelnetClient is tring to connect " << dest_ip << ":" << dest_port << std::endl;
        boost::asio::io_service io_service;

        // resolve the host name and port number to an iterator that can be used to connect to the server
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(dest_ip, dest_port);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        // define an instance of the main class of this program

        AsioTelnetClient telnet_client(io_service, iterator);

        telnet_client.setReceivedSocketCallback([&](const std::string& message)
        {
            std::cout << "From server - " << message;
            serverMessage = message; //must use dont modify
        });

        telnet_client.setClosedSocketCallback([]()
        {
            std::cout << " # disconnected" << std::endl;
        });

        while (1)
        {

            //            char ch;
            //            std::cin.get(ch); // blocking wait for standard input

            //            if (ch == 3) // ctrl-C to end program
            //            {
            //                break;
            //            }

            //           telnet_client.write(ch);
            //           cout << ch << endl;
            char ch;
            if(flag == 1)
            {
                char a[sendCommand.length()];

                for(int i=0;i<sizeof(a);i++)
                {
                    a[i] = sendCommand[i];
                    telnet_client.write(a[i]);
                }

                ch = '\n';
                telnet_client.write(ch);

                flag = 0;
            }
            usleep(10*1000);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
#ifdef POSIX // restore default buffering of standard input
    tcsetattr(0, TCSANOW, &stored_settings);
#endif
}
