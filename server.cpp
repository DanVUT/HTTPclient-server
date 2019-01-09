/*
 * IPK.2015L
 *
 * Demonstration of trivial TCP communication.
 *
 * Ondrej Rysavy (rysavy@fit.vutbr.cz)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <boost/regex.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <string.h>
#include <errno.h>

void parse(std::string& buffer, std::string& command, std::string& user, std::string& path, bool& file, int& content){



    int index = 0;
    while(buffer[index] != ' '){
        command = command + buffer[index];
        index++;
    }
    index++;
    index++;
    while(buffer[index] != '/'){
        user = user + buffer[index];
        index++;
    }

    index++;
    while(buffer[index] != '?'){
        path = path + buffer[index];
        index++;
    }
    for(int i = 0; i<7; i++){
        index++;
    }
    if(buffer[index] == 'i'){
        file = true;
    }

    if(file){
        int newLines=0;
        std::string tmp("");
        while(newLines != 6){
            if(buffer[index] == '\n')
                newLines++;
            index++;
        }
        while(buffer[index++] != ':'){}
        index++;
        while(buffer[index] != '\r'){
            tmp = tmp + buffer[index];
            index ++;
        }
        content = atoi(tmp.c_str());
    }

}

void createHeader(std::string& header, bool& not_found, bool& bad_request, std::string& command, int size, std::string& errorMessage){
    std::string answer("");
    std::string timeStr("");
    std::string error("");

    if(not_found){
        error = "404 Not Found";
    }
    else if(bad_request){
        error = "400 Bad Request";
    }
    else{
        error = "200 OK";
    }


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


    header = header + "HTTP/1.1 " + error + "\r\n";
    header = header + "Date: " + days[timePtr->tm_wday] +", " + std::to_string(timePtr->tm_mday) + " " + months[timePtr->tm_mon - 1] + " " + std::to_string(timePtr->tm_year + 1900) + " " + timeStr + " CET" + "\r\n";
    header = header + "Content-Type: application/octet-stream\r\n";
    if(command == "GET" && size > 0){
        header = header + "Content-Length: " + std::to_string(size) + "\r\n";
        header = header + "Content-Encoding: identity\r\n";
    }
    if((not_found || bad_request) && errorMessage != ""){
        header = header + errorMessage + "\r\n";
    }
    header = header + "\r\n";

}


int main (int argc, const char * argv[]) {
    int rc;
    int welcome_socket;
    struct sockaddr_in6 sa;
    struct sockaddr_in6 sa_client;
    char str[INET6_ADDRSTRLEN];
    int port_number = 0;
    std::string buffer("");

    if (argc != 1 && argc != 3 && argc != 5) {
       fprintf(stderr,"usage: %s <-r ROOT-FOLDER><-p PORT>\n", argv[0]);
       exit(EXIT_FAILURE);
    }
    std::string argument("");
    std::string root("");
    for(int i = 1; i < argc; i++){
        argument = argv[i];
        if(argument == "-p"){
            i++;
            argument = argv[i];
            port_number = atoi(argument.c_str());
        }
        else if(argument == "-r"){
            i++;
            root = argv[i];
        }
        else{
            fprintf(stderr,"usage: %s <-r ROOT-FOLDER><-p PORT>\n", argv[0]);
            exit(EXIT_FAILURE);
        }


    }
    if(root != ""){
        if(root[root.size() - 1] != '/'){
            root += "/";
        }
    }
    if(port_number == 0){
        port_number = 6677;
    }

    
    
	socklen_t sa_client_len=sizeof(sa_client);
	if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_addr = in6addr_any;
	sa.sin6_port = htons(port_number);	
        
    
    
	if ((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0)
	{
                exit(EXIT_FAILURE);
	}
	if ((listen(welcome_socket, 1)) < 0)
	{
                exit(EXIT_FAILURE);
	}
	while(1)
	{
		int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);		
		if (comm_socket > 0)
		{
                        if(inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {
                        }
			
                        char buff[512];
                        bzero(buff, 512);
                        int res = 0;
                        int count = 0;
			for (;;)		
			{	
                                res = recv(comm_socket, buff, 1,0);
                                if (res <= 0)
                                    break;
                                buffer = buffer + buff;
                                if(buff[0] == '\r'){
                                    res = recv(comm_socket, buff, 1,0);
                                    buffer = buffer + buff[0];
                                    if(buff[0] == '\n'){
                                        res = recv(comm_socket, buff, 1,0);
                                        buffer = buffer + buff;
                                        if(buff[0] == '\r'){
                                            res = recv(comm_socket, buff, 1,0);
                                            buffer = buffer + buff;
                                            if(buff[0] == '\n'){
                                                break;
                                            }

                                        }
                                    }
                                }
                        }

                        std::string command("");
                        std::string user("");
                        std::string path("");
                        std::string header("");
                        std::string error("");
                        bool file = false;
                        bool not_found = false;
                        bool bad_request = false;
                        int size = -1;
                        parse(buffer, command, user, path, file, size);


                        int accepted = 0;
                        if(size > -1 && command == "PUT"){
                            FILE *files;
                            path = root + user + "/" +path;
                            files = fopen(path.c_str(), "r");
                            if(files != NULL){
                                error = "Already exists.";
                                bad_request = true;
                                fclose(files);
                                while(size > 0){
                                    if(size > 512){
                                        res = recv(comm_socket, buff, 512,0);
                                    }
                                    else{
                                        res = recv(comm_socket, buff, size,0);
                                    }
                                    if (res <= 0)
                                        break;
                                    bzero(buff, 512);
                                    size -= res;

                                }
                            }
                            else{
                                files = fopen(path.c_str(), "w");
                                while(size > 0){
                                    if(size > 512){
                                        res = recv(comm_socket, buff, 512,0);
                                    }
                                    else{
                                        res = recv(comm_socket, buff, size,0);
                                    }
                                    if (res <= 0)
                                        break;
                                    fwrite(buff, 1, res, files);
                                    bzero(buff, 512);
                                    size -= res;

                                }
                                fclose(files);
                            }
                            createHeader(header, not_found, bad_request, command, 0, error);
                            send(comm_socket, header.c_str(), strlen(header.c_str()), 0);
                        }

                        if(size == -1 && command == "PUT"){
                            DIR *test;
                            FILE *testerino;
                            path = root + user + "/" + path;
                            test = opendir(path.c_str());
                            testerino = fopen(path.c_str(), "r");

                            if (testerino != NULL){
                                error = "Already exists.";
                                bad_request = true;
                                fclose(testerino);
                            }
                            else if(test != NULL){
                                error = "Already exists.";
                                bad_request = true;
                                closedir(test);
                            }
                            else{
                                closedir(test);
                                path = "mkdir -p \"" + path + '\"';
                                file = popen(path.c_str(), "r");
                            }
                            createHeader(header, not_found, bad_request, command, 0, error);
                            send(comm_socket, header.c_str(), strlen(header.c_str()), 0);
                        }

                        if(command == "DELETE" && file){
                            DIR *test;
                            path = root + user + "/"+ path;
                            test = opendir(path.c_str());

                            if(test != NULL){
                                bad_request = true;
                                error = "Not a file.";
                                closedir(test);

                            }
                            else{
                                closedir(test);
                                FILE *testf;
                                testf = fopen(path.c_str(), "r");
                                if(testf == NULL){
                                    not_found = true;
                                    error = "File not found.";
                                }
                                else{
                                    fclose(testf);
                                    remove(path.c_str());
                                }
                            }
                            createHeader(header, not_found, bad_request, command, 0, error);
                            send(comm_socket, header.c_str(), strlen(header.c_str()), 0);
                        }

                        if(command == "DELETE" && !file){

                            int n=0;
                            if(user != "" && path == ""){
                                error = "Can not delete user directory.";
                                bad_request = true;
                            }
                            else{
                                path = root + user + "/"+ path;
                                DIR *test = opendir(path.c_str());
                                struct dirent *d;
                                if(test == NULL){
                                    closedir(test);
                                    not_found = true;
                                    error = "Directory not found.";

                                }
                                else{
                                    while ((d = readdir(test)) != NULL) {
                                        if(++n > 2)
                                          break;
                                    }
                                    closedir(test);
                                    if(n<=2){
                                        path = "rmdir \"" + path + '\"';
                                        system(path.c_str());
                                    }
                                    else{
                                        bad_request = true;
                                        error = "Directory not empty.";
                                    }
                                }
                            }
                            createHeader(header, not_found, bad_request, command, 0, error);
                            send(comm_socket, header.c_str(), strlen(header.c_str()), 0);


                        }
                        if(command == "GET" && file){
                                DIR *directoro;
                                path = root + user + "/" + path;
                                directoro = opendir(path.c_str());
                                if(directoro != NULL){
                                    bad_request = true;
                                    error = "Not a file.";
                                    closedir(directoro);
                                    createHeader(header, not_found, bad_request, command, -1, error);
                                    send(comm_socket, header.c_str(),strlen(header.c_str()),0);
                                }
                                else{

                                    closedir(directoro);
                                    FILE *files;
                                    files = fopen(path.c_str(), "r");

                                    int readFile = 0;
                                    char *buf[512];
                                    bzero(buf, 512);
                                    if(files == NULL){
                                        not_found = true;
                                        error = "File not found.";
                                        createHeader(header, not_found, bad_request, command, 0, error);
                                        send(comm_socket, header.c_str(),strlen(header.c_str()),0);
                                    }
                                    else{
                                        fseek(files, 0, SEEK_END);
                                        int size = ftell(files);
                                        createHeader(header, not_found, bad_request, command, size,error);
                                        send(comm_socket, header.c_str(),strlen(header.c_str()),0);
                                        int read = 0;

                                        fseek(files,0x0,SEEK_SET);
                                        bzero(buf, 512);

                                        while(read < size){
                                            readFile = fread(buf, 1, 512, files);
                                            send(comm_socket, buf,readFile,0);
                                            read += readFile;

                                        }

                                        fclose(files);
                                    }
                                }
                        }
                        if(command == "GET" && !file){
                                path = root + user + "/" + path;
                                std::string list("");
                                FILE *pipe;
                                char ls[512];
                                bzero(ls, 512);
                                path = "ls \"" + path + '\"';
                                pipe = popen(path.c_str(), "r");
                                while(fgets(ls, 512, pipe) != NULL){
                                    list += ls;
                                    bzero(ls, 512);
                                }
                                std::string header("");
                                createHeader(header, not_found, bad_request, command, list.size() , error);
                                send(comm_socket, header.c_str(),strlen(header.c_str()),0);

                                send(comm_socket, list.c_str(),strlen(list.c_str()),0);



                        }
                        buffer = "";
                        header = "";


		}
		else
		{
			printf(".");
		}

		close(comm_socket);
	}	
}
