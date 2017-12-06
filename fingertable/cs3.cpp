#include <string>
#include <thread>
#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <map>
#include <utility>
#include <math.h>


using namespace std;
using namespace zmqpp;

void servidor(socket &s, string ipLocal, int idLocal,context &ctx, map<int,pair<int,string>> &finguerTable) {
    s.bind("tcp://*:" + ipLocal);
    while (true) {
        message in;
        string ipReq;
        int idReq;
        s.receive(in);
        string operation;
        in >> operation;
        
        
        if (operation == "connect") {
            in >> ipReq; 
            in >> idReq;

            if (finguerTable[0].second == "") {
                message out;
                out << "accept" << ipLocal << idLocal;
                s.send(out);
                finguerTable[0].second = ipReq;
                finguerTable[0].first = idReq;
                cout << "ipSucesor: " << finguerTable[0].second << endl;
                cout << "idSucesor: " << finguerTable[0].first << endl;
                for(int i = 1;i<7;i++){
                	int data;
                	if (idLocal+pow(2,i)>127){
                		data=(idLocal+pow(2,i))-127;
                	}else{
                		data=idLocal+pow(2,i);
                	}
                	if(data>=idLocal&&data<finguerTable[0].first){
                		finguerTable[i].first=idLocal;
                		finguerTable[i].second=ipLocal;
                	}else if ((idLocal>finguerTable[0].first)&&(((data<idLocal)&&(data<finguerTable[0].first))||((data>finguerTable[0].first)&&(data>idLocal))))
                	{
                		finguerTable[i].first=idLocal;
                		finguerTable[i].second=ipLocal;
                	}else{
                		finguerTable[i].first=finguerTable[0].first;
                		finguerTable[i].second=finguerTable[0].second;
                	}

                }
                cout << "Alone "<< endl;
            }
            else if (idReq>idLocal&&idReq<finguerTable[0].first) {
                message out;
                out << "accept" << finguerTable[0].second << finguerTable[0].first;
                s.send(out);
                finguerTable[0].second = ipReq;
                finguerTable[0].first = idReq;
                cout << "ipSucesor: " << finguerTable[0].second << endl;
                cout << "idSucesor: " << finguerTable[0].first << endl;
                cout << "middle" << endl;
            }
            else if ((idLocal>finguerTable[0].first)&&(((idReq<idLocal)&&(idReq<finguerTable[0].first))||((idReq>finguerTable[0].first)&&(idReq>idLocal)))) {
                message out;
                out << "accept" << finguerTable[0].second << finguerTable[0].first;
                s.send(out);
                finguerTable[0].second = ipReq;
                finguerTable[0].first = idReq;
                cout << "ipSucesor: " << finguerTable[0].second << endl;
                cout << "idSucesor: " << finguerTable[0].first << endl;
                cout << "extrem" << endl;
            } else {
                message out;
                out << "reject" << finguerTable[0].second << finguerTable[0].first;
                s.send(out);
                cout << " reject --- ipSucesor: " << finguerTable[0].second << endl;

            }

        }
        if (operation == "salir") {
            operation = "";
            string ipReq;
            int idReq;
            int idReqx;
            in >> idReq;
            in >> idReqx;
            in >> ipReq;

            cout << "idReq: " << idReq << endl;
            cout << "idReqx: " << idReqx << endl;
            cout << "ipReq:" << ipReq << endl;

            if (idReq == finguerTable[0].first) {
                finguerTable[0].first = idReqx;
                finguerTable[0].second = ipReq;
                message out;
                out << "ok";
                s.send(out);
                cout << "siguiente actualizado" << endl;
            } else {
                message n;
                n << "ok";
                s.send(n);
                socket c(ctx, socket_type::req);
                c.connect("tcp://localhost:" + finguerTable[0].second);
                message out;
                out << "salir" << idReq << idReqx << ipReq;
                c.send(out);
                message in;
                c.receive(in);
            }
        }
        if (operation == "inSpace") {
            int llave;
            in >> llave;
            if(llave>=idLocal&&llave<finguerTable[0].first){
                message out;
                out << "positivo" << idLocal << ipLocal;
                s.send(out);
                cout<<"positivo"<<endl;
            }else if ((idLocal>finguerTable[0].first)&&(((llave<idLocal)&&(llave<finguerTable[0].first))||((llave>finguerTable[0].first)&&(llave>idLocal)))){
                message out;
                out << "positivo" << idLocal << ipLocal;
                s.send(out);
                cout<<"positivo extream"<<endl;
            }
            else{
            	message out;
                out << "negativo" << finguerTable[0].first << finguerTable[0].second;
                s.send(out);
                cout<<"negativo"<<endl;
                cout <<finguerTable[0].first << finguerTable[0].second << endl;


            }
        }
    }
}


void cliente(socket &c,string &operation, int idLocal, string ipLocal, map<int,pair<int,string>> &finguerTable) {
    while (true) {
        if (operation == "connect") {
            operation = "";
            bool connect = true;

            while (connect) {
                c.connect("tcp://localhost:" + finguerTable[0].second);
                message out;
                out << "connect" << ipLocal << idLocal;
                c.send(out);

                message in;
                c.receive(in);
                string answer;
                in>>answer;
                cout << "antes -------" << finguerTable[0].second << finguerTable[0].first<<endl;
                in >> finguerTable[0].second;
                in >> finguerTable[0].first;
                cout << "reject" << finguerTable[0].second << finguerTable[0].first<<endl;
                if (answer == "accept"){
                    cout <<"acept"<<endl;
                    string ipBusqueda = finguerTable[0].second;
                    for(int i=1;i<7;i++){
                        bool encontrado = true;
                        while(encontrado){
                            c.connect("tcp://localhost:" + ipBusqueda);
                            message out;
                            int data;
                            if (idLocal+pow(2,i)>127){
                            	data=(idLocal+pow(2,i))-127;
                            }else{
                            	data=idLocal+pow(2,i);
                            }
                            out << "inSpace" << data;
                            cout<<data<<endl;
                            c.send(out);
                            message in;
                            c.receive(in);
                            string answer;
                            in>>answer;
                            if (answer == "positivo"){
                                in>>finguerTable[i].first;
                                cout<<finguerTable[i].first<<endl;
                                in>>finguerTable[i].second;
                                encontrado = false;
                            }else{
                                in>>ipBusqueda;
                            }
                        }
                    }

                    connect = false;
                    c.connect("tcp://localhost:" + finguerTable[0].second);
                	message out;
                	out << "finguerTable";
                	c.send(out);

                	message in;
                	c.receive(in);
                }else{
                    cout << "Newraund " <<endl;
                }

            }
        }
        if (operation == "salir") {
            operation = "";
            c.connect("tcp://localhost:" + finguerTable[0].second);
            message out;
            out << "salir" << idLocal << finguerTable[0].first << finguerTable[0].second;
            c.send(out);

            message in;
            c.receive(in);
        }
    }
}

int main() {

    context ctx;
    string ipLocal = "3333";
    int idLocal = 87;
    map<int,pair<int,string> > finguerTable;
    finguerTable[0].second = "";
    string operation;

    socket s(ctx, socket_type::rep);
    socket c(ctx, socket_type::req);
    thread t1(servidor, ref(s), ipLocal, idLocal, ref(ctx), ref(finguerTable));
    thread t2(cliente, ref(c), ref(operation), idLocal, ipLocal, ref(finguerTable));
    string aux;
    cout << "conoce la direccion ip de otro cliente (si/no) " << endl;
    cin >> aux;
    
    if (aux == "si") {
        cout << "introduzca la direccion ip a travez de la cual quiere ingresar" << endl;
        cin >> aux;
        finguerTable[0].first=0;
        finguerTable[0].second=aux;
        operation = "connect";
    }

    while (true) {
        string a;
        cout << "que desea hacer"<<endl;
        cin >> a;
        if (a == "salir"){
            operation = "salir";
            cout << "te has salido traidor" << endl;
        }else{
        	for(int i=0;i<7;i++){
        		cout << "idSucesor" << finguerTable[i].first <<"ipSucesor"<<finguerTable[i].second<< endl;
        	}
        }
        
    }

    t1.join();
    t2.join();

    return 0;
}
