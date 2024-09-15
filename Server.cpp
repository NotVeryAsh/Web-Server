#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

SOCKET listenSocket = NULL;
SOCKET acceptSocket = NULL;

void handleError(const string& message, const int exitCode = 1)
{
    cout << message << WSAGetLastError() << endl << endl;
    if(socket != NULL)
    {
        closesocket(listenSocket);
    }

    if(acceptSocket != NULL)
    {
        closesocket(acceptSocket);
    }
    WSACleanup();
    exit(exitCode);
}

void handleExit()
{
    handleError("Closing server", 0);
}

string getMethod(vector<string> &segments)
{
    const string &line = segments[0];
    return line.substr(0, line.find(' '));
}

string getEndpoint(vector<string> &segments)
{
    string line = segments[0];
    line = line.substr(line.find('/'));
    return line.substr(0, line.find(' '));
}

void readHeaders(const string& buffer)
{
    stringstream stream(buffer);
    string body;
    vector<string> bodySegments;

    while(getline(stream, body, '\n')) {
        bodySegments.push_back(body);
    }

    const string method = getMethod(bodySegments);
    const string endpoint = getEndpoint(bodySegments);
    const string content = bodySegments[bodySegments.size() - 1];
}

int main()
{
    cout << "Attempting to create server: " << endl << endl;

    WSADATA wsaData;
    sockaddr_in server = {};
    int server_len;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        handleError("Could not initialize: ");
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSocket == INVALID_SOCKET)
    {
        handleError("Could not create socket: ");
    }

    const int port = 8080;
    const char *ipAddress = "127.0.0.1";

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipAddress);
    server.sin_port = htons(port);
    server_len = sizeof(server);

    if(bind(listenSocket, (SOCKADDR *)&server, server_len) == SOCKET_ERROR)
    {
        handleError("Could not bind socket: ");
    }

    if(listen(listenSocket, 20) == SOCKET_ERROR)
    {
        handleError("Could not start listening: ");
    }

    cout << "Listening on " << ipAddress << ":" << port << endl << endl;

    int bytes = 0;
    const int BUFFER_SIZE = 30720;
    while((acceptSocket = accept(listenSocket, (SOCKADDR *)&server, &server_len))) {

        if(acceptSocket == INVALID_SOCKET) {
            handleError("Could not accept: ");
        }

        char buff[BUFFER_SIZE] = {};
        bytes = recv(acceptSocket, buff, BUFFER_SIZE, 0);
        if(bytes < 0) {
            closesocket(acceptSocket);
            handleError("Could not read client request: ");
        }

        readHeaders(buff);

        string content = "<html><h1>Hello world</h1></html>";
        string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        response.append(to_string(content.size()));
        response.append("\n\n");
        response.append(content);

        int bytesSent = 0;
        bytesSent = send(acceptSocket, response.c_str(), (int) response.length(), 0);
        if(bytesSent == SOCKET_ERROR) {
            closesocket(acceptSocket);
            handleError("Could not send response: ");
        }

        cout << "Sent response to client" << endl << endl;
        closesocket(acceptSocket);
    }
}