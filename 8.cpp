#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <set>
#include <fstream>
#include <string>

using namespace std;

vector <pair<int, char>> global_nm;

void show_binary_tree(map<char, string>& enc){
    cout<<"Бинарное дерево кодов:\n";
    set<string> a;
    for(auto ent: enc){
        string temp = "";
        for(auto i: ent.second){
            if(temp != "")
                a.insert(temp+"->"+temp+i+"[label=\" " + i + " \"];");
            temp += i;
        }
        a.insert(ent.second+"->\""+ ent.first +"\"[label=\" enc \"];");
    }
    a.insert("root->0[label=\" 0 \" ];");
    a.insert("root->1[label=\" 1 \" ];");
    for(auto i: a)cout<<i;cout<<'\n';
}

map<char, string> shenon_encode(std::string& s){
    map<char, string> ans;
    map <char, int> nm;
    for(char c: s){ nm[c] ++; }
    vector <pair<int, char>> nm2;
    for(auto i: nm){ nm2.push_back({i.second, i.first}); }
    vector<int> pref(nm2.size()+1, 0);
    sort(nm2.rbegin(), nm2.rend());
    for(int i=0;i<nm2.size();i++){
        //cout<<nm2[i].first<<' '<<nm2[i].second<<'\n';
        pref[i+1] = pref[i] + nm2[i].first;
    }

    auto f = [&](int l, int m, int r){ return abs(pref[r] - pref[m]*2 + pref[l]); };

    function <void(int, int, string)> recur = [&](int l, int r, string code){
        if(r - l == 1){
            ans[nm2[l].second] = code;
            return;
        }
        int best=l+1;
        for(int i=l+1;i<r;i++){
            //cout<<l<<' '<<i<<' '<<r<<' '<<f(l, i, r)<<'\n';
            if( f(l, best, r) > f(l, i, r) ) best = i;
        }
        recur(l, best, code+"0");
        recur(best, r, code+"1");
    };
    recur(0, nm2.size(), "");


    global_nm.swap(nm2);
    string ans_s;
    for(auto i: s){
        ans_s += ans[i] + " ";
    }
    swap(ans_s, s);
    return ans;
}

map<char, string> haffman_encode(string &s, bool from_file=0){
    map<char, string> ans;
    map <char, int> nm;
    ifstream in;
    if(from_file){
        in.open(s, ios::binary | ios::in);
        if(!in.is_open()){
            cout<<"Файл не найден";
            exit(1);
        }else{
            char c;
            while(in.get(c)){ nm[c] ++; }
        }
    }else for(char c: s){ nm[c] ++; }

    struct nod{
        int amount;
        char let;
        nod *l = 0, *r = 0;
        nod(int am=0, char letter='`') : let(letter), amount(am){}
        ~nod(){ if(l) delete l; if(r) delete r; } 

        void restcode(map<char, string> &ans, string code){
            if(l && r){
                l->restcode(ans, code+"0");
                r->restcode(ans, code+"1");
            }else{
                ans[let] = code;
            }
        }

        std::string start(double sm, string shift, string code=""){
            string v = to_string(int(amount/sm*100+0.5))+"%";
            if(l&&r){
                l->start(sm, shift+"---", code+"0");
                cout<<shift<<v<<" precode: "<<code<<'\n';
                r->start(sm, shift+"---", code+"1");
            }else{
                cout<<shift<<v<<": '"<<let<<"' code: "<<code<<'\n';
            }
            return "\""+v+"\"";
        }
    };

    set<pair<int, nod*>> vert;
    for(auto i: nm){
        vert.insert({i.second, new nod(i.second, i.first)});
    }
    while(vert.size() > 1){
        pair <int, nod *> a = *vert.begin(), b = *++vert.begin(), c;
        vert.erase(vert.begin());
        vert.erase(vert.begin());
        c.first = a.first + b.first;
        c.second = new nod(a.second->amount + b.second->amount);
        c.second->l = a.second;
        c.second->r = b.second;
        vert.insert(c);
    }
    vert.begin()->second->restcode(ans, "");
       
    if(! from_file){
        cout<<"Дерево вероятностей:  \n";
        double sm = vert.begin()->second->amount;
        vert.begin()->second->start(sm, "");
    }
    
    delete vert.begin()->second;

    string ans_s="";
    if(from_file){
        ofstream out("zip_of_" + s, ios::binary | ios::out);
        out<< (unsigned char)(ans.size());
        for(auto i: ans){
            out<<i.first<<(unsigned char)(i.second.size());
            char lol=0;
            int j=0;

            while(i.second.size()&7){
                i.second += "0";
            }

            for(;j<i.second.size()/8;j++){
                unsigned char forwrite=0;
                for(int q=0;q<8;q++){
                    forwrite |= (i.second[j*8+q]-'0')<<q;   
                }
                out<<forwrite;
            }
        }
        char let;
        string bits="";
        in.close();
        in.open(s, ios::binary | ios::in);
        while(in.get(let)){
            bits += ans[let];
            while(bits.size() > 8){
                unsigned char comp = 0;
                for(int i=0;i<8;i++){
                    comp |= (bits[i]-'0')<<i;
                }
                //cout<<(int)comp<<'\n';
                out<<comp;
                bits = bits.substr(8, bits.size());
            }
        }
        while(bits.size() & 7){ bits += "0"; }
        while(bits.size() > 8){
            unsigned char comp = 0;
            for(int i=0;i<8;i++){
                comp |= (bits[i]-'0')<<i;
            }
            out<<comp;
            bits = bits.substr(8, bits.size());
        }

        out.close();
    }else {
        for(auto &i: s){ans_s += ans[i] + " ";}
        s = ans_s;

    }
    

    return ans;
}

void haff_dec(string s){
    ifstream in(s, ios::binary | ios::in);
    if(!in.is_open()){
        cout<<"Файл не найден\n";
        exit(1);
    }
    map<string, char> a;
    ofstream out("decoded_"+s, ios::binary | ios::out);

    unsigned char amount;
    in.read((char*)&amount, sizeof(amount));
    while(amount-->0){
        char let;
        string s;
        in.get(let);
        unsigned char s_size;
        in.read((char*)&s_size, sizeof(s_size));
        while(1){
            unsigned char temp;
            in.read((char*)&temp, sizeof(temp));
            for(int i=0;i<min<int>(8, s_size);i++){
                s += (char)(((temp>>i)&1) + '0');
            }
            if(s_size <= 8)break;
            s_size -= 8;
        }
        a[s] = let;
    }
    string bit="";
    char temp;
    while(in.get(temp)){
        for(int i=0;i<8;i++){
            bit += (char)( ( (temp>>i)&1 ) + '0' );
            if(a.find(bit) != a.end()){
                out<<a[bit];
                bit = "";
            }
        }
    }

    out.close();
}

int main() {
    system("chcp 1251");
    system("cls");
    bool f, f2;
    string s;
    cout<<"Кодировка - 0; Раскодировка - 1 >> ";
    cin>>f;
    
    if(f){
        cout<<"Введите название файла\n";
        cin.get();
        getline(cin, s);
        haff_dec(s);
        return 0;
    }
    cout<<"1 - Метод Хаффмана; 0 - Метод Шеннона-Фано >> ";
    cin>>f2;
    if(f2){
        cout<<"1 - ввод из файла; 0 - с клавиатуры >> ";
        cin>>f;
        if(f)cout<<"Введите название файла\n";
        else cout<<"Введите кодируемый текст\n";
        cin.get();
        getline(cin, s);
        auto q = haffman_encode(s, f);
        if(!f){
            show_binary_tree(q);
            cout<<"Закодированная строка\n"<<s;
        }
    
    }else{
        cout<<"Введите кодируемый текст\n";
        cin.get();
        getline(cin, s);
        auto q = shenon_encode(s);
        show_binary_tree(q);
        cout<<"Закодированная строка\n"<<s;
    
    }
    return 0;
}
