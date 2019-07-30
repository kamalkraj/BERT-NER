# BERT NER

Use google BERT to do CoNLL-2003 NER !


# Requirements

-  `python3`
- `pip3 install -r requirements.txt`

# Run

`python run_ner.py --data_dir=data/ --bert_model=bert-base-cased --task_name=ner --output_dir=out_!x --max_seq_length=128 --do_train --num_train_epochs 5 --do_eval --warmup_proportion=0.4`


# Result

## BERT-BASE

### Validation Data
```
             precision    recall  f1-score   support

        PER     0.9687    0.9756    0.9721      1842
        ORG     0.9299    0.9292    0.9295      1341
       MISC     0.8878    0.9100    0.8988       922
        LOC     0.9674    0.9701    0.9687      1837

avg / total     0.9470    0.9532    0.9501      5942
```
### Test Data
```
             precision    recall  f1-score   support

        ORG     0.8754    0.9055    0.8902      1661
        PER     0.9663    0.9573    0.9618      1617
       MISC     0.7803    0.8348    0.8066       702
        LOC     0.9271    0.9305    0.9288      1668

avg / total     0.9049    0.9189    0.9117      5648
```
## Pretrained model download from [here](https://drive.google.com/file/d/1hmj1zC6xipR7KTT04bJpSUPNU1pRuI7h/view?usp=sharing)

## BERT-LARGE

### Validation Data
```
             precision    recall  f1-score   support

        PER     0.9787    0.9739    0.9763      1842
        LOC     0.9700    0.9690    0.9695      1837
       MISC     0.8899    0.9208    0.9051       922
        ORG     0.9311    0.9478    0.9394      1341

avg / total     0.9515    0.9583    0.9548      5942
```
### Test Data
```
             precision    recall  f1-score   support

        ORG     0.8856    0.9181    0.9016      1661
        LOC     0.9250    0.9323    0.9286      1668
        PER     0.9694    0.9586    0.9639      1617
       MISC     0.7937    0.8219    0.8076       702

avg / total     0.9098    0.9219    0.9157      5648
```
## Pretrained model download from [here](https://drive.google.com/file/d/1OCI6EeXwX3AF50hnR6J1VAgBUPRNZGG8/view?usp=sharing)

# Inference

```python
from bert import Ner

model = Ner("out_!x/")

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


### Tensorflow version

- https://github.com/kyzhouhzau/BERT-NER
