import numpy as np
import os, sys

trainDataPath = "./mnist/train-images.idx3-ubyte"
testDataPath = "./mnist/t10k-images.idx3-ubyte"
trainLabelPath = "./mnist/train-labels.idx1-ubyte"
testLabelPath = "./mnist/t10k-labels.idx1-ubyte"

scriptPath = os.path.dirname(os.path.abspath(sys.argv[0]))
modelDir = os.path.join(scriptPath, "mnist", "mnist_convnet_model")

class mnist_data:
    def __init__(self):
        self.testData = self.readDataFile(testDataPath)
        self.trainData = self.readDataFile(trainDataPath)
        self.testLabels = self.readLabelFile(testLabelPath)
        self.trainLabels = self.readLabelFile(trainLabelPath)

    def readDataFile(self, path):
        with open(path, 'rb') as dataFile:
            magic = dataFile.read(4)
            items = int.from_bytes(dataFile.read(4), byteorder = 'big')
            rows = int.from_bytes(dataFile.read(4), byteorder = 'big')
            cols = int.from_bytes(dataFile.read(4), byteorder = 'big')
            dataSize = cols * rows
            data = np.zeros((items, dataSize), dtype = np.uint8)
            for i in range(items):
                data[i] = np.frombuffer(dataFile.read(dataSize), dtype = np.uint8)
            return data

    def readLabelFile(self, path):
        with open(path, 'rb') as labelFile:
            magic = labelFile.read(4)
            items = int.from_bytes(labelFile.read(4), byteorder = 'big')
            data = np.frombuffer(labelFile.read(items), dtype = np.uint8)
            return data