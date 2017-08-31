#include <glob.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

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

vector<char> readFileToBytes(const string& fileName) {
    ifstream ifs(fileName, ios::binary | ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    vector<char> result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(result.data(), pos);

    return result;
}
int fileToMessage(const string& fileName, message& msg, int parte) {
    vector<char> bytes = readFileToBytes(fileName);
    vector<char> v;
    int inicio = (512*1024)*parte;
    int fin = inicio + (512*1024);
    for(int i=inicio;i<fin;i++){
        if(i>=bytes.size()){
            bytes=v;
            msg.add_raw(bytes.data(), bytes.size());
            return 1;
        }
        else{
            v.push_back(bytes[i]);
        }
    }
    bytes=v;
    msg.add_raw(bytes.data(), bytes.size());
    return 0;
}

int main() {

	context ctx;
    socket s(ctx, socket_type::rep);
    s.bind("tcp://*:5555");

	//string path("music/*");
    unordered_map<string,string> songs = fromDirectory("music/*");

    cout << "Start serving requests!" << endl;
    int parte;
    while(true){
	    // Send message
	    message m;
	    s.receive(m);
	    string option;
	    m >> option;

	    // Receive answer from client
	    message answer;
	    
	    if (option == "list"){
	    	answer<<songs.size();  
		    for (const auto& p : songs)
		        answer<<p.first;
		    s.send(answer);
		}
				
		else if (option == "add"){
			string nameSong;
			m >> nameSong;
			auto search = songs.find(nameSong);  
	        if (search != songs.end()){
	            answer << "yes";
	        	s.send(answer);
	        }
	        else{
	            answer << "not";
	        	s.send(answer);
	        }
	    }
	    else if (option == "play"){
	    	message answer;
            string songName;
            m >> songName;
            m >> parte;
            cout << songName << endl;
            int fin;
            fin = fileToMessage(songs[songName], answer,parte);
            answer<<fin;
            cout<<"fin"<<fin<<endl;
            s.send(answer);     
	    }
	}

	return 0;
}
