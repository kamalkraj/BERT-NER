# BERT NER

Use google BERT to do CoNLL-2003 NER !


# Requirements

-  `python3`
- `pip3 install -r requirements.txt`

# Run

`python run_ner.py --data_dir=data/ --bert_model=bert-base-cased --task_name=ner --output_dir=out_!x --max_seq_length=128 --do_train --num_train_epochs 5 --do_eval --warmup_proportion=0.4`


# Result

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

# Inference

```python
from bert import Ner

model = Ner("out_!x/")

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