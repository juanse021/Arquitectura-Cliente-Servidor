fileA = open("matriz", "r")
fileB = open("matriz2", "r")

rows1 = int(fileA.readline())
cols1 = int(fileA.readline())

rows2 = int(fileB.readline())
cols2 = int(fileB.readline())

def fileToVector(file, rows, cols):
	vec = []
	for i in range(rows):
		aux = file.readline().split()
		for j in aux:
			vec.append(int(j))

	return vec
print"antes"
m1 = fileToVector(fileA, rows1, cols1)
print "medio"
m2 = fileToVector(fileB, rows2, cols2)
print"despues"
resp = open("mrPython", "a")
respuesta = []
for i in range(rows1):
	print i
	for j in range(cols2):
		num = 0
		for k in range(rows1):
			num += m1[k+i*rows1] * m2[j+k*rows1]
		respuesta.append(num)
j=1
line=""
for i in respuesta:
	if j==rows1:
		line+=str(i)+"\n"
		j=1
		resp.write(line)
		line=""
	else:
		j+=1
		line+=(str(i)+",")




