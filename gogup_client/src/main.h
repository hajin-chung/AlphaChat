// init: 
//      1. initialize file descriptor cmd_fd(stdin)
//         initialize multicast socket for recv
//      2. recieve server info from multicast socket
//         close socket
//      3. initialize sockets: HeartBeat(UDP), REQ(TCP)
void init(char* argv[]);
void handle_cmd();
void handle_res();
void send_heartbeat();
void req_register();
void print_help();
