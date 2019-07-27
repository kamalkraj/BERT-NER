import pandas as pd
import re
import tqdm
from tqdm import tqdm

# # set your read path
# root = "~/PycharmProjects/DeeCamp"
# path = "/data/regex_test.xlsx"
# data = pd.read_excel(root + path)


def select_singel_label(data):
    data.dropna(how ="any",inplace=True)
    icd_list = data["icd"].tolist()
    diagnose_list = data["diagnose"].tolist()
    diagnose_list = [t.strip(" ") for t in diagnose_list]

    # pattern = "(\w+\d+(\.\w+)*,.*?),"
    pattern = "(?<=[^\d]),"
    match = re.compile(pattern)

    new_list = []
    for line in tqdm(icd_list,desc="Split ICD"):
        print(line)
        line = re.sub(pattern, ";", line)
        line = line.split(";")
        new_list.append(line)

    data["icd"] = new_list
    icd_list = new_list

    total_list = list(zip(diagnose_list,icd_list))

    output_list = []
    for line in tqdm(total_list, desc="Select Single Label"):
        if len(line[1]) == 1:
            container = line[1][0].split(",")
            output_list.append((line[0], container[1], container[0]))

    # [print(t) for t in output_list]
    """
        output_list:
            ('右 输尿管结石', '输尿管结石', 'N20.100')
            ('先兆临产管结石', '假临产', 'O47')
            ...
    """

    # data_output = pd.DataFrame(columns=["diagnose","icd"])
    # for line in tqdm(output_list, desc="To Datafram"):
    #     data_output = data_output.append({"diagnose": line[0], "icd": line[1]}, ignore_index=True)

    return output_list
    # print(data_output)
    # print(data)

    #data.to_excel("your_path")





