# BERT NER

Use google BERT to do CoNLL-2003 NER !


# Requirements

-  `python3`
- `pip3 install -r requirements.txt`

# Run

`python run_ner.py --data_dir=data/ --bert_model=bert-base-cased --task_name=ner --output_dir=out --max_seq_length=128 --do_train --num_train_epochs 5 --do_eval --warmup_proportion=0.4`


# Result

### Validation Data
```
             precision    recall  f1-score   support

       MISC     0.9407    0.9304    0.9355       273
        LOC     0.9650    0.9881    0.9764       419
        PER     0.9844    0.9783    0.9813       322
        ORG     0.9794    0.9852    0.9822       337

avg / total     0.9683    0.9734    0.9708      1351
```
### Test Data
```
             precision    recall  f1-score   support

        ORG     0.9152    0.9073    0.9113       464
        PER     0.9767    0.9692    0.9730       260
        LOC     0.9397    0.9263    0.9330       353
       MISC     0.8276    0.9014    0.8629       213

avg / total     0.9198    0.9240    0.9217      1290
```

## Pretrained model download from [here](https://drive.google.com/file/d/1UKE2UVFStXZFtXFgZObGg5mo_MzW-ZoC/view?usp=sharing) 

# Inference

```python
from bert import Ner

model = Ner("out/")

output = model.predict("Steve went to Paris")

print(output)
# {
#     "Steve": {
#         "tag": "B-PER",
#         "confidence": 0.999879002571106
#     },
#     "went": {
#         "tag": "O",
#         "confidence": 0.9968552589416504
#     },
#     "to": {
#         "tag": "O",
#         "confidence": 0.9996656179428101
#     },
#     "Paris": {
#         "tag": "B-LOC",
#         "confidence": 0.999504804611206
#     }
# }

```


### Tensorflow version

- https://github.com/kyzhouhzau/BERT-NER