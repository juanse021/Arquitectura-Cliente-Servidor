#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <glob.h>
#include <fstream>

using namespace std;
using namespace zmqpp;



// Split all available songs
vector<string> split(string s, char del){
    vector<string> v;
    string nameSong = "";

    for (int i = 0; i < int(s.size()); i++) {
        if (s[i] != del)
            nameSong += s[i];

        else{
            v.push_back(nameSong);
            nameSong = "";
        }
    }
    
    if (nameSong != "")
        v.push_back(nameSong);

    return v;
}

// Load all files from directory
unordered_map<string,string> fromDirectory(const string& path){
    glob_t glob_result;
    glob(path.c_str(), GLOB_TILDE, NULL, &glob_result);

    vector<string> files;

    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
        files.push_back(string(glob_result.gl_pathv[i]));

    globfree(&glob_result);
    unordered_map<string,string> Result;

    for (int i = 0; i < int(files.size()); i++){
        vector<string> vec = split(files[i], '.');

        if (vec[1] == "ogg"){
            vec=split(vec[0], '/');
            Result[vec[1]] = files[i];
        }
    }

    return Result;
}

// Read file in bytes
vector<char> readFileToBytes(const string& fileName) {
    ifstream ifs(fileName, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	vector<char> result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(result.data(), pos);

	return result;
}

// Send bytes in a message
void fileToMessage(const string& fileName, message& msg) {
	vector<char> bytes = readFileToBytes(fileName);
	msg.add_raw(bytes.data(), bytes.size());
}

int main(int argc, char** argv) {

    context ctx;
    socket s(ctx, socket_type::rep);
    s.bind("tcp://*:5555");

    string path(argv[1]);
    unordered_map<string,string> songs = fromDirectory(path + "*");
    

    cout << "Start serving requests!" << endl;

    while(true) {

        // Send message
        message m;
        s.receive(m);
        string option;
        m >> option;

        // Receive answer from client
        message answer;

        cout << "Option:  " << option << endl;


        // List all songs in server
        if (option == "list") { 
            answer << "list" << songs.size();
            for (const auto& p : songs)
                answer << p.first;
        }


        // Send bytes to message
        else if (option == "play") {
            string songName;
            m >> songName;
            cout << songName << endl;
            answer << "file";
            fileToMessage(songs[songName], answer);
        }

        else {
            cout << "Invalid operation requested!!\n";
        }

        s.send(answer);
    }

    cout << "Finished\n";
    return 0;
}