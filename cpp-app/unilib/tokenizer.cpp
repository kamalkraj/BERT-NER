#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <map>
#include <regex>
#include <sstream>
#include "unicode.h"
#include "uninorms.h"
#include <codecvt>
#include <algorithm>

#include "tokenizer.h"


using namespace std;
using namespace ufal::unilib;


map<string, unicode::category_t> categories = {
  {"Lu", unicode::Lu}, {"Ll", unicode::Ll}, {"Lt", unicode::Lt}, {"Lm", unicode::Lm}, {"Lo", unicode::Lo},
  {"Mn", unicode::Mn}, {"Mc", unicode::Mc}, {"Me", unicode::Me}, {"Nd", unicode::Nd}, {"Nl", unicode::Nl},
  {"No", unicode::No}, {"Pc", unicode::Pc}, {"Pd", unicode::Pd}, {"Ps", unicode::Ps}, {"Pe", unicode::Pe},
  {"Pi", unicode::Pi}, {"Pf", unicode::Pf}, {"Po", unicode::Po}, {"Sm", unicode::Sm}, {"Sc", unicode::Sc},
  {"Sk", unicode::Sk}, {"So", unicode::So}, {"Zs", unicode::Zs}, {"Zl", unicode::Zl}, {"Zp", unicode::Zp},
  {"Cc", unicode::Cc}, {"Cf", unicode::Cf}, {"Cs", unicode::Cs}, {"Co", unicode::Co}, {"Cn", unicode::Cn},
};

map<unicode::category_t,string> categories_rev;

string ltrim(string str ) {
    return regex_replace( str, regex("^\\s+"), string("") );
}

string rtrim( string str ) {
    return regex_replace( str, regex("\\s+$"), string("") );
}

string trim( string str ) {
    return ltrim( rtrim( str ) );
}

vector<string> split(const string &str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  return internal;
}

map<string,int> read_vocab(const char* filename)
{
    map <string,int> vocab;
    int index = 0;
    unsigned int line_count = 1;
    ifstream fs8(filename);
    if (!fs8.is_open())
    {
        cout << "Could not open " << filename << endl;
        return vocab;
    }
    string line;
    // Read all the lines in the file
    while (getline(fs8, line))
    {
       // check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
       // string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
        vocab.insert(pair<string,int>(string(line.begin(), line.end()),index));
        index++;
        line_count++;
    }
    return vocab;
}

vector<string> whitespace_tokenize(string text)
{
    vector<string> result;
    char delimeter = ' ';
    text = trim(text);
    if(text == "")
    {
        return result;
    }
    result = split(text,delimeter);
    return result;
}

bool _is_whitespace(char letter)
{
    if (letter == ' ' or letter == '\t' or letter == '\n' or letter == '\r')
        return true;
    long int cat = unicode::category(int(letter));
    if(cat == categories["Zs"])
        return true;
    return false;
}

bool _is_control(char letter)
{
    if(letter == '\t' or letter == '\n' or letter == '\r')
        return false;
    unicode::category_t cat = unicode::category(int(letter));
    string cat_ = categories_rev[cat];
    if(cat_[0] == 'C')
        return true;
    return false;
}

bool _is_punctuation(char letter)
{
    int cp = int(letter);
    if ((cp >= 33 and cp <= 47) or (cp >= 58 and cp <= 64) or
            (cp >= 91 and cp <= 96) or (cp >= 123 and cp <= 126))
        return true;
    unicode::category_t cat = unicode::category(int(letter));
    string cat_ = categories_rev[cat];
    if(cat_[0] == 'P')
        return true;
    return false;
}

string BasicTokenizer::_clean_text(string text)
{
    string output;
    int len = 0;
    int len_char_array = text.length();
    char * char_array = new char [text.length()+1];
    strcpy (char_array, text.c_str());
    while(char_array[len] != '\0')
        {
            int cp = int(char_array[len]);
            if (cp == 0 or cp == 0xfffd or _is_control(char_array[len]))
                continue;
            if (_is_whitespace(char_array[len]))
                output = output + " ";
            else
                output = output + char_array[len];
            ++len;
        }
    return output;
}

vector<string> BasicTokenizer::_run_split_on_punc(string text)
{
    // vector<string> never_split = {"[UNK]", "[SEP]", "[PAD]", "[CLS]", "[MASK]"};
    if (find(never_split_.begin(), never_split_.end(), text) != never_split_.end())
    {
        vector<string> temp =  {text};
        return temp;
    }
    int len_char_array = text.length();
    char * char_array = new char [text.length()+1];
    strcpy (char_array, text.c_str());
    int i = 0;
    bool start_new_word = true;
    vector<vector<char>> output;
    while(i < len_char_array)
    {
        char letter = char_array[i];
        if (_is_punctuation(letter))
        {
            vector<char> temp =  {letter};
            output.push_back(temp);
            start_new_word = true;
        }
        else
        {
            if (start_new_word)
            {
                vector<char> temp_2;
                output.push_back(temp_2);
            }
            start_new_word = false;
            output.back().push_back(letter);
        }
        i += 1;
    }
    vector<string> final_output;
    vector<vector<char>>::iterator ptr;
    for (ptr = output.begin(); ptr < output.end(); ptr++)
    {
        vector<char> out = *ptr;
        string word = "";
        vector<char>::iterator itr;
        for (itr = out.begin(); itr < out.end(); itr++)
        {
            word = word + *itr;
        }
        final_output.push_back(word);
    }
    return final_output;
}

string BasicTokenizer::_run_strip_accents(string text)
{
    wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
    auto temp =  conv.from_bytes(text);
    auto nfd = [](u32string str) { uninorms::nfd(str); return str; };
    auto text_ = nfd(temp);
    string output;
    int i = 0;
    int len_char_array = text_.length()+1;
    char * char_array = new char [text_.length() + 1];
    int j;
    for (j = 0; j < len_char_array; j++)
    {
        char_array[j] = text_[j];
    }
    while(i < len_char_array)
    {
        long int cat = unicode::category(int(char_array[i]));
        if(cat == categories["Mn"])
        {
            i++;
            continue;
        }
        // if (_is_punctuation(char_array[i]))
        // {
        //     i++;
        //     continue;
        // }
        output = output + char_array[i];
        i++;
    }
    return output;
}

vector<string> BasicTokenizer::tokenize(string text)
{
    text = _clean_text(text);
    vector<string> orig_tokens = whitespace_tokenize(text);
    vector<string> split_tokens;
    vector<string>::iterator itr;
    for (itr = orig_tokens.begin(); itr < orig_tokens.end(); itr++)
    {
        string temp = *itr;
        if (do_lower_case_ and not bool(find(never_split_.begin(), never_split_.end(), *itr) != never_split_.end()))
        {
            transform(temp.begin(), temp.end(), temp.begin(),[](unsigned char c){ return std::tolower(c); });;
            temp = _run_strip_accents(temp);
        }
        vector<string> split = _run_split_on_punc(temp);
        split_tokens.insert( split_tokens.end(), split.begin(), split.end() );
    }
    string temp_text;
    vector<string>::iterator ptr;
    for (ptr = split_tokens.begin(); ptr < split_tokens.end(); ptr++)
    {
        temp_text = temp_text + " "+ *ptr;
    }
    return whitespace_tokenize(temp_text);
}


void WordpieceTokenizer::add_vocab(map<string,int> vocab)
{
    vocab_ = vocab;
    unk_token_ = "[UNK]";
    max_input_chars_per_word_ = 100;
}

vector<string> WordpieceTokenizer::tokenize(string text)
{
    vector<string> output_tokens;
    vector<string> whitespace_tokens = whitespace_tokenize(text);
    vector<string>::iterator ptr;
    for (ptr = whitespace_tokens.begin(); ptr < whitespace_tokens.end(); ptr++)
    {
        // cout<<*ptr<<"\n";
        string token = *ptr;
        int len_char_array = token.length();
        char * char_array = new char [token.length()+1];
        strcpy (char_array, token.c_str());
        if (len_char_array > max_input_chars_per_word_)
        {
            output_tokens.push_back(unk_token_);
            continue;
        }
        // cout<<len_char_array<<'\n';
        bool is_bad = false;
        int start = 0;
        vector<string> sub_tokens;
        while(start < len_char_array)
        {
            int end = len_char_array;
            string cur_substr = "";
            while(start < end)
            {
                string substr;
                for(int c = start;c < end; c++)
                    substr = substr + char_array[c];
                if (start > 0)
                    substr = "##" + substr;
                if (vocab_.count(substr) == 1)
                {
                    cur_substr = substr;
                    break;
                }
                end = end - 1;
            }
            if(cur_substr == "")
            {
                is_bad = true;
                break;
            }
            sub_tokens.push_back(cur_substr);
            start = end;
        }
        if(is_bad)
            output_tokens.push_back(unk_token_);
        else
        {
            output_tokens.insert( output_tokens.end(), sub_tokens.begin(), sub_tokens.end() );
        }
    }
    return output_tokens;
}


void BertTokenizer::add_vocab(const char* vocab_file)
{
    vocab = read_vocab(vocab_file);
    for (map<string, int>::iterator i = vocab.begin(); i != vocab.end(); ++i)
        ids_to_tokens[i->second] = i->first;
    do_basic_tokenize_ = true;
    do_lower_case_ = false;
    wordpiece_tokenizer.add_vocab(vocab);
    maxlen_ = 512;
}

vector<string> BertTokenizer::tokenize(string text)
{
    vector<string> split_tokens;
    if(do_basic_tokenize_)
    {
        vector<string> temp_tokens = basic_tokenizer.tokenize(text);
        vector<string>::iterator ptr;
        for (ptr = temp_tokens.begin(); ptr < temp_tokens.end(); ptr++)
        {
            vector<string> subtokens = wordpiece_tokenizer.tokenize(*ptr);
            split_tokens.insert(split_tokens.end(),subtokens.begin(),subtokens.end());
        }
    }
    else
    {
        split_tokens = wordpiece_tokenizer.tokenize(text);
    }
    return split_tokens;
}

vector<float> BertTokenizer::convert_tokens_to_ids(vector<string> tokens)
{
    vector<float> ids;
    vector<string>::iterator ptr;
    for (ptr = tokens.begin(); ptr < tokens.end(); ptr++)
    {
        ids.push_back(float(vocab[*ptr]));
    }
    if (ids.size() > maxlen_)
        cout<<"Token indices sequence length is longer than the specified maximum";
    return ids;
}
