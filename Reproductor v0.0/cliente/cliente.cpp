#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <fstream>
#include <iostream>
#include <SFML/Audio.hpp>
#include <zmqpp/zmqpp.hpp>
#include <condition_variable>

using namespace std;
using namespace zmqpp;
using namespace sf;

template <class T>

class SafeQueue {
    private:
	    queue<T> q;
	    std::mutex __mtx;
	    std::condition_variable cv;
    
    public:
	    SafeQueue() : q(), __mtx(), cv() {}
	    
	    void push(T element) {
		    std::lock_guard<std::mutex> lock(__mtx);
		    q.push(element);
		}

        T front() {
		    std::unique_lock<std::mutex> lock(__mtx);

		    while (q.empty())
			    cv.wait(lock);

            T element = q.front();
            cout << "Next song: " << element << endl;
            q.pop();
            return element;
		}

        bool empty() {
			return q.empty();
		}

		
/*		void update() {
		    for (int i = 0; i < q.size(); i++) {
		        cout << "[" << q.front() << "]" << endl;
		        q.pop();
		    }		    
		}
*/

		~SafeQueue() {};
	
};

void messageToFile(const message &msg, const string &fileName) {
    const void *data;
    msg.get(&data, 1);
    size_t size = msg.size(1);

    ofstream ofs(fileName, ios::binary);
    ofs.write((char *)data, size);
}

void threadOperation(Music *music, SafeQueue<string> &playList, string &operation, socket &s){
    bool player = false;
    
	while (true) {   
	 
		if (operation == "play"){
			player = true;
			message n;
			n << "play";
        	n << playList.front();
        	if (playList.empty()){
        		player=false;
        	}
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
        	if (playList.empty()){
        		player = false;
        	}
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
    SafeQueue<string> playList;
    Music music;
    string operation;
    thread t1(threadOperation, &music, ref(playList), ref(operation), ref(s));

    bool Player = true;

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
		else if (option == "play" && Player && !playList.empty()) {
		    Player = false;
			operation = "play";
		}
		else if (option == "next" && !Player && !playList.empty()) {
		    operation = "play";
		}
        
    }
    return 0;
}
