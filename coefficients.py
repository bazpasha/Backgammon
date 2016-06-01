from sklearn.linear_model import LinearRegression
import numpy as np
import random

def normalize(vector):
    module = 0
    for elem in vector:
        module += elem ** 2
    module **= 0.5
    for i in range(len(vector)):
        vector[i] /= module
    return vector


inFile = open('datasetFinal.txt')
dataset = []
probabilities = []
for line in inFile:
    line = line.split(' ')
    dataset.append(list(map(int, line[:-1])))
    dataset[-1] = normalize(dataset[-1])
    probabilities.append(float(line[-1]))
test_number = len(dataset)


datasetN = np.array(dataset)
probabilitiesN = np.array(probabilities)
answer = LinearRegression()
answer = answer.fit(datasetN, probabilitiesN)


print(answer.intercept_, end='\n{')
for elem in answer.coef_[:-1]:
    print(elem, end=', ')
print(answer.coef_[-1], end='}')

