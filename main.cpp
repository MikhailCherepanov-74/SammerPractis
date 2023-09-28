#include <cstring>// для mempcy
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <fstream>
#include<map>
#include <clocale>
#include <sstream>
#include <vector>
#include<algorithm>
#include <list>
#include"Tree.h"
#include "mpi.h"
#define FillRandom
using namespace std;
int SIZE_STR = 50000;
int main(int argc, char **argv) {
    //setlocale(NULL, "Russian");
    //функции для каждого процесса:

    int AMOUNT_FLOW, RANK_NUMBER;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &AMOUNT_FLOW);   // узнает количество потоков
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK_NUMBER);   // узнает номер потока
    ifstream file_in_first;
    ofstream file_encoding_Huffman;
    SIZE_STR=(SIZE_STR/AMOUNT_FLOW)*AMOUNT_FLOW;/// illegal cheating
    if (RANK_NUMBER == 0) {
        srand(time(0));

        //file_encoding_Huffman.open("/home/mikhail/ Разработка/Clion/0507mpi/encodingHuffman.txt", ios::out);
        file_encoding_Huffman.open("encodingHuffman.txt", ios::out);
        if (!file_encoding_Huffman.is_open()) {
            cout << "file error";
            return 0;
        }
        //  file_in_first = ifstream("/home/mikhail/ Разработка/Clion/0507mpi/Original.txt");
        file_in_first = ifstream("Original.txt");
        if (!file_in_first) {
            cerr << "File error." << endl;
            return 1;
        }
    }
#ifdef FillRandom

    char *sending_buff=new char[SIZE_STR / AMOUNT_FLOW];
    char *getting_buff;
    string returned_random = GetRandStr(SIZE_STR / AMOUNT_FLOW);
    //cout<<"STR f 1:"<<returned_random<<endl;

    for(int i=0;i<returned_random.size();i++){
        sending_buff[i]=returned_random[i];
        // cout<<sending_buff[i];
    }//cout<<endl;
    if(RANK_NUMBER==0){
        getting_buff =new char[SIZE_STR+2];
    }
    MPI_Gather(sending_buff, SIZE_STR / AMOUNT_FLOW, MPI_CHAR,
               getting_buff, SIZE_STR / AMOUNT_FLOW, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(RANK_NUMBER==0){
        fstream file_original;
        file_original.open("Original.txt", ios::out);//app - добавляем в конец файла
        if (!file_original.is_open()) {
            cout << "file error";
            return 0;
        }
        if(SIZE_STR % AMOUNT_FLOW==1){
            string returned_random = GetRandStr(1);
            getting_buff[SIZE_STR-1]=returned_random[0];
        }
        if(SIZE_STR % AMOUNT_FLOW==2){
            string returned_random = GetRandStr(2);
            getting_buff[SIZE_STR-2]=returned_random[0];
            getting_buff[SIZE_STR-1]=returned_random[1];
        }
        for(int i=0;i<SIZE_STR;i++)
            file_original<<getting_buff[i];
        file_original.close();
        // cout<<"Записали в файл";
        delete[]getting_buff;
    }
    delete[] sending_buff;
    MPI_Barrier(MPI_COMM_WORLD);

#endif

//////////////////////////////////////////////////////составляем таблицу для вероятностей

    file_in_first = ifstream("Original.txt");
    int size_all_str;
    string str_from_file((std::istreambuf_iterator<char>(file_in_first)),
                         std::istreambuf_iterator<char>());
    char** str_for_each_proc=new char*[AMOUNT_FLOW];
    if (RANK_NUMBER == 0) {



        for(int i=0;i<AMOUNT_FLOW;i++){
            str_for_each_proc[i]=new char[SIZE_STR / AMOUNT_FLOW + 5];
        }

        for (int i = 0; i < str_from_file.size() / AMOUNT_FLOW; i++) {

            for(int j=0;j<AMOUNT_FLOW;j++){
                str_for_each_proc[j][i]=str_from_file[i+(str_from_file.size() / AMOUNT_FLOW)*j];
            }
        }

        for(int j=0;j<AMOUNT_FLOW;j++){
            str_for_each_proc[j][str_from_file.size() / AMOUNT_FLOW]='\0';
        }

        cout<<RANK_NUMBER<<endl;
        size_all_str = str_from_file.size()*2;

        for(int i=1;i<AMOUNT_FLOW;i++){
            MPI_Send(str_for_each_proc[i], str_from_file.size() / AMOUNT_FLOW + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
           // MPI_Send(&size_all_str, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        cout<<RANK_NUMBER<<endl;
    }
                                                                                                     ///  MPI_Barrier(MPI_COMM_WORLD); вот тут ошибка была!!!!!!
    cout<<RANK_NUMBER<<" "<<str_from_file.size()<<endl;
    size_all_str = str_from_file.size()+2;
    int amount_simvols_in_str_each_proc=size_all_str / AMOUNT_FLOW+1;
    char *str_each_proc = new char[size_all_str / AMOUNT_FLOW + size_all_str*2]; //  я не знаю почему не могу принять размер сообщения равный этому сообщению
    if(RANK_NUMBER!=0)
    MPI_Recv(str_each_proc, str_from_file.size() / AMOUNT_FLOW + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE); // каждый процесс примет сообщения
    else{
        memcpy(str_each_proc,str_for_each_proc[0],str_from_file.size()+1);
    }
    cout<<RANK_NUMBER<<endl;
    MPI_Barrier(MPI_COMM_WORLD);

    ////////////////////////////////////////////////////// составим таблицу вероятности

    map<char, double> probability;
    int i = 0;
    map<char, int> sim_and_amount;
    while (str_each_proc[i] != '\0') {
        sim_and_amount[str_each_proc[i]]++;
        i++;
    }
    vector<SimvolAndAmount> vec_struct;
    map<char, int>::iterator it = sim_and_amount.begin();
    //cout << "rank:  " << RANK_NUMBER;
    for (; it != sim_and_amount.end(); it++) {
        vec_struct.push_back(SimvolAndAmount(it->first, it->second));

    }

    int buffer_size = vec_struct.size() * sizeof(SimvolAndAmount);
    char *buffer_each = new char[buffer_size];
    char *buffer_general;
    if (RANK_NUMBER == 0) {
        buffer_general = new char[AMOUNT_FLOW*100 * sizeof(SimvolAndAmount)];// максимальное число различных символов <100
    }

    memcpy(buffer_each, vec_struct.data(), buffer_size);
    vector<int> displs(AMOUNT_FLOW);
    vector<int> sent_counts(AMOUNT_FLOW);
    cout<<"buffer_size=" <<buffer_size<<endl;
    MPI_Allgather(&buffer_size, 1, MPI_INT,
                  sent_counts.data(), 1, MPI_INT,
                  MPI_COMM_WORLD);
    displs[0] = 0;
    for(int i=1;i<AMOUNT_FLOW;i++){
        displs[i]=0;
        for(int j=i;j>0;j--){
            displs[i]+= sent_counts[j-1];
        }
        cout<<"dis"<<displs[i]<<endl;
    }

    MPI_Gatherv(buffer_each, buffer_size, MPI_CHAR,
                buffer_general, sent_counts.data(), displs.data(), MPI_CHAR, 0, MPI_COMM_WORLD);

    vector<int> sum_size(AMOUNT_FLOW);
    MPI_Gather(&buffer_size, 1, MPI_INT,
               sum_size.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (RANK_NUMBER == 0) {
        int general_size = 0;
        for(int i=0;i<AMOUNT_FLOW;i++){
            general_size+=sum_size[i];
        }
        cout<<" Gen size= "<<general_size<<endl;
        vector<SimvolAndAmount> general_vec_struct(general_size / sizeof(SimvolAndAmount));
        cout<<general_vec_struct.size();
        memcpy(general_vec_struct.data(), buffer_general, general_size);
        cout<<"vec_struct_size= "<<general_vec_struct.size()<<endl;
        for (int i = 0; i < general_vec_struct.size(); i++) {
           // general_vec_struct[i].Print();
            probability[general_vec_struct[i].simvol] += general_vec_struct[i].amount;
        }
        map<char, double>::iterator it = probability.begin();
        for (; it != probability.end(); it++) {
            it->second /= SIZE_STR;
            cout<<"simvol: "<<it->first<<" prob: "<<it->second<<endl;
        }
    }

    delete buffer_each;
    if(RANK_NUMBER==0)
        delete buffer_general;
    MPI_Barrier(MPI_COMM_WORLD);


////////////////////////////////////////////////////////////закончили с таблицей
    Node *root;
    map<string, vector<bool>> table_map;
    int  buffer_for_vec_struct_size;
    vector<SimvolAndCodeSimvol> vec_struct_code;

    if (RANK_NUMBER == 0) {
        root = CreatTree(probability);
        table_map = CreateTable(root);                                         //////Составили вектора кода для каждого символа

        for_each(table_map.begin(), table_map.end(), [&vec_struct_code](
                const pair <string, vector<bool>> &pair) {  //// преобразуем map в вектор структур удобный для пересылки
            SimvolAndCodeSimvol local;
            local.simvol = pair.first[0];
            for (int i = 0; i < pair.second.size(); i++) {
                local.mass_code[i] = (int) pair.second[i];
            }
            local.mass_code[pair.second.size()] = 3;
            vec_struct_code.push_back(local);
        });
        buffer_for_vec_struct_size=sizeof(SimvolAndCodeSimvol)* vec_struct_code.size();
        for(int i=0;i<AMOUNT_FLOW;i++)
            MPI_Send(&buffer_for_vec_struct_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }




    MPI_Recv(&buffer_for_vec_struct_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    char *buffer_for_vec_struct=new char[buffer_for_vec_struct_size];

    if(RANK_NUMBER==0) {
        memcpy(buffer_for_vec_struct, vec_struct_code.data(), buffer_for_vec_struct_size);
        for (int i = 1; i < AMOUNT_FLOW; i++)
            MPI_Send(buffer_for_vec_struct, buffer_for_vec_struct_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
    if(RANK_NUMBER!=0)
        MPI_Recv(buffer_for_vec_struct, buffer_for_vec_struct_size, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    vec_struct_code.clear();
    for(int i=0;i<(buffer_for_vec_struct_size/sizeof(SimvolAndCodeSimvol))+1;i++){
        vec_struct_code.push_back(SimvolAndCodeSimvol());
    }

    memcpy(vec_struct_code.data(), buffer_for_vec_struct, buffer_for_vec_struct_size);





    if(RANK_NUMBER==0)
        for(int i=0;i<vec_struct_code.size();i++){
            vec_struct_code[i].Print();
        }

    ////////////////////////////////Кодируем файл
    vector<int> encoding_vec_every_proc;
    for(int i=0;i<amount_simvols_in_str_each_proc;i++)
        for(int j=0;j<vec_struct_code.size();j++)
            if(vec_struct_code[j].simvol==str_each_proc[i]) {
                int k=0;
                while (vec_struct_code[j].mass_code[k]!=3&&k<vec_struct_code[j].amount_bit) {
                    encoding_vec_every_proc.push_back(vec_struct_code[j].mass_code[k]);
                    k++;
                }
            }
    int sending_size=encoding_vec_every_proc.size();

    MPI_Allgather(&sending_size, 1, MPI_INT,
                  sent_counts.data(), 1, MPI_INT,
                  MPI_COMM_WORLD);
    displs[0]=0;
    for(int i=1;i<AMOUNT_FLOW;i++){
        displs[i]=0;
        for(int j=i;j>0;j--){
            displs[i]+= sent_counts[j-1];
        }
    }
    int encoding_vec_general_size=0;
    for(int i=0;i<AMOUNT_FLOW;i++)
        encoding_vec_general_size+=sent_counts[i];
    vector<int>encoding_vec_general(encoding_vec_general_size);
    // cout<<"size:  "<<sent_counts[0]+sent_counts[1]+sent_counts[2]<<  "  "<<amount_simvols_in_str_each_proc;
    MPI_Gatherv(encoding_vec_every_proc.data(), sending_size, MPI_INT,
                encoding_vec_general.data(), sent_counts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);
    if(RANK_NUMBER==0){
        for(int i=0;i<encoding_vec_general.size();i++){
            //   cout<<encoding_vec_general[i];
            file_encoding_Huffman<<encoding_vec_general[i];
        }
        // cout<<endl<<"Ok"<<endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);// остановили все процессы, пока не выполнится главный
    // file_encoding_Huffman.close();
    file_encoding_Huffman.close();
    //////////////////////////////////////////// Закодировали файл

    ///////////////////////////////Декодируем файл
    if(RANK_NUMBER==0) {
        ifstream file_encodingHuffman_in("encodingHuffman.txt");
        if (!file_encodingHuffman_in) {
            cerr << "File error." << endl;
            return 1;
        }
        string str_encoding;
        if (!getline(file_encodingHuffman_in, str_encoding)) {
            cout << "error reading" << endl;
            return 0;
        }
        ofstream file_decodingHuffman;
        file_decodingHuffman.open("decodingHuffman.txt", ios::out);
        file_decodingHuffman << DecodingHuffman(str_encoding, root);
        file_decodingHuffman.close();
        file_encodingHuffman_in.close();

        cout << "Code price:  " << CodePrice(table_map, probability) << endl;
    }

////////////////////////////////////////////////////////////Задание 2 RLE
    if(RANK_NUMBER==0) {
        ofstream file_encoding_RLE;
        file_encoding_RLE.open("encodingRLE.txt", ios::out);
        file_in_first = ifstream("Original.txt");
        string str_from_file((std::istreambuf_iterator<char>(file_in_first)),
                             std::istreambuf_iterator<char>());
        string strRLE = RLEGetStr(str_from_file);
        file_encoding_RLE << strRLE;
        file_encoding_RLE.close();
        ifstream file_encoding_RLE_from("encodingRLE.txt");
        if (!file_encoding_RLE_from) {
            cerr << "File error." << endl;
            return 1;
        }
        ofstream file_decoding_RLE;
        file_decoding_RLE.open("decodingRLE.txt", ios::out);

        stringstream all1;
        all1 << file_encoding_RLE_from.rdbuf();
        string all_str1;
        all_str1 = all1.str();

        file_decoding_RLE << DecodingRLE(all_str1);
        file_decoding_RLE.close();
        file_encoding_RLE_from.close();
    }

////////////////////////////////////////////////////////////Задание 3 RLE+Хаффман
/////////////////////////////////////////////////////// Кодируем RLE+Хаффман:
    if(RANK_NUMBER==0) {
        ifstream file_encoding_RLE1("encodingRLE.txt");
        ofstream file_encoding_Huffman_RLE;
        file_encoding_Huffman_RLE.open("encodingHuffman_RLE.txt", ios::out);
        string str_from_file1;
        map<char, double> probability1 = FillPropability(file_encoding_RLE1, str_from_file1);
        Node *root1 = CreatTree(probability1);
        map<string, vector<bool>> table_map1 = CreateTable1(root1);

        stringstream str_stream1;
        for (int i = 0; i < str_from_file1.size(); i++) {
            for_each(table_map1[string(1, str_from_file1[i])].begin(),
                     table_map1[string(1, str_from_file1[i])].end(),
                     [&str_stream1](bool a) { str_stream1 << a; });
        }

        string strRLE1 = str_stream1.str();
        file_encoding_Huffman_RLE << strRLE1;
        file_encoding_Huffman_RLE.close();


//////////////////////////////////////////////////////////Декодируем Хаффмен+RLE:
        ifstream max_encoding_from("encodingHuffman_RLE.txt");
        ofstream max_decoding_in("decodingHuffman_RLE.txt", ios::out);
        stringstream all2;
        all2 << max_encoding_from.rdbuf();
        string all_str2;
        all_str2 = all2.str();
        string help_str;
       //help_str.reserve(100000);
        help_str = DecodingRLE(all_str2);
        string help_str1;
        //help_str1.reserve(100000);
        help_str1 = DecodingHuffman(help_str, root1);
        string help_str2;
       // help_str2.reserve(100000);
        help_str2 = DecodingRLE(help_str1);
        max_decoding_in << help_str2;
        max_decoding_in.close();
        max_encoding_from.close();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(RANK_NUMBER==0){
        ifstream f1("decodingHuffman_RLE.txt");
        ifstream f2("Original.txt");
        string s1((std::istreambuf_iterator<char>(f1)),
                  std::istreambuf_iterator<char>());
        string s2((std::istreambuf_iterator<char>(f2)),
                  std::istreambuf_iterator<char>());
        bool flag=1;
        for(int i=0;i<s2.size();i++){
            if(s1[i]!=s2[i])
                flag=0;
        }
        if(flag ==0) {
            cout << "Decoding Huffman+RLE is WRONG!" << endl;
            cout<<endl<<endl<<s1<<endl<<s2<<endl<<endl;
        }
        else
            cout<<"Decoding Huffman+RLE is RIGHT!"<<endl;

    }

    MPI_Barrier(MPI_COMM_WORLD);
   delete[]str_each_proc;
   delete[]buffer_for_vec_struct;


    cout<<"Process: "<<RANK_NUMBER<<" final";
    if(RANK_NUMBER==0){
        file_encoding_Huffman.close();
        // file_in_first.close();
    }

    MPI_Finalize();
}
