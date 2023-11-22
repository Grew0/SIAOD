#include <iostream>
#include <vector>
#include <set>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iomanip>

struct graph{
    int n;
    std::vector<std::vector<int>> mat;

    void inp(){
        std::cout<<"Введите количество вершин\n";
        std::cin>>n;
        mat.assign(n, std::vector<int>(n, 0));
        int m, u, v, w;
        std::cout<<"Введите количество дуг\n";
        std::cin>>m;
        std::cout<<"Введите дуги в формате: 'u v w', где 1 <= u, v <= "<<n<<"\n\
Будет создана дуга от u к v с весом w\n";
        while(m-->0){
            std::cin>>u>>v>>w;
            mat[--u][--v] = w;
        }
    }

    bool connected(){
        struct dsu{
            std::vector <int> a;
            dsu (int n){
                a.resize(n);
                for(int i=0;i<n;i++)a[i] = i;
            }
            int getPar(int i){ return (i==a[i]? i: a[i] = getPar(a[i])); }
            bool ins(int u, int v){ return getPar(u) == getPar(v); }
            void join(int u, int v){ a[getPar(u)] = getPar(v); }
        };

        dsu d(n);
        for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            if(mat[i][j])
                d.join(i, j);
        for(int i=1;i<n;i++)
            if(!d.ins(i-1, i))
                return false;
        return true;
    }

    int getPath(int u, int v){
        int inf = -1;
        std::vector <int> res(n, inf);
        std::set<std::pair<int, int>> d({{0, u}});
        while(d.size()){
            std::pair<int, int> it = *d.begin();
            d.erase(d.begin());
            if(res[it.second] == inf){
                res[it.second] = it.first;
                for(int i=0;i<n;i++){
                    if(mat[it.second][i]){
                        d.insert({mat[it.second][i]+it.first, i});
                    }
                }
            }
        }
        return res[v];
    }

    void draw(){
        using namespace sf;
        RenderTexture win;
        win.create(600, 600);
        win.clear(Color::White);
        float r = 50 * 2/5;
        CircleShape sp(r);
        sp.setOrigin(r, r);
        sp.setOutlineThickness(10);
        sp.setOutlineColor(Color::Black);

        auto coord = [&](int vert, bool xy){
            return 300+200*sin(vert * 3.1415 * 2 / n + xy * 3.1415 / 2 + 3.1415);
        };

        Font fn;
        fn.loadFromFile("Sans.ttf");

        Text tx;
        tx.setFillColor(Color::Black);
        tx.setFont(fn);

        for(int i=0;i<n;i++)
        for(int j=0;j<n;j++){
            if(!mat[i][j])continue;
            Vector2f _i(coord(i, 0), coord(i, 1));
            Vector2f _j(coord(j, 0), coord(j, 1));
            Vector2f dir = _i - _j, norm;
            dir *= 1 / sqrt(dir.x*dir.x + dir.y*dir.y);
            norm.x = dir.y;
            norm.y = -dir.x;
            
            norm *= 10.f;
            dir *= 40.f;
            Vector2f shift = dir * (r/40);
            Vertex vr[4] = {
                Vertex(_j+shift, Color::Black), 
                Vertex(_j+norm+dir+shift, Color::Black),
                Vertex(_j-norm+dir+shift, Color::Black)
            };

            if(!mat[j][i])
                win.draw(vr, 3, sf::Triangles);

            norm *= .2f;
            vr[0] = Vertex(_j+norm+shift, Color::Black); 
            vr[1] = Vertex(_j-norm+shift, Color::Black); 
            vr[2] = Vertex(_i-norm, Color::Black);
            vr[3] = Vertex(_i+norm, Color::Black);
            
            win.draw(vr, 4, sf::Quads);

        }


        for(int i=0;i<n;i++){
            sp.setPosition(coord(i, 0), coord(i, 1));
            win.draw(sp);
            tx.setPosition(coord(i, 0)-10, coord(i, 1)-20);
            tx.setString(std::to_string(i+1));
            win.draw(tx);

        }
        win.display();
        win.getTexture().copyToImage().saveToFile("img.png");
    }
};

int main() {
    system("chcp 1251");    
    system("cls");
    graph a;
    a.inp();
    std::cout<<"Выберете операцию:\n\
a u v w - добавить дугу (или изменить ее вес)\nd u v - удалить дугу\nc - понять связан ли граф\n\
w u v - получить путь от u до v\np - нарисовать\ne - выход\n";
    while(true){
        char c;
        int u, v, w;
        std::cin>>c;
        switch(c){
        case 'a':
            std::cin>>u>>v>>w;
            a.mat[--u][--v] = w;
            break;
        case 'd':
            std::cin>>u>>v;
            a.mat[--u][--v] = 0;
            break;
        case 'p':
            a.draw();
            for(auto i: a.mat){
                for(auto j: i)
                    std::cout<<std::setw(4)<<j;
                std::cout<<'\n';
            }
            break;
        case 'c':
            std::cout<<"Граф "<<(a.connected()? "": "не ")<<"связан\n";
            break;
        case 'w':
            std::cin>>u>>v;
            w = a.getPath(u-1, v-1);
            std::cout<<(w!=-1? "Кратчайший п":"П")<<"уть от "<<u<<" к "<<v<<": "<<(w == -1? "не существует": std::to_string(w))<<'\n';
            break;
        case 'e':
            return 0;
        default:
            std::cout<<"Неизвестная команда\n";
            break;
        }

    }
    return 0;
}