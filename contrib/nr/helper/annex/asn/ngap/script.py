#getlibraries 

import os
import glob 

directory =  os.getcwd()
new_directory = ""
array = []
array1 = []
array2 = []
array3 = []

for i in range(len(directory)):
	if i > 38:
		new_directory = new_directory + directory[i]
for f in glob.glob("*.cpp"):
	array.append(new_directory+"/"+f)

for i in range(len(glob.glob("*.cpp"))):
	if array:
		print(array[i])

for f in glob.glob("*.hpp"):
	array1.append(new_directory+"/"+f)
for i in range(len(glob.glob("*.cpp"))):
	if array1:
		print(array1[i])


for f in glob.glob("*.c"):
	array2.append(new_directory+"/"+f)
for i in range(len(glob.glob("*.c"))):
	if array2:
		print(array2[i])


for f in glob.glob("*.h"):
	array3.append(new_directory+"/"+f)
for i in range(len(glob.glob("*.h"))):
	if array3:
		print(array3[i])
