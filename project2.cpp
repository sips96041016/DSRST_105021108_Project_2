#include <fstream>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <stack>
#include <utility>
#include <algorithm>
//#include <iomanip>//setw
#define triple(x,y,z) make_pair(x,make_pair(y,z))
using namespace std;

class DIR{
public:
    DIR(int total_unvisit=0, int direction=-1){ tot_unvisit = total_unvisit; dir = direction; }
    void operator= (DIR f){ tot_unvisit = f.tot_unvisit; dir = f.dir; }
    bool operator== (DIR f){ return( tot_unvisit == f.tot_unvisit && dir == f.dir ); }
    bool operator!= (DIR f){ return !(operator==(f)); }
    int tot_unvisit;
    int dir;
};
const DIR unknown,start_point(0,4);
//1 1 1
//1 R 1
//1 1 1

const char dir_c[4] = {'u','d','l','r'};

fstream fs;
queue<int> que;
stack<int> stk[4];// far at top ,near at button
pair <int,int> lu(0,0),ru(0,0),ld(0,0),rd(0,0),lr(0,0),ud(0,0),zero(0,0);

void update_dist_and_pairs(int **dist,char **dir,bool **visited,int x,int y,int a,int b){
    if( dist[a][b]!=-1 ){
        if(!visited[a][b])
            { que.push(a); que.push(b); visited[a][b] = true; dist[a][b] = dist[x][y]+1; if( dir[x][y]!='o' ) dir[a][b] = dir[x][y]; }
        else{ //find min turn path
            if( dir[x][y]=='d' ){
                if( ud == zero && dir[a][b]=='u' ) ud = make_pair (a,b);
            }
            else if( dir[x][y]=='l' ){
                if( lu == zero && dir[a][b]=='u' ) lu = make_pair (a,b);
                else if( ld == zero && dir[a][b]=='d' ) ld = make_pair (a,b);
            }
            else if( dir[x][y]=='r' ){
                if( ru == zero && dir[a][b]=='u' ) ru = make_pair (a,b);
                else if( rd == zero && dir[a][b]=='d' ) rd = make_pair (a,b);
                else if( lr == zero && dir[a][b]=='l' ) lr = make_pair (a,b);
            }
        }
    }
}

void bfs(int **dist,char **dir,bool **visited,int x,int y,int n,int m){
    int i,j,k,a,b;
    for(i=1;i<=n;i++) for(j=1;j<=m;j++) visited[i][j] = false;
    que.push(x); que.push(y); visited[x][y] = true; dir[x][y] = 'o';
    dir[x-1][y] = 'u'; dir[x+1][y] = 'd'; dir[x][y-1] = 'l'; dir[x][y+1] = 'r';
    while(!que.empty()){
        x = que.front(); que.pop(); y = que.front(); que.pop();
        for(k=0;k<4;k++) if( dir[x][y] == dir_c[k] ) { stk[k].push(y); stk[k].push(x); break; }
        update_dist_and_pairs(dist,dir,visited,x,y,x-1,y);//u
        update_dist_and_pairs(dist,dir,visited,x,y,x+1,y);//d
        update_dist_and_pairs(dist,dir,visited,x,y,x,y-1);//l
        update_dist_and_pairs(dist,dir,visited,x,y,x,y+1);//r
    }
}

void get_stk_top(int &x,int &y,int k){ x = stk[k].top(); stk[k].pop(); y = stk[k].top(); stk[k].pop(); }

void update_from(int **dist, char **dir, bool **visited, DIR **from, vector<pair<int,pair<int,int>>> &heap_f,
                 int x, int y, int a, int b, int from_dir, bool limit){
    if( dist[a][b]!=-1 && dist[a][b]<dist[x][y] && ( !limit || ( ( dir[a][b]=='o' || dir[a][b]==dir[x][y] ) ) ) && from[a][b]==unknown ){
        int v = from[a][b].tot_unvisit = from[x][y].tot_unvisit + !visited[a][b];
        from[a][b].dir = from_dir;
        heap_f.push_back(triple(v,a,b)); push_heap(heap_f.begin(),heap_f.end());
    }
}

vector<pair<int,int>> from_R(int **dist, char **dir, bool **visited, DIR **from,
                             int R_x, int R_y, int x, int y, int n, int m, bool limit = false){
    vector<pair<int,int>> path;
    vector<pair<int,pair<int,int>>> heap_f;// I want priority queue
    pair<int,pair<int,int>> block;
    int v,i,j;
    for(i=1;i<=n;i++) for(j=1;j<=m;j++) from[i][j] = unknown;
    heap_f.push_back(triple(0,x,y)); from[x][y] = start_point;//start point
    while(!heap_f.empty()){//will never use this condition
        block = heap_f.front(); v = block.first; x = block.second.first; y = block.second.second;
        pop_heap(heap_f.begin(),heap_f.end()); heap_f.pop_back();
        update_from(dist,dir,visited,from,heap_f,x,y,x-1,y,1,limit);//u ,from d
        update_from(dist,dir,visited,from,heap_f,x,y,x+1,y,0,limit);//d ,from u
        update_from(dist,dir,visited,from,heap_f,x,y,x,y-1,3,limit);//l ,from r
        update_from(dist,dir,visited,from,heap_f,x,y,x,y+1,2,limit);//r ,from l
        if(from[R_x][R_y]!=unknown) break;
    }
    x = R_x; y = R_y;
    while(1){
        if( from[x][y].dir==0 ) x--;
        else if( from[x][y].dir==1 ) x++;
        else if( from[x][y].dir==2 ) y--;
        else if( from[x][y].dir==3 ) y++;
        else break;
        if( from[x][y] != start_point ) path.push_back(make_pair(x,y));
        else break;
    }
    return path;
}

bool check_and_insert_2(int **dist, bool **visited, vector<pair<int,int>> &path,
                        int front_x,int front_y,int back_x,int back_y,int index,bool strict){
    if( dist[front_x][front_y]>0 && dist[back_x][back_y]>0 &&
        ( ( !visited[front_x][front_y] && !visited[back_x][back_y] ) ||
        ( !strict && ( !visited[front_x][front_y] || !visited[back_x][back_y] ) ) ) ){
        path.insert(path.begin()+index,make_pair(back_x,back_y));
        path.insert(path.begin()+index,make_pair(front_x,front_y));
        visited[front_x][front_y] = true;
        visited[back_x][back_y] = true;
        return true;
    }
    return false;
}

bool try_insert_path_2(int **dist, bool **visited, vector<pair<int,int>> &path,int &index,int &remain_dist,
                vector<pair<int,int>>::reverse_iterator &rit_n, vector<pair<int,int>>::reverse_iterator &rit,bool strict){
    int front_x, front_y, back_x, back_y;
    bool re = false;
    if(rit_n != path.rend()){
        front_x = rit_n->first; front_y = rit_n->second;
        back_x = rit->first; back_y = rit->second;
        if( front_y == back_y ){
            if(check_and_insert_2(dist,visited,path,front_x,front_y-1,back_x,back_y-1,index,strict)) re = true;
            else if(check_and_insert_2(dist,visited,path,front_x,front_y+1,back_x,back_y+1,index,strict)) re = true;
        }
        else if( front_x == back_x ){
            if(check_and_insert_2(dist,visited,path,front_x-1,front_y,back_x-1,back_y,index,strict)) re = true;
            else if(check_and_insert_2(dist,visited,path,front_x+1,front_y,back_x+1,back_y,index,strict)) re = true;
        }
        rit++; rit_n++; index--;
    }
    if(re) remain_dist-=2;
    return re;
}

bool check_and_insert_1(int **dist, bool **visited, vector<pair<int,int>> &path,int cur_x,int cur_y,int ins_x,int ins_y,int index){
    if( dist[ins_x][ins_y]>0 && !visited[ins_x][ins_y] ){
        path.insert(path.begin()+index,make_pair(ins_x,ins_y));
        path.insert(path.begin()+index,make_pair(cur_x,cur_y));
        visited[ins_x][ins_y] = true;
        return true;
    }
    return false;
}

bool try_insert_path_1(int **dist, bool **visited, vector<pair<int,int>> &path,int &index,int &remain_dist,
                       vector<pair<int,int>>::reverse_iterator &rit){
    int cur_x, cur_y;
    bool re = false;
    if(rit != path.rend()){
        cur_x = rit->first; cur_y = rit->second;
        if(check_and_insert_1(dist,visited,path,cur_x,cur_y,cur_x-1,cur_y,index)) re = true;
        else if(check_and_insert_1(dist,visited,path,cur_x,cur_y,cur_x+1,cur_y,index)) re = true;
        else if(check_and_insert_1(dist,visited,path,cur_x,cur_y,cur_x,cur_y-1,index)) re = true;
        else if(check_and_insert_1(dist,visited,path,cur_x,cur_y,cur_x,cur_y+1,index)) re = true;
        rit++; index--;
    }
    if(re) remain_dist-=2;
    return re;
}

vector<pair<int,int>> go(int **dist, bool **visited, vector<pair<int,int>> go_path, vector<pair<int,int>> back_path,
                         int x, int y,int remain_dist){
    int i,j;
    bool success = true;
    vector<pair<int,int>>::iterator it;
    vector<pair<int,int>>::reverse_iterator rit_b,rit_b_n,rit_g,rit_g_n;
    go_path.push_back(make_pair(x,y)); back_path.push_back(make_pair(x,y));
    while( success && remain_dist>1 ){
        i = back_path.size() - 1; j = go_path.size() - 1;
        rit_g = rit_g_n = go_path.rbegin(); rit_b = rit_b_n = back_path.rbegin();
        rit_g_n++; rit_b_n++;
        while( ( rit_b_n != back_path.rend() || rit_g_n != go_path.rend() ) && remain_dist>1 ){
            success = try_insert_path_2(dist,visited,back_path,i,remain_dist,rit_b_n,rit_b,true);
            if(remain_dist<=1) break;
            success = success | try_insert_path_2(dist,visited,go_path,j,remain_dist,rit_g_n,rit_g,true);
            if(success) break;
        }
        if( !success && remain_dist>1 ){
            i = back_path.size() - 1; j = go_path.size() - 1;
            rit_g = rit_g_n = go_path.rbegin(); rit_b = rit_b_n = back_path.rbegin();
            rit_g_n++; rit_b_n++;
            while( ( rit_b_n != back_path.rend() || rit_g_n != go_path.rend() ) && remain_dist>1 ){
                success = try_insert_path_2(dist,visited,back_path,i,remain_dist,rit_b_n,rit_b,false);
                if(remain_dist<=1) break;
                success = success | try_insert_path_2(dist,visited,go_path,j,remain_dist,rit_g_n,rit_g,false);
                if(success) break;
            }
        }
        if( !success && remain_dist>1 ){
            i = back_path.size() - 1; j = go_path.size() - 1;
            rit_g = go_path.rbegin(); rit_b = back_path.rbegin();
            while( ( rit_b != back_path.rend() || rit_g != go_path.rend() ) && remain_dist>1 ){
                success = try_insert_path_1(dist,visited,back_path,i,remain_dist,rit_b);
                if(remain_dist<=1) break;
                success = success | try_insert_path_1(dist,visited,go_path,j,remain_dist,rit_g);
                if(success) break;
            }
        }
    }
    back_path.pop_back();
    vector<pair<int,int>> total_path = go_path;
    for (rit_b = back_path.rbegin() ; rit_b != back_path.rend(); ++rit_b) total_path.insert(total_path.end(),*rit_b);
    return total_path;
}

vector<pair<int,int>> turn_path(int **dist, char **dir, bool **visited, DIR **from, int R_x, int R_y, int x, int y, int n, int m,
                                char a, char b, bool limit){
    vector<pair<int,int>> p1,p2;
    dir[x][y] = a;
    p1 = from_R(dist,dir,visited,from,R_x,R_y,x,y,n,m,true); p1.push_back(make_pair(x,y));
    for (vector<pair<int,int>>::iterator it = p1.begin() ; it != p1.end(); ++it) if(limit) visited[it->first][it->second] = true;
    dir[x][y] = b;
    p2 = from_R(dist,dir,visited,from,R_x,R_y,x,y,n,m,true);
    for (vector<pair<int,int>>::reverse_iterator rit = p2.rbegin() ; rit != p2.rend(); ++rit)
        { if(limit) visited[rit->first][rit->second] = true; p1.push_back((*rit)); }
    return p1;
}

int go_go(int **dist, char **dir, bool **visited, int R_x, int R_y, int n, int m, int max_dist){
    int i,j,k,x,y,total = 0;
    int in_x,in_y,choose,min_len,cur_len, max_stk;
    bool check[4][4] = { { !stk[0].empty() , ( ud!=zero ) , ( lu!=zero ) , ( ru!=zero ) } ,
                        { ( ud!=zero ) , !stk[1].empty() , ( ld!=zero ) , ( rd!=zero ) } ,
                        { ( lu!=zero ) , ( ld!=zero ) , !stk[2].empty() , ( lr!=zero ) } ,
                        { ( ru!=zero ) , ( rd!=zero ) , ( lr!=zero ) , !stk[3].empty() } };
    int how_go[4][4];
    for(i=0;i<4;i++) for(j=0;j<4;j++) how_go[i][j] = ( check[i][j] && i!=j ) ? j : -1 ;
    for(k=0;k<4;k++) for(i=0;i<4;i++) for(j=0;j<4;j++)
        if( i!=j && how_go[i][k]!=-1 && how_go[k][j]!=-1 && how_go[i][j]==-1 ) how_go[i][j] = how_go[i][k];
    bool limit = false;//first go turn_path?
    bool finish[4][4] ={{ true, ( ud==zero ), ( lu==zero ), ( ru==zero )},
                        { ( ud==zero ), true, ( ld==zero ), ( rd==zero )},
                        { ( lu==zero ), ( ld==zero ), true, ( lr==zero )},
                        { ( ru==zero ), ( rd==zero ), ( lr==zero ), true}};
    if(!limit) for(i=0;i<4;i++) for(j=0;j<4;j++) finish[i][j] = true;
    DIR **from = new DIR*[n+2];
    DIR *tmpf = new DIR[(n+2)*(m+2)];
    for(i=0;i<n+2;i++) from[i] = &(tmpf[i*(m+2)]);

    vector<pair<int,int>> go_path, back_path, total_path, lu_path, ru_path, ld_path, rd_path, lr_path, ud_path;
    vector<pair<int,int>> *path[4][4] ={{NULL, &ud_path, &lu_path, &ru_path},
                                        {&ud_path, NULL, &ld_path, &rd_path},
                                        {&lu_path, &ld_path, NULL, &lr_path},
                                        {&ru_path, &rd_path, &lr_path, NULL}};
    vector<pair<int,int>>::iterator it;
    vector<pair<int,int>>::reverse_iterator rit;
    char direction = 'u';
    max_stk = max(max(stk[0].size(),stk[1].size()),max(stk[2].size(),stk[3].size()));
    for(i=0;i<4;i++) if(stk[i].size()==max_stk) direction = dir_c[i];
    for(i=1;i<=n;i++) for(j=1;j<=m;j++) visited[i][j] = false;
    visited[R_x][R_y] = true;

    if( lu!=zero ) lu_path = turn_path(dist,dir,visited,from,R_x,R_y,lu.first,lu.second,n,m,'l','u',limit);//l to u
    if( ru!=zero ) ru_path = turn_path(dist,dir,visited,from,R_x,R_y,ru.first,ru.second,n,m,'r','u',limit);//r to u
    if( ld!=zero ) ld_path = turn_path(dist,dir,visited,from,R_x,R_y,ld.first,ld.second,n,m,'l','d',limit);//l to d
    if( rd!=zero ) rd_path = turn_path(dist,dir,visited,from,R_x,R_y,rd.first,rd.second,n,m,'r','d',limit);//r to d
    if( lr!=zero ) lr_path = turn_path(dist,dir,visited,from,R_x,R_y,lr.first,lr.second,n,m,'r','l',limit);//r to l
    if( ud!=zero ) ud_path = turn_path(dist,dir,visited,from,R_x,R_y,ud.first,ud.second,n,m,'d','u',limit);//d to u

    while(1){
        for(k=0;k<4;k++) if( direction == dir_c[k] ) break;
        x=R_x; y=R_y;
        while ( visited[x][y] && !stk[k].empty() ) get_stk_top(x,y,k);
        if(stk[k].empty()) {
            for(i=0;i<4;i++) {
                x=R_x; y=R_y;
                while( visited[x][y] && !stk[i].empty() ) get_stk_top(x,y,i);
                if(!stk[i].empty()) {stk[i].push(y); stk[i].push(x);}
            }
            choose = 4; min_len = -1;
            for(i=0;i<4;i++) {
                if( !stk[i].empty() && how_go[k][i]!=-1 ) {
                    cur_len = 0;
                    for(k=0;k<4;k++) if( direction == dir_c[k] ) break;
                    while( how_go[k][i] != -1) {
                        go_path = *path[k][how_go[k][i]];
                        for (it = go_path.begin() ; it != go_path.end(); ++it) cur_len++;
                        cur_len++;
                        k = how_go[k][i];
                    }
                    if( min_len==-1 || min_len>cur_len ){ min_len=cur_len; choose=i; }
                }
            }
            if(choose==4) break;
            else {
                for(k=0;k<4;k++) if( direction == dir_c[k] ) break;
                i = choose;
                while( how_go[k][i] != -1) {
                    go_path = *path[k][how_go[k][i]]; finish[k][how_go[k][i]] = finish[how_go[k][i]][k] = true;
                    if(k>how_go[k][i]) for (it = go_path.begin() ; it != go_path.end(); ++it)
                        { que.push(it->first); que.push(it->second); total++; }
                    else for (rit = go_path.rbegin() ; rit != go_path.rend(); ++rit)
                        { que.push(rit->first); que.push(rit->second); total++; }
                    que.push(R_x); que.push(R_y);
                    k = how_go[k][i];
                }
                direction = dir_c[i];
                continue;
            }
        }
        //from R to (x,y)
        go_path = from_R(dist,dir,visited,from,R_x,R_y,x,y,n,m);
        for (it = go_path.begin() ; it != go_path.end(); ++it) visited[it->first][it->second] = true;
        back_path = from_R(dist,dir,visited,from,R_x,R_y,x,y,n,m);
        for (it = back_path.begin() ; it != back_path.end(); ++it) visited[it->first][it->second] = true;
        visited[x][y] = true;

        total_path = go(dist,visited,go_path,back_path,x,y,max_dist-2*dist[x][y]);

        for (it = total_path.begin() ; it != total_path.end(); ++it){ que.push(it->first); que.push(it->second); total++; }
        que.push(R_x); que.push(R_y);
        if( back_path.empty() ) { in_x = x; in_y = y; }
        else { in_x = back_path.front().first; in_y = back_path.front().second; }
        if( in_x == R_x-1 && in_y == R_y ) direction = 'u';
        else if( in_x == R_x+1 && in_y == R_y ) direction = 'd';
        else if( in_x == R_x && in_y == R_y-1 ) direction = 'l';
        else if( in_x == R_x && in_y == R_y+1 ) direction = 'r';
        else break;
    }

    //if not limit ,not use
    for(i=0;i<4;i++) { for(j=0;j<4;j++) if(!finish[i][j]) break; if( j<4 && !finish[i][j] ) break; }
    while( i!=4 || j!=4 ){
        for(k=0;k<4;k++) if( direction == dir_c[k] ) break;
        while( how_go[k][i] != -1) {
            go_path = *path[k][how_go[k][i]]; finish[k][how_go[k][i]] = finish[how_go[k][i]][k] = true;
            if(k>how_go[k][i]) for (it = go_path.begin() ; it != go_path.end(); ++it)
                { que.push(it->first); que.push(it->second); total++; }
            else for (rit = go_path.rbegin() ; rit != go_path.rend(); ++rit)
                { que.push(rit->first); que.push(rit->second); total++; }
            que.push(R_x); que.push(R_y); total++;
            k = how_go[k][i];
            if(finish[i][j]) break;
        }
        if(!finish[i][j]){
            go_path = *path[k][j]; finish[k][j] = finish[j][k] = true;
            if(k>j) for (it = go_path.begin() ; it != go_path.end(); ++it)
                { que.push(it->first); que.push(it->second); total++; }
            else for (rit = go_path.rbegin() ; rit != go_path.rend(); ++rit)
                { que.push(rit->first); que.push(rit->second); total++; }
            que.push(R_x); que.push(R_y); total++;
            direction = dir_c[j];
        }
        for(i=0;i<4;i++) { for(j=0;j<4;j++) if(!finish[i][j]) break; if(!finish[i][j]) break; }
    }

    delete [] tmpf; delete [] from;
    return total;
}

int main(void){
    int i,j,n,m,k,max_dist,R_x,R_y;
    char c;
    char **dir,*tmpc;
    bool **visited,*tmpb;
    int **dist,*tmp;
    //input
    fs.open ("floor.data", ios::in );
    fs>>n>>m>>max_dist;

    dir = new char*[n+2];
    tmpc = new char[(n+2)*(m+2)];
    for(i=0;i<n+2;i++) dir[i] = &(tmpc[i*(m+2)]);
    for(i=0;i<n+2;i++) for(j=0;j<m+2;j++) dir[i][j] = '\0';
    visited = new bool*[n+2];
    tmpb = new bool[(n+2)*(m+2)];
    for(i=0;i<n+2;i++) visited[i] = &(tmpb[i*(m+2)]);
    dist = new int*[n+2];
    tmp = new int[(n+2)*(m+2)];
    for(i=0;i<n+2;i++) dist[i] = &(tmp[i*(m+2)]);

    for(j=1;j<=m;j++) { dist[0][j] = -1; dist[n+1][j] = -1; }
    for(i=1;i<=n;i++) {
        dist[i][0] = -1;
        for(j=1;j<=m;j++){
            fs>>c;
            if(c=='1') dist[i][j] = -1;
            else {
                dist[i][j] = 0;
                if(c=='R') { R_x = i; R_y = j; }
            }
        }
        dist[i][m+1] = -1;
    }
    fs.close();
    //find dist
    bfs(dist,dir,visited,R_x,R_y,n,m);
    //output
    fs.open ("final.path", ios::out );
    fs<<go_go(dist,dir,visited,R_x,R_y,n,m,max_dist)<<endl;
    while(!que.empty()){ fs<<que.front()-1<<" "; que.pop(); fs<<que.front()-1<<endl; que.pop(); }
    fs.close();
    delete [] tmp; delete [] dist; delete [] tmpc; delete [] dir; delete [] tmpb; delete [] visited;
    return 0;
}
