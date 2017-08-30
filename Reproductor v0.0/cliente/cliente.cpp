#include <string>
#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <queue>
#include <thread>
#include <iostream>
#include <SFML/Audio.hpp>
#include <fstream>

using namespace std;
using namespace zmqpp;
using namespace sf;

void messageToFile(const message &msg, const string &fileName) {
	Music music;
    const void *data;
    msg.get(&data, 1);
    size_t size = msg.size(1);

    ofstream ofs(fileName, ios::binary);
    ofs.write((char *)data, size);
}

void threadOperation(Music *music, queue<string> &playList, string &operation, socket &s){
	bool player = false;
	
	while (true) {
		if (operation == "play"){
			player = true;
			message n;
			n << "play";
        	n << playList.front();
        	playList.pop();
	        s.send(n);

	        // Receive operation to do
	        message answer;
	        s.receive(answer);
            string debug;
            answer >> debug;
            cout << debug << endl;   
	        messageToFile(answer, "song.ogg"); 
            music->openFromFile("song.ogg");
            music->play();
            operation = "none";
		}
		if (music->getStatus() != SoundSource::Playing && player){
			message n;
			n << "play";
        	n << playList.front();
        	playList.pop();
	        s.send(n);

	        // Receive operation to do
	        message answer;
	        s.receive(answer);
            string debug;
            answer >> debug;
            cout << debug << endl;   
	        messageToFile(answer, "song.ogg"); 
            music->openFromFile("song.ogg");
            music->play();
		}
	}

}

int main() {

    cout << "This is the client" << endl;
    context ctx;
    socket s(ctx, socket_type::req);
    cout << "Connecting to tcp port 5555" << endl;
    s.connect("tcp://localhost:5555");
    queue<string> playList;
    Music music;
    string operation;
    std::thread t1(threadOperation, &music, ref(playList), ref(operation), ref(s));

    bool player = true;

    while (true) {

        cout << "Option? " << endl;
        string option;
        cin >> option;

        message n;
        n << option;
        
        if (option == "list"){
        	s.send(n);
	        // Receive operation to do
	        message answer;
	        s.receive(answer);
	        size_t numSongs;
	        string songName;
	        answer >> numSongs;
	        cout << "Available songs: " << numSongs << endl;
	        for (int i = 0; i < int(numSongs); i++) {
                answer >> songName;
                cout << (i+1) << ":\t" << songName << endl;
            }
	    }
	    else if (option == "add") {
		    string nameSong;
		    cout << "nameSong" << endl;
		    cin >> nameSong;
		    n << nameSong;
		    s.send(n);
		    message answer;
		    s.receive(answer);
		    string Available;
		    answer >> Available;
		    
		    if(Available == "yes") {
			    cout<<"add song..."<<endl;
			    playList.push(nameSong);
		    }
		    
		    else {
			    cout<<"the song isn't Available"<<endl;
		    }

		}
		else if (option == "play" && player){
		    player = false;
			operation = "play";
		}
		else if (option == "next"){
			operation = "play";
		}
        
    }
    return 0;
}
