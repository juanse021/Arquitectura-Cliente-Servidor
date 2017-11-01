import random
f = open("matriz", "a")
filas = int(raw_input("filas \n"))
columnas = int(raw_input("columnas \n"))
while(filas>0):
	filas -= 1
	aux = 0
	columna = ""
	while(aux<columnas):
		aux += 1
		columna += str(random.randint(0,99))+" "
	columna += "\n"
	f.write(columna)
f.close()