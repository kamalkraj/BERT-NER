# BERT NER

Use google BERT to do CoNLL-2003 NER !

![new](https://i.imgur.com/OB4Ugp4.png) Train model using Python and Inference using C++

[ALBERT-TF2.0](https://github.com/kamalkraj/ALBERT-TF2.0)

[BERT-NER-TENSORFLOW-2.0](https://github.com/kamalkraj/BERT-NER-TF)

[BERT-SQuAD](https://github.com/kamalkraj/BERT-SQuAD)


# Requirements

-  `python3`
- `pip3 install -r requirements.txt`

# Run

`python run_ner.py --data_dir=data/ --bert_model=bert-base-cased --task_name=ner --output_dir=out_base --max_seq_length=128 --do_train --num_train_epochs 5 --do_eval --warmup_proportion=0.1`


# Result

## BERT-BASE

### Validation Data
```
             precision    recall  f1-score   support

        PER     0.9677    0.9745    0.9711      1842
        LOC     0.9654    0.9711    0.9682      1837
       MISC     0.8851    0.9111    0.8979       922
        ORG     0.9299    0.9292    0.9295      1341

avg / total     0.9456    0.9534    0.9495      5942
```
### Test Data
```
             precision    recall  f1-score   support

        PER     0.9635    0.9629    0.9632      1617
        ORG     0.8883    0.9097    0.8989      1661
        LOC     0.9272    0.9317    0.9294      1668
       MISC     0.7689    0.8248    0.7959       702

avg / total     0.9065    0.9209    0.9135      5648
```
## Pretrained model download from [here](https://1drv.ms/u/s!Auc3VRul9wo5hghurzE47bTRyUeR?e=08seO3)

## BERT-LARGE

### Validation Data
```
             precision    recall  f1-score   support

        ORG     0.9288    0.9441    0.9364      1341
        LOC     0.9754    0.9728    0.9741      1837
       MISC     0.8976    0.9219    0.9096       922
        PER     0.9762    0.9799    0.9781      1842

avg / total     0.9531    0.9606    0.9568      5942
```
### Test Data
```
             precision    recall  f1-score   support

        LOC     0.9366    0.9293    0.9329      1668
        ORG     0.8881    0.9175    0.9026      1661
        PER     0.9695    0.9623    0.9659      1617
       MISC     0.7787    0.8319    0.8044       702

avg / total     0.9121    0.9232    0.9174      5648
```
## Pretrained model download from [here](https://1drv.ms/u/s!Auc3VRul9wo5hgr8jwhFD8iPCYp1?e=UsJJ2V)

# Inference

```python
from bert import Ner

model = Ner("out_base/")

output = model.predict("Steve went to Paris")

print(output)
'''
    [
        {
            "confidence": 0.9981840252876282,
            "tag": "B-PER",
            "word": "Steve"
        },
        {
            "confidence": 0.9998939037322998,
            "tag": "O",
            "word": "went"
        },
        {
            "confidence": 0.999891996383667,
            "tag": "O",
            "word": "to"
        },
        {
            "confidence": 0.9991968274116516,
            "tag": "B-LOC",
            "word": "Paris"
        }
    ]
'''
```

# Inference C++

## Pretrained and converted bert-base model download from [here](https://1drv.ms/u/s!Auc3VRul9wo5hgkJjtxZ8FAQGuj2?e=wffJCT)
### Download libtorch from [here](https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-1.2.0.zip)

- install `cmake`, tested with `cmake` version `3.10.2`
- unzip downloaded model and `libtorch` in `BERT-NER`
- Compile C++ App
  ```bash
    cd cpp-app/
    cmake -DCMAKE_PREFIX_PATH=../libtorch
   ```
    ![cmake output image](/img/cmake.png)
    ```bash
    make
    ```
    ![make output image](/img/make.png)


- Runing APP
  ```bash
     ./app ../base
  ```
     ![inference output image](/img/inference.png)

NB: Bert-Base C++ model is split in to two parts.
  - Bert Feature extractor and NER classifier.
  - This is done because `jit trace` don't support `input` depended `for` loop or `if` conditions inside `forword` function of `model`.



# Deploy REST-API
BERT NER model deployed as rest api
```bash
python api.py
```
API will be live at `0.0.0.0:8000` endpoint `predict`
#### cURL request
` curl -X POST http://0.0.0.0:8000/predict -H 'Content-Type: application/json' -d '{ "text": "Steve went to Paris" }'`

Output
```json
{
    "result": [
        {
            "confidence": 0.9981840252876282,
            "tag": "B-PER",
            "word": "Steve"
        },
        {
            "confidence": 0.9998939037322998,
            "tag": "O",
            "word": "went"
        },
        {
            "confidence": 0.999891996383667,
            "tag": "O",
            "word": "to"
        },
        {
            "confidence": 0.9991968274116516,
            "tag": "B-LOC",
            "word": "Paris"
        }
    ]
}
```
#### cURL 
![curl output image](/img/curl.png)
#### Postman
![postman output image](/img/postman.png)

### C++ unicode support 
- http://github.com/ufal/unilib

### Tensorflow version

- https://github.com/kyzhouhzau/BERT-NER
