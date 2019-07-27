from data_process.ner_label import *
from data_process.split_single_label import select_singel_label
import pandas as pd
import pysnooper
import tqdm
from tqdm import tqdm
from tqdm._tqdm import trange

# @pysnooper.snoop()
def devide(length, scale):
    sum = scale[0]+scale[1]+scale[2]
    per = [x/sum for x in scale]
    range1 = int(per[0] * length)
    range2 = int(per[1] * length) + range1
    return range1,range2

def data_devide(df,scale):
    length = len(df)
    print(length)
    range1, range2 = devide(length, scale)
    print(range1,range2)
    train = df[0:range1]
    print(len(train))
    test = df[range1:range2]
    print(len(test))
    validation = df[range2:length]
    print(len(validation))
    return train, test, validation

def process(data_list, name):
    name = './' + name + '.txt'
    with open(name, 'w+') as f:
        # print(data_list.__class__)
        for line in tqdm(data_list):
            if tag(line) != []:
                out = tag(line)
                for element in out:
                    print(element[0],element[1], file = f)
                print('', file = f)


if __name__ == '__main__':
    root = "~/PycharmProjects/BERT_NER"
    path = "/data_process/regex.xlsx"

    data = pd.read_excel(root + path)

    print(data)
    data = select_singel_label(data)
    # df = pd.read_csv(path)

    scale = (6, 1, 1)
    train, test, validation = data_devide(data,scale)

    # print(train.head())
    # print(test.head())
    # print(validation.head())
    # print(len(train), len(test), len(validation))

    train_data = process(train, 'train')
    test_data = process(test, 'test')
    validation_data = process(validation, 'valid')
    #
    print(len(train_data))
    print(len(test_data))
    print(len(validation_data))


    # data = process(data, name = "dump")
    # print(data)
