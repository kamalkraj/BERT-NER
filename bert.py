"""BERT NER Inference.""" 

from __future__ import absolute_import, division, print_function

import json
import os

import torch
import torch.nn.functional as F
from nltk import word_tokenize
from pytorch_pretrained_bert.modeling import (CONFIG_NAME, WEIGHTS_NAME,
                                              BertConfig,
                                              BertForTokenClassification)
from pytorch_pretrained_bert.tokenization import BertTokenizer


class Ner:

    def __init__(self,model_dir: str):
        self.model , self.tokenizer, self.model_config = self.load_model(model_dir)
        self.label_map = self.model_config["label_map"]
        self.max_seq_length = self.model_config["max_seq_length"]
        self.label_map = {int(k):v for k,v in self.label_map.items()}
        self.model.eval()

    def load_model(self, model_dir: str, model_config: str = "model_config.json"):
        model_config = os.path.join(model_dir,model_config)
        model_config = json.load(open(model_config))
        output_config_file = os.path.join(model_dir, CONFIG_NAME)
        output_model_file = os.path.join(model_dir, WEIGHTS_NAME)
        config = BertConfig(output_config_file)
        model = BertForTokenClassification(config, num_labels=model_config["num_labels"])
        model.load_state_dict(torch.load(output_model_file))
        tokenizer = BertTokenizer.from_pretrained(model_config["bert_model"],do_lower_case=False)
        return model, tokenizer, model_config

    def tokenize(self, text: str):
        """ tokenize input"""
        words = word_tokenize(text)
        tokens = []
        valid_positions = []
        for i,word in enumerate(words):
            token = self.tokenizer.tokenize(word)
            tokens.extend(token)
            for i in range(len(token)):
                if i == 0:
                    valid_positions.append(1)
                else:
                    valid_positions.append(0)
        return tokens, valid_positions

    def preprocess(self, text: str):
        """ preprocess """
        tokens, valid_positions = self.tokenize(text)
        ## insert "[CLS]"
        tokens.insert(0,"[CLS]")
        ## insert "[SEP]"
        tokens.append("[SEP]")
        segment_ids = []
        for i in range(len(tokens)):
            segment_ids.append(0)
        input_ids = self.tokenizer.convert_tokens_to_ids(tokens)
        input_mask = [1] * len(input_ids)
        while len(input_ids) < self.max_seq_length:
            input_ids.append(0)
            input_mask.append(0)
            segment_ids.append(0)
        return input_ids,input_mask,segment_ids,valid_positions

    def predict(self, text: str):
        input_ids,input_mask,segment_ids,valid_positions = self.preprocess(text)
        input_ids = torch.tensor([input_ids],dtype=torch.long)
        input_mask = torch.tensor([input_mask],dtype=torch.long)
        segment_ids = torch.tensor([segment_ids],dtype=torch.long)
        with torch.no_grad():
            logits = self.model(input_ids, segment_ids, input_mask)
        logits = F.softmax(logits,dim=2)
        logits_label = torch.argmax(logits,dim=2)
        logits_label = logits_label.detach().cpu().numpy()
        # import ipdb; ipdb.set_trace()
        logits_confidence = [values[label].item() for values,label in zip(logits[0],logits_label[0])]

        logits_label = [logits_label[0][index] for index,i in enumerate(input_mask[0]) if i.item()==1]
        logits_label.pop(0)
        logits_label.pop()

        assert len(logits_label) == len(valid_positions)
        labels = []
        for valid,label in zip(valid_positions,logits_label):
            if valid:
                labels.append(self.label_map[label])
        words = word_tokenize(text)
        assert len(labels) == len(words)
        output = [word:{"tag":label,"confidence":confidence} for word,label,confidence in zip(words,labels,logits_confidence)]
        return output
