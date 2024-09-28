


import random


data_dict = { str(i)+"_09" : str(random.random()) for i in range(int(4e6))}


count = 0 
for i in range(int(4e6)):
    if str(i) in data_dict:
        count+= data_dict[i]