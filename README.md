# BERT_NER

Use google BERT to do CoNLL-2003 NER !


# Requirements

-  `python3`
- `pip3 install -r requirements.txt`

# Run

`python run_ner.py --data_dir=data/ --bert_model=bert-base-cased --task_name=ner --output_dir=out --max_seq_length=128 --do_eval`


# Result
```
             precision    recall  f1-score   support

       MISC     0.9407    0.9304    0.9355       273
        LOC     0.9765    0.9905    0.9834       419
        PER     0.9814    0.9814    0.9814       322
        ORG     0.9706    0.9792    0.9749       337

avg / total     0.9690    0.9734    0.9711      1351
```


### Tensorflow version

- https://github.com/kyzhouhzau/BERT-NER