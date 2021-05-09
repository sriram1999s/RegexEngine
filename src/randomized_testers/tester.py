import re

string = input()
num = int(input())
for i in range(num):
    pat = input()
    m = re.search(pat, string)
    if m:
    	if m.end(0)==0:
    		print('1', m.start(0), m.end(0))
    	else:
        	print('1', m.start(0), m.end(0) - 1)
    else:
        print(0)
