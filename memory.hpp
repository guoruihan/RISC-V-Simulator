#include<bits/stdc++.h>
using namespace std;

#ifndef _MEMORY
#define _MEMORY

class memory{
public:
    unsigned char mem[4194304];
    int pos;

public:
    memory(){
        pos = 0;
    }
    ~memory(){}

    void write(int d){
        mem[pos++] = d & 0xff;
    }

    void write_w(int mem_pos,int d){
        memcpy(mem_pos + mem, &d, 4);
    }

    void write_h(int mem_pos,short d) {
        memcpy(mem_pos + mem, &d, 2);
    }

    void write_b(int mem_pos, char d){
        memcpy(mem_pos + mem, &d, 1);
    }

    unsigned int load_word(){
        unsigned int t = 0;
        /*for (int i = 0; i < 4; ++i) {
            t = ((int) mem[pos + i] << (8 * i)) | t;
        }*/
        memcpy(&t, mem + pos, 4);
        pos += 4;
        return t;
    }



    unsigned int load_from_pos(int mem_pos, int bytes){

        // if (bytes == 4) {
        //     unsigned int tmp = *(int *)(mem + mem_pos);
        //     return tmp;
        // }
        

        unsigned int t = 0;
        memcpy(&t, mem + mem_pos, bytes);
        return t;
    }

    void load_w(int mem_pos, int &x){
        memcpy(&x, mem_pos + mem, 4);
    }

    void load_h(int mem_pos, short &x){
        memcpy(&x, mem_pos + mem, 2);
    }

    void load_b(int mem_pos, char &x){
        memcpy(&x, mem_pos + mem, 2);
    }

    void change_pos(int d){
        pos = d;
    }

    /*void initialize_mem(){
        char f; int t;
        char line[256];
        while (!cin.eof()){
            cin.getline(line, 256);
            // ???
            char e = line[strlen(line) - 1];
            if (((int) e == 32))
                line[strlen(line) - 1] = '\0';

            stringstream s;
            s << line;
            if (line[0] == '~') return;
            if (line[0] == '@') {
                char ch;
                s >> ch;
                s >> hex >> t;
                change_pos(t);
                continue;
            }
            while (!s.eof()){
                int b1, b2, b3, b4;
                s >> hex >> b4 >> b3 >> b2 >> b1;
                unsigned int temp = (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;
                memcpy(mem + pos, &temp, sizeof(temp));
                //cout << hex << temp << endl;
                pos += 4;
            }
        }
    }*/


    void initialize_mem(){
        char buf[40];
        int t;
        while (scanf("%s", buf) != EOF){
            if (buf[0] == '@') {
                sscanf(buf + 1, "%x", &t);
                change_pos(t);
            }
            else if (buf[0] == '~') return;
            else {
                int b1, b2, b3, b4;
                sscanf(buf, "%x", &b4);
                scanf("%x %x %x", &b3, &b2, &b1);
                unsigned int temp = (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;
                cout << hex << temp << endl;
                memcpy(mem + pos, &temp, sizeof(temp));
                pos += 4;
            }
        }

    }

    void traverse(){
        for (int i = 0; i < 0x2000 / 4; ++i){
            unsigned int temp;
            memcpy(&temp, mem + 4 * i, 4);
            cout << hex << temp << ' ';
            if ((i + 1) % 16 == 0) cout << endl;
        }
    }

};

#endif