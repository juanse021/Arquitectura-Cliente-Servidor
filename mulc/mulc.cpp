#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <stdio.h> 
#include <string>

using namespace std;

vector<int> FileToVector(ifstream &file,int tamano){
   char aux[100];
   vector<int> data;
   for(int i=0;i<tamano;i++){
      file >> aux;
      data.push_back(atoi(aux));
   }
   return data; 
}

vector<int> Multiplicacion(vector <int> matriz,vector <int> matriz2,int rows,int cols,int rows2,int cols2){
   vector<int> respuesta;
   for(int i=0;i<rows;i++){
      for(int j=0;j<cols2;j++){
         int num=0;
         for(int k=0;k<cols;k++){
            num += matriz[k+i*cols] * matriz2[j+k*cols];       
         }
         respuesta.push_back(num);
      }
   }
}

void VetorToFile(vector<int> v,ifstream &file){

}

int main () {
   
   ifstream matriz2; 
   matriz2.open("matriz2");
   ifstream matriz; 
   matriz.open("matriz");
   
   char aux[20];
   matriz >> aux;
   int rows = atoi(aux);
   cout<<rows<<endl;
   matriz >> aux;
   int cols = atoi(aux);
   matriz2 >> aux;
   int rows2 = atoi(aux);
   matriz2 >> aux;
   int cols2 = atoi(aux);


   

   vector <int> Matriz = FileToVector(matriz,rows*cols);
   vector <int> Matriz2 = FileToVector(matriz2,rows2*cols2);
   vector<int> respuesta;
   for(int i=0;i<rows;i++){ 
      cout<<i<<endl;
      for(int j=0;j<cols2;j++){
         int num=0;
         for(int k=0;k<cols;k++){
            num += Matriz[k+i*cols] * Matriz2[j+k*cols];  
         }
         respuesta.push_back(num);

      }
   }
   
   ofstream salida("salida"); 
   for (int i = 0; i < int(respuesta.size()); i++){
      if(i>rows-2&&i%rows==0){
         salida<<"\n";
      }
      salida<<respuesta[i];
      salida<<" ";
   }

   salida.close();
   matriz2.close();
   matriz.close();

   return 0;
}