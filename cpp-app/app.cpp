#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>
#include <bits/stdc++.h>

#include "tokenizer.h"

using namespace std;

BertTokenizer tokenizer;
BasicTokenizer basictokenizer;

int max_seq_length = 128;
map<int,string> label_map;
bool do_lower_case;

vector<string> rive(const string &str, char delimiter) 
{
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    return internal;
}

string join(const char* a, char* b)
{
    string message;
    message.reserve(strlen(a) + 1 + strlen(b));
    message = a;
    message += "/";
    message += b;
    return message;
}

torch::jit::script::Module bert;
torch::jit::script::Module ner;

void load_model(const char* model)
{
    bert = torch::jit::load(join(model,"bert_features.zip").c_str());
    ner = torch::jit::load(join(model,"bert_ner.zip").c_str());
    string line;
    ifstream config (join(model,"model_config.txt").c_str());
    int i= 0;
    if (config.is_open())
    {
      while ( getline (config,line) )
      {
          //cout << line << '\n'<<i<<'\n';
          if (i == 0)
            {
                //cout << line << '\n';
                if(rive(line,' ').at(1) == "true")
                    do_lower_case = true;
                else
                    do_lower_case = false;
            }
            if (i == 1)
            {
		    max_seq_length = stoi(rive(line,' ').at(1));
		}
		if(i > 1)
            {
                vector<string> temp = rive(line,' ');
                label_map.insert(pair<int,string>{stoi(temp.at(0)),temp.at(1)});
            }
            i++;
      }
       config.close();
    }
   bert.eval();
   ner.eval();
}

void tokenize(string text, vector<string> &tokens, vector<float> &valid_positions)
{
    vector<string> words = basictokenizer.tokenize(text);
    vector<string> token;
    vector<string>::iterator itr;
    for (itr = words.begin(); itr < words.end(); itr++)
    {
        token = tokenizer.tokenize(*itr);
        tokens.insert(tokens.end(),token.begin(),token.end());
        for (int i = 0; i < token.size(); i++)
        {
            if(i == 0)
                valid_positions.push_back(1);
            else
                valid_positions.push_back(0);
        }
    }
}

void preprocess(string text, vector<float> &input_ids, vector<float> &input_mask, vector<float> &segment_ids, vector<float> &valid_positions)
{
    vector<string> tokens;
    tokenize(text,tokens,valid_positions);
    // insert "[CLS}"
    tokens.insert(tokens.begin(),"[CLS]");
    valid_positions.insert(valid_positions.begin(),1.0);
    // insert "[SEP]"
    tokens.push_back("[SEP]");
    valid_positions.push_back(1.0);
    for(int i = 0; i < tokens.size(); i++)
    {
        segment_ids.push_back(0.0);
        input_mask.push_back(1.0);
    }
    input_ids = tokenizer.convert_tokens_to_ids(tokens);
    while(input_ids.size() < max_seq_length)
    {
        input_ids.push_back(0.0);
        input_mask.push_back(0.0);
        segment_ids.push_back(0.0);
        valid_positions.push_back(0.0);
    }
}

vector<map<string,string>> predict(string text)
{
    vector<torch::jit::IValue> inputs;
    vector<float> input_ids;
    vector<float> input_mask;
    vector<float> segment_ids;
    vector<float> valid_positions;
    preprocess(text,input_ids,input_mask,segment_ids,valid_positions);
    inputs.push_back(torch::from_blob(input_ids.data(), {1, max_seq_length}).to(torch::kInt64));
    inputs.push_back(torch::from_blob(input_mask.data(), {1, max_seq_length}).to(torch::kInt64));
    inputs.push_back(torch::from_blob(segment_ids.data(), {1, max_seq_length}).to(torch::kInt64));
    torch::Tensor valid_ids = torch::from_blob(valid_positions.data(), {1, max_seq_length}).to(torch::kInt64);

    torch::Tensor features = bert.forward(inputs).toTensor();

    int batch_size = features.size(0);
    int max_len = features.size(1);
    int feat_dim = features.size(2);

    auto options = torch::TensorOptions().dtype(torch::kInt64);
    torch::Tensor one = torch::ones({1},options);


    torch::Tensor valid_output = torch::zeros({batch_size, max_len, feat_dim});
    for(int i = 0; i < batch_size; i++)
    {
        int jj = -1;
        for(int j = 0; j < max_len; j++)
        {
            if (valid_ids[i][j].equal(one[0]))
            {
                jj++;
                valid_output[i][jj] = features[i][j];
            }
        }
    }
    vector<torch::jit::IValue> features_out;
    features_out.push_back(valid_output);
    torch::Tensor output = ner.forward(features_out).toTensor().argmax(2);

    vector<long> out(output[0].data<long>(), output[0].data<long>() + output[0].numel());

    vector<long> logits;
    int pos = 0;
    for (int i = 1; i < valid_positions.size(); i++)
    {
        if (valid_positions.at(i) == 1.0)
            logits.push_back(out.at(i-pos));
        else
            pos++;
    }
    logits.pop_back();
    
    vector<string> words = basictokenizer.tokenize(text);
    vector<map<string,string>> result;
    for (int i = 0; i < words.size(); i++)
    {
        map<string,string> mp;
        mp.insert({words.at(i),label_map[logits.at(i)]});
        result.push_back(mp);
    }
    return result;
}

int main(int argc, const char* argv[])
{
    const char* model = argv[1];
    string text;
    tokenizer.add_vocab(join(model,"vocab.txt").c_str());
    load_model(model);
    while(true)
    {
        cout<<"\n"<<"Input -> ";
        getline(cin,text);
        vector<map<string,string>> output = predict(text);
        for(int i=0; i < output.size(); i++)
        {
            cout<<output[i]<<'\n';
        }
    }
	return 0;
}
