A = open("matriz2", "r")
B = open("matriz", "r")
tamano = 3
def FiletoVector(A):
    a = A.readline()
    a = A.readline()
    m_a=[]
    while(True):
        a = A.readline()
        if (a == ""):
            break
        m_a += a.split()
    return m_a
A=FiletoVector(A)
B=FiletoVector(B)
respuesta=[]
i=0
print "a calcular"
while(i<tamano*tamano):
    fila = i/tamano
    columna = i-(fila*tamano)
    aux=0
    casilla=0
    while(aux<tamano):
        casilla += int(B[(fila*tamano)+aux])*int(A[(tamano*aux)+columna])
        aux+=1
    respuesta.append(casilla)
    i+=1
    if (i%(tamano*100) == 0):
        print i
resp=open("respuesta","a")
aux=0
i=0
line=""
while(aux<tamano*tamano):
    if (i == tamano-1):
        line += str(respuesta[aux])+"\n"
        resp.write(line)
        line=""
        aux+=1
        i=0
    else:
        line += str(respuesta[aux])+","
        aux+=1
        i+=1
    
        
