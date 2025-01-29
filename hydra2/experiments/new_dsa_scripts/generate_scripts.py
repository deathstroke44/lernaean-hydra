import os

files=os.listdir()

s=''
for file in files:
   s=s+'bash '+file+' && '
s=s+' echo 1'
print(s) 