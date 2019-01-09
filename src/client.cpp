/*Dannyho parser*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <boost/regex.h>
#include <iostream>
#include <ctime>
#include <vector>

#define BUFSIZE 1024

int getSize(std::string buffer){
    int spaces = 0;
    int index = 0;
    std::string tmp("");
    while (spaces != 3){
        if(buffer[index] == '\n'){
            spaces++;
            index++;
        }
        else{
            index++;
        }
    }
    while (buffer[index] != ':'){
        index++;
    }
    index++;
    index++;
    while(buffer[index] != '\n'){
        tmp = tmp + buffer[index];
        index++;
    }
    return atoi(tmp.c_str());


}

void getError(std::string& buffer, std::string& error){
    int index = 0;
    int space = 0;
    while(space != 3){
        if(buffer[index++] == '\n'){
            space++;
        }
    }

    while(buffer[index] != '\r'){
        error += buffer[index];
        index++;
    }




}

int errorCodeParser(std::string& header){
    int index = 0;
    std::string tmp("");
    while (header[index] != ' '){
        index ++;
    }
    index++;
    while(header[index] != ' '){
        tmp += header[index];
        index ++;
    }

    return atoi(tmp.c_str());
}



int main(int argc, char *argv[])
{
    bool GET;
    bool PUT;
    bool DELETE;
    int index = 0;
    int size;

    if(argc != 3 && argc != 4){
        fprintf(stderr, "ZLE ZADANE ARGUMENTY");
        return -1;
    }
    std::string command(argv[1]);
    std::string path(argv[2]);
    std::string IP("");
    std::string port("");
    std::string user("");
    std::string remotePath("");
    std::string localPath("");
    std::string type("");
    std::string files("");
    if(argc == 4){
        localPath = argv[3];
    }

    if(command == "get" || command == "lst"){
        GET = true;
    }

    if(command == "put" || command == "mkd"){
        PUT = true;
    }

    if(command == "del" || command == "rmd"){
        DELETE = true;
    }
	
    if(command == "mkd" || command == "rmd" || command == "lst"){
        type = "folder";
    }
    else{
        type = "file";
    }

    if(command == "put" && argc != 4 && type == "file"){
        fprintf(stderr, "ZLE ZADANE ARGUMENTY");
        return -1;
    }

    if(command == "put" && argc != 4 && type == "file"){
        fprintf(stderr, "ZLE ZADANE ARGUMENTY");
        return -1;
    }

    if(command != "put" && argc > 3){
        fprintf(stderr, "ZLE ZADANE ARGUMENTY");
        return -1;
    }

    if(path[5] != '/' && path[6] != '/'){
        printf("ZLE ZADANE ARGUMENTY");
        return -1;
    }


    for(index = 7; path[index] != ':'; index++){
        IP = IP + path[index];
        if(index >= 50){
            printf("ZLE ZADANE ARGUMENTY");
            return -1;
        }
    }
    index ++;
    for(; path[index] != '/'; index++){
        port = port + path[index];
        if(index >= 50){
            printf("ZLE ZADANE ARGUMENTY");
            return -1;
        }
    }
    index ++;
    for(; path[index] != '/' && path[index] != '\0'; index++){
        user = user + path[index];
        if(index >= 50){
            printf("ZLE ZADANE ARGUMENTY");
            return -1;
        }
    }
    index ++;
    while(path[index] != '\0'){
        remotePath = remotePath + path[index];
        if(index >= 500){
            printf("ZLE ZADANE ARGUMENTY");
            return -1;
        }
    index++;
    }

    if(GET && type == "file"){
        int size= remotePath.size();
        while(remotePath[size] != '/' && size != 0){size--;}
        if(size != 0){
            size++;
        }
        while(size < remotePath.size()){
            files += remotePath[size++];
        }


    }
    
    std::string answer("");
    std::string timeStr("");
    time_t t = time(nullptr);
    tm *timePtr = localtime(&t);

    if(timePtr->tm_hour < 10){
        timeStr = timeStr + "0" + std::to_string(timePtr->tm_hour) + ":";
    }
    else{
        timeStr = timeStr + std::to_string(timePtr->tm_hour) + ":";
    }
    if(timePtr->tm_min < 10){
        timeStr = timeStr + "0" + std::to_string(timePtr->tm_min) + ":";
    }
    else{
        timeStr = timeStr + std::to_string(timePtr->tm_min) + ":";
    }
    if(timePtr->tm_sec < 10){
        timeStr = timeStr + "0" + std::to_string(timePtr->tm_sec) ;
    }
    else{
        timeStr = timeStr + std::to_string(timePtr->tm_sec) ;
    }
    std::vector<std::string> days = {"Sun", "Mon", "Tue", "Wed","Thu", "Fri","Sat"};
    std::vector<std::string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    if(GET){
        answer = answer + "GET /";
    }
    else if(PUT){
        answer = answer + "PUT /";
    }
    else{
        answer = answer + "DELETE /";
    }

    answer = answer + user + "/" + remotePath + "?" + "type=" + type + " HTTP/1.1\r\n";
    answer = answer + "Host: " + IP + "\r\n";
    answer = answer + "Date: " + days[timePtr->tm_wday] +", " + std::to_string(timePtr->tm_mday) + " " + months[timePtr->tm_mon - 1] + " " + std::to_string(timePtr->tm_year + 1900) + " " + timeStr + " CET" + "\r\n";
    answer = answer + "Accept: text/plain\r\n";
    answer = answer + "Accept-Encoding: identity\r\n";
    if(PUT && argc == 4){
        FILE * file;


        file = fopen(argv[3], "rb");

        if(file == NULL){
            fprintf(stderr, "File not found.");
            return 404;
        }

        fseek(file, 0, SEEK_END);
        size = ftell(file);
        answer = answer + "Content-Type: application/octet-stream\r\n";
        answer = answer + "Content-Length: " + std::to_string(size) + "\r\n";

        fclose(file);
    }
    answer = answer + "\r\n";

    //std::cout << answer << "\n";
    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    int client_socket, port_number, bytestx, bytesrx;
    socklen_t serverlen;
    const char *server_hostname;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[BUFSIZE];
    bool parsed = false;

    server_hostname = IP.c_str();
    port_number = atoi(port.c_str());

    if ((server = gethostbyname(server_hostname)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);



    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
            exit(EXIT_FAILURE);
    }

    bzero(buf, BUFSIZE);

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
    {
                exit(EXIT_FAILURE);
    }

    bytestx = send(client_socket, answer.c_str(), strlen(answer.c_str()), 0);
    int read = 0;
    int readFile = 0;
    char buffer[512];
    bzero(buffer, 512);
    int retValue = 200;
    int res;
    std::string tempo("");
    std::string error("");
    if(PUT && argc == 4){
        FILE *file;
        file = fopen(localPath.c_str(), "r");
        if(file == NULL){
            fclose(file);
            fprintf(stderr,"File not found.");
            return 400;
        }
        while(size > 0){
            if(size > 512){
                readFile = fread(buffer, 1, 512, file);
            }
            else{
                readFile = fread(buffer, 1, size, file);
            }
            send(client_socket, buffer,readFile,0);
            bzero(buffer, 512);
            size -= readFile;
        }
        fclose(file);
    }

    if(GET && type == "file"){
        std::string buferino("");
        bzero(buffer, 512);
        int res = 0;
        for (;;)
        {
                res = recv(client_socket, buffer, 1,0);
                buferino = buferino + buffer;
                if (res <= 0)
                    break;

                if(buffer[0] == '\r'){
                    res = recv(client_socket, buffer, 1,0);
                    buferino = buferino + buffer;
                    if(buffer[0] == '\n'){
                        res = recv(client_socket, buffer, 1,0);
                        buferino = buferino + buffer;
                        if(buffer[0] == '\r'){
                            res = recv(client_socket, buffer, 1,0);
                            buferino = buferino + buffer;
                            if(buffer[0] == '\n'){
                                break;
                            }

                        }
                    }
                }
        }
        bzero(buffer, 512);
        int size = 0;
        retValue = errorCodeParser(buferino);
        if(retValue != 200){
            getError(buferino , error);
        }

        if(retValue == 200){
            size = getSize(buferino);
        }
        else{
            size = 0;
        }
        parsed = true;
        FILE *test;
        test = fopen(files.c_str(), "w");
        if(size > 0){
            while(size > 0){
                if(size >= 512){
                    res = recv(client_socket, buffer, 512,0);
                }
                else{
                    res = recv(client_socket, buffer, size,0);
                }
                fwrite(buffer, 1, res, test);
                bzero(buffer, 512);
                size -= res;
            }

            fclose(test);
        }

    }

    if(GET && type == "folder"){
        std::string buferino("");
        bzero(buffer, 512);
        int res = 0;
        for (;;)
        {
                res = recv(client_socket, buffer, 1,0);
                buferino = buferino + buffer;
                if (res <= 0)
                    break;

                if(buffer[0] == '\r'){
                    res = recv(client_socket, buffer, 1,0);
                    buferino = buferino + buffer;
                    if(buffer[0] == '\n'){
                        res = recv(client_socket, buffer, 1,0);
                        buferino = buferino + buffer;
                        if(buffer[0] == '\r'){
                            res = recv(client_socket, buffer, 1,0);
                            buferino = buferino + buffer;
                            if(buffer[0] == '\n'){
                                break;
                            }

                        }
                    }
                }
        }
        int size = 0;
        retValue = errorCodeParser(buferino);
        if(retValue != 200){
            getError(tempo , error);
        }
        if(retValue == 200){
            size = getSize(buferino);
        }
        else{
            size = 0;
        }
        parsed = true;
        bzero(buffer, 512);
        std::string list("");
        while(size > 0){
            if(size >= 512){
                res = recv(client_socket, buffer, 512,0);
            }
            else{
                res = recv(client_socket, buffer, size,0);
            }
            if (res <= 0)
                break;

            list += buffer;
            bzero(buffer, 512);
            size -= res;
        }
        std::cout << list;

    }

    /* prijeti odpovedi a jeji vypsani */

    if(!parsed){
        bzero(buffer, 512);
        bytesrx = recv(client_socket, buf, BUFSIZE, 0);
        tempo = buf;
        retValue = errorCodeParser(tempo);
        if(retValue != 200){
            getError(tempo , error);
        }
    }
    if(retValue != 200){
        fprintf(stderr, error.c_str());
    }
    close(client_socket);
    return retValue;
	
	
	
}
