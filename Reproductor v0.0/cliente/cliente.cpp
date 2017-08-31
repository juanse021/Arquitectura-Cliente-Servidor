#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <fstream>
#include <iostream>
#include <SFML/Audio.hpp>
#include <zmqpp/zmqpp.hpp>
#include <condition_variable>
#include <SFML/System/Time.hpp>

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
            cout << "El elemento es: " << element << endl;
            q.pop();
            return element;
		}

        bool empty() {
			return q.empty();
		}

		~SafeQueue() {};
	
};

void messageToFile(const message &msg, const string &fileName, bool first) {
    const void *data;
    msg.get(&data, 0);
    size_t size = msg.size(0);
    if(first){
        ofstream ofs(fileName, ios::binary | ios_base::app);
        ofs.write((char *)data, size);
    }else{
        ofstream ofs(fileName, ios::binary);
        ofs.write((char *)data, size);
    
    }
    
}

void threadOperation(Music *music, SafeQueue<string> &playList, string &operation, socket &s){
    bool player = false;
    int parte = 0;
    int fin = 0;
    bool flag = false;
    string nameSong;
    Clock clock;
	while (true) {   
	 
		if (operation == "play"){
			flag = true;
			operation = "none";
			parte = 0;
            fin = 0;
			player = true;
        	message n;
        	n << "play";
        	nameSong = playList.front();
        	n << nameSong;
        	n << 0;
	        s.send(n);

	        // Receive operation to do
	        message answer;
	        s.receive(answer);
            messageToFile(answer, "song.ogg",false);
            music->openFromFile("song.ogg");
            music->play();
            Clock reset;
            clock=reset;
            if (playList.empty()){
        		player=false;
        	}
        	answer>>fin;
	        answer>>fin;
            cout<<fin<<endl;
            parte++;
		}

		if (fin == 0 && flag){
			if(clock.getElapsedTime().asSeconds()>10){
				message n;
            	n << "play";
            	n << nameSong;
            	n << parte;
    	        s.send(n);

    	        message answer;
    	        s.receive(answer);
                messageToFile(answer, "song.ogg",true);
                Clock reset;
                clock=reset;
    	        answer>>fin;
    	        answer>>fin;
                cout<<fin<<endl;
                parte++;
			}
		}
	
		
		
		if (music->getStatus() != SoundSource::Playing && player){
			operation = "play";
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
