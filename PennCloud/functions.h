/*
 * functions.h
 *
 *  Created on: Apr 11, 2018
 *      Author: cis505
 */

#include <chrono>

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#define panic(a...) do { fprintf(stderr, "\033[1;31m"); fprintf(stderr, a); fprintf(stderr, "\033[0m"); fprintf(stderr, "\n"); exit(1); } while (0)
#define logVerbose(a...) do { if (true) { printf("\033[0;31m"); printf(a); printf("\033[0m"); printf("\n"); } } while(0)
#define logSuccess(a...) do { if (true) { printf("\033[1;35m"); printf(a); printf("\033[0m"); printf("\n"); } } while(0)
#define logError(a...) do { if (true) { printf("\033[1;35m"); printf(a); printf("\033[0m"); printf("\n"); } } while(0)
#define log(a...) do { printf(a); printf("\n"); } while(0)
#define logReceive(a...) do { if (true) { printf("\033[0;34m[R] "); printf(a); printf("\033[0m"); printf("\n"); } } while(0)
#define logSend(a...) do { if (true) { printf("\033[0;32m[S] "); printf(a); printf("\033[0m"); printf("\n"); } } while(0)

std::string time() {

//	Returns the current time in microseconds

	using namespace std::chrono;
	std::stringstream time;
	time << duration_cast< microseconds >(system_clock::now().time_since_epoch()).count();
	return time.str();
}

void split(const std::string &s, const char* delim, std::vector<std::string> & v){
    char * dup = strdup(s.c_str());
    char * token = strtok(dup, delim);
    while(token != NULL){
        v.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
    free(dup);
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::string urlEncode(std::string str){
	std::string new_str = "";
    char c;
    int ic;
    const char* chars = str.c_str();
    char bufHex[10];
    int len = strlen(chars);

    for(int i=0;i<len;i++){
        c = chars[i];
        ic = c;
        // uncomment this if you want to encode spaces with +
        /*if (c==' ') new_str += '+';
        else */if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
        else {
            sprintf(bufHex,"%X",c);
            if(ic < 16)
                new_str += "%0";
            else
                new_str += "%";
            new_str += bufHex;
        }
    }
    return new_str;
 }

std::string urlDecode(std::string str){
    std::string ret;
    char ch;
    int i, ii, len = str.length();

    for (i=0; i < len; i++){
        if(str[i] != '%'){
            if(str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }else{
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    logVerbose("String decoded from %s to %s", str.c_str(), ret.c_str());
    return ret;
}

#endif /* FUNCTIONS_H_ */
