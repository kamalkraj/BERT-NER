from flask import Flask,request,jsonify
from flask_cors import CORS

from bert import Ner

import nltk.data
sent_detector = nltk.data.load('tokenizers/punkt/english.pickle')

app = Flask(__name__)
CORS(app)

model = Ner("out_!x")

@app.route("/predict",methods=['POST'])
def predict():
    text = request.json["text"]
    sentences = sent_detector.tokenize(text)

    try:
        ner_tagged_sentences = []
        for sentence in sentences:
            if len(sentence) < 512:
                tagged_sentence = model.predict(sentence)
                ner_tagged_sentences.extend(tagged_sentence)
            else:
                chunks = [sentence[x:x+512] for x in range(0, len(sentence), 512)]
                tagged_sentence = []
                for chunk in chunks:
                    ner_chunk = model.predict(chunk)
                    tagged_sentence.extend(ner_chunk)
                ner_tagged_sentences.extend(tagged_sentence)
        return jsonify({"result":ner_tagged_sentences})
    except Exception as e:
        print(e)
        return jsonify({"result":"Model Failed"})

if __name__ == "__main__":
    app.run('0.0.0.0',port=8000)
