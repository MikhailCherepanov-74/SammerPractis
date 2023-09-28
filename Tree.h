#pragma once
#include<string>
#include "mpi.h"
using namespace std;

class Node{
public:
    string str;
    double probability;
    Node* left;
    Node* right;
    Node* parent;
    Node(string s,double prob,Node*l=NULL,Node*r=NULL,Node*p=NULL){
        str=s;
        probability=prob;
        left=l;
        right=r;
        parent=p;
    };
    bool operator<(const Node other){
        return probability<other.probability;
    }
    bool operator>(const Node other){
        return probability>other.probability;
    }
};
bool comp (Node* a,Node* b) {
    return a->probability > b->probability;
}
map<string,vector<bool>>&CreateTable(Node* root){
    static map<string,vector<bool>> table_map;
    static vector<bool> code;
    if(root->left!=NULL){
        code.push_back(0);
        CreateTable(root->left);
    }
    if(root->right!=NULL){
        code.push_back(1);
        CreateTable(root->right);
    }
    if(root->right==NULL&&root->left==NULL)
        table_map[root->str]=code;
    code.pop_back();
    return table_map;
}

map<string,vector<bool>>&CreateTable1(Node* root1){
    static map<string,vector<bool>> table_map1;
    static vector<bool> code1;
    if(root1->left != NULL){
        code1.push_back(0);
        CreateTable1(root1->left);
    }
    if(root1->right != NULL){
        code1.push_back(1);
        CreateTable1(root1->right);
    }
    if(root1->right == NULL && root1->left == NULL)
        table_map1[root1->str]=code1;
    code1.pop_back();
    return table_map1;
}



string DecodingHuffman(string str_encoding, Node* root){
    stringstream str_stream;
    string h_str="";
    Node *help_root=root;
    for(int i=0;i<str_encoding.size();i++){
        if(str_encoding[i]=='0'&&help_root->left!=NULL){
            help_root=help_root->left;
            continue;
        }
        if(str_encoding[i]=='1'&&help_root->right!=NULL){
            help_root=help_root->right;
            continue;
        }
        str_stream<<help_root->str;
        h_str+=help_root->str;
        help_root=root;
        i--;
    }
    h_str+=help_root->str;
    return h_str;
}
/////////////////////////////////////////other
void FillFile( wfstream &file){
    wstring SIMVOLS=L"уфхцчшУФХЦЧШ 0123456789&,. \n";

    //file<<"rfrfrf";
    int lemit=SIMVOLS.size();
    srand(time(0));
    int n;
    for(int i=0;i<10000;i++){
        n=rand()%lemit;
        file<<SIMVOLS[n];
    }
}
map<char,double>FillPropability(ifstream &file,string & str_){
    stringstream all;
    all<<file.rdbuf();
    string all_str;
    all_str=all.str();
    str_=all_str;
    map<char,double> help_map;
    int amount=all_str.size();
    for(int i=0;i<amount;i++){
        help_map[all_str[i]]++;
    }
    map<char,double>::iterator it=help_map.begin();
    for(;it!=help_map.end();it++){
        it->second=it->second/amount;
    }

    return help_map;
}
string DecodingRLE(string all_str){
    /* stringstream all;
     all<<file.rdbuf();
     string all_str;
     all_str=all.str();*/
    stringstream returned;
    for(int i=0;i<all_str.size();i++){
        if(all_str[i]!='@')
            returned<<all_str[i];
        else{
            if(all_str[i+2]=='@'){
                int counter=stoi(string(1,all_str[i+1]))*10+stoi(string(1,all_str[i+3]));
                while(counter!=0){
                    returned<<all_str[i+4];
                    counter--;
                }
                i+=4;
            }
            else {
                i++;
                int counter = stoi(string(1, all_str[i]));
                i++;
                while (counter != 0) {
                    returned << all_str[i];
                    counter--;
                }
            }
        }
    }
    return returned.str();
}
double CodePrice(map<string,vector<bool>> table_vec,map<char,double>table_prop){
    double sum=0;
    map<char,double>::iterator it =table_prop.begin();
    for(;it!=table_prop.end();it++){
        sum+=table_vec[string(1,it->first)].size()*it->second;
    }
    return sum;
}

string RLEGetStr(string in){
    stringstream str_stream;
    int counter=1;
    for(int i=0;i<in.size()-1;i++){
        if(in[i]==in[i+1]){
            counter++;
            continue;
        }
        if(counter!=1) {
            if (counter >= 10 && counter < 100) {
                str_stream << "@" << to_string(counter / 10) << "@" << to_string(counter % 10) << in[i - 1];
            } else
                str_stream << "@" << to_string(counter) << in[i - 1];
        }
        else
            str_stream<<in[i];
        counter=1;
    }
    if(counter!=1) {
        if (counter >= 10 && counter < 100) {
            str_stream << "@" << to_string(counter / 10) << "@" << to_string(counter % 10) << in[in.size() - 1];
        } else
            str_stream << "@" << to_string(counter) << in[in.size() - 1];
    }
    else
        str_stream<<in[in.size()-1];
    counter=1;
    return str_stream.str();
}
Node* CreatTree( map<char,double> probability){
    vector<Node*> vec_layout;
    map<char,double>::iterator it1=probability.begin();

    for(;it1!=probability.end();it1++){
        vec_layout.push_back( new Node(string(1,it1->first),it1->second));
    }
    while(vec_layout.size()!=1){
        sort(vec_layout.begin(),vec_layout.end(),comp);
        Node* new_node = new Node(vec_layout[vec_layout.size()-1]->str + vec_layout[vec_layout.size()-2]->str,
                                  vec_layout[vec_layout.size()-1]->probability+ vec_layout[vec_layout.size()-2]->probability,
                                  vec_layout[vec_layout.size()-1],vec_layout[vec_layout.size()-2],NULL);
        vec_layout.pop_back();
        vec_layout.pop_back();
        vec_layout.push_back(new_node);
    }
    Node*root =vec_layout.back();
    return root;
}
/*
void ParalF(int argc, char **argv){
    int size,world_rank;
    MPI_Init(&argc, &argv);
     MPI_Comm_size(MPI_COMM_WORLD, &size);   // узнает количество потоков
     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);   // узнает номер потока
     cout<<world_rank<<endl;
    MPI_Finalize();
}*/
// ///////////////////////////////////// MPI

string GetRandStr(int N){
    string SIMVOLS="abcdifABCDIF 0123456789&,. \n";
    string returned;
    //file<<"rfrfrf";
    int lemit=SIMVOLS.size();

    int n;
    for(int i=0;i<N;i++){
        n=rand()%lemit;
        returned.push_back(SIMVOLS[n]);
    }
/*    for(int i=0;i<returned.size();i++){
        wcout<<returned[i];
    }cout<<endl;*/
    return returned;
}
struct SimvolAndAmount{
    char simvol;
    int amount;
    SimvolAndAmount(char a,int b){
        simvol=a;
        amount=b;
    }
    SimvolAndAmount(){
        simvol=0;
        amount='+';
    }
    void Print(){
        cout<<"simvol: "<<simvol<<endl;
        cout<<"amount: "<<amount<<endl<<endl;
    }
};
struct SimvolAndCodeSimvol{
    char simvol;
    int mass_code[10];//в худшем случае закодируем 128 символов и один символ указывает конец кода
    int amount_bit=10;
    void Print(){
        cout<<"simvol: "<<simvol<<" code: ";
        int i=0;
        int counter=0;
        while(mass_code[i]!=3&&counter<amount_bit){
            cout<<mass_code[i];
            i++;
            counter++;
        }
        cout<<endl;
    }
    SimvolAndCodeSimvol(){
        simvol='a';
        for(int i=0;i<amount_bit;i++)
            mass_code[i]=3;
    }
    SimvolAndCodeSimvol(char s){
        simvol=s;
        for(int i=0;i<amount_bit;i++)
            mass_code[i]=3;
    }
};