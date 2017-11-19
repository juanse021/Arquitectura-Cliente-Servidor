#include <string>
#include <thread>
#include <iostream>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

void servidor(socket &s, string ipLocal, string &ipSucesor, int &idSucesor, int idLocal,context &ctx) {
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

            if (ipSucesor == "") {
                ipSucesor = ipReq;
                idSucesor = idReq;
                message out;
                out << "accept" << ipLocal << idLocal;
                s.send(out);
                cout << "ipSucesor: " << ipSucesor << endl;
                cout << "idSucesor: " << idReq << endl;
                cout << "Alone "<< endl;
            }
            else if (idReq>idLocal&&idReq<idSucesor) {
                ipReq = ipSucesor;
                idReq = idSucesor;
                message out;
                out << "accept" << ipSucesor << idSucesor;
                s.send(out);
                cout << "ipSucesor: " << ipSucesor << endl;
                cout << "idSucesor: " << idSucesor << endl;
                cout << "middle" << endl;
            }
            else if ((idLocal>idSucesor)&&(((idReq<idLocal)&&(idReq<idSucesor))||((idReq>idSucesor)&&(idReq>idLocal)))) {
                ipReq = ipSucesor;
                idReq = idSucesor;
                message out;
                out << "accept" << ipSucesor << idSucesor;
                s.send(out);
                cout << "ipSucesor: " << ipSucesor << endl;
                cout << "idSucesor: " << idSucesor << endl;
                cout << "extrem" << endl;
            } else {
                message out;
                out << "reject" << ipSucesor << idSucesor;
                s.send(out);
                cout << " reject --- ipSucesor: " << ipSucesor << endl;

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

            if (idReq == idSucesor) {
                idSucesor = idReqx;
                ipSucesor = ipReq;
                message out;
                out << "ok";
                s.send(out);
                cout << "siguiente actualizado" << endl;
            } else {
                message n;
                n << "ok";
                s.send(n);
                socket c(ctx, socket_type::req);
                c.connect("tcp://localhost:" + ipSucesor);
                message out;
                out << "salir" << idReq << idReqx << ipReq;
                c.send(out);
                message in;
                c.receive(in);
            }
        }
    }
}


void cliente(socket &c, string &ipSucesor,string &operation, int idLocal, string ipLocal, int &idSucesor) {
    while (true) {
        if (operation == "connect") {
            operation = "";
            bool connect = true;

            while (connect) {
                c.connect("tcp://localhost:" + ipSucesor);
                message out;
                out << "connect" << ipLocal << idLocal;
                c.send(out);

                message in;
                c.receive(in);
                string answer;
                in>>answer;
                in >> ipSucesor;
                in >> idSucesor;
                if (answer == "accept"){
                out << "reject" << ipSucesor << idSucesor;
                    connect = false;
                }else{
                    cout << "Newraund " <<endl;
                }

            }
        }
        if (operation == "salir") {
            operation = "";
            c.connect("tcp://localhost:" + ipSucesor);
            message out;
            out << "salir" << idLocal << idSucesor << ipSucesor;
            c.send(out);

            message in;
            c.receive(in);
        }
    }
}

int main() {

    context ctx;
    string ipLocal = "5555";
    string ipSucesor = "";
    int idLocal = 50;
    int idSucesor;
    string operation;

    socket s(ctx, socket_type::rep);
    socket c(ctx, socket_type::req);
    thread t1(servidor, ref(s), ipLocal, ref(ipSucesor), ref(idSucesor), idLocal, ref(ctx));
    thread t2(cliente, ref(c), ref(ipSucesor), ref(operation), idLocal, ipLocal, ref(idSucesor));
    string aux;
    cout << "conoce la direccion ip de otro cliente (si/no) " << endl;
    cin >> aux;
    
    if (aux == "si") {
        cout << "introduzca la direccion ip a travez de la cual quiere ingresar" << endl;
        cin >> ipSucesor;
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
            cout << "ipSucesor" << ipSucesor<< "idSucesor" << idSucesor <<endl; 

        }
        
    }

    t1.join();
    t2.join();

    return 0;
}
