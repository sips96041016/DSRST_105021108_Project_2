#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <utility>
#include <algorithm>
#include <iomanip>//setw
#define triple(x,y,z) make_pair(x,make_pair(y,z))
using namespace std;

struct greater_triple{
  bool operator()(pair<int,pair<int,int>>& a,pair<int,pair<int,int>>& b) const{
    return a>b;//> or >=
  }
};
class DIR{
public:
    DIR(int total_visit=0, int direction=-1){ tot_visit = total_visit; dir = direction; }
    void operator= (DIR f){ tot_visit = f.tot_visit; dir = f.dir; }
    bool operator== (DIR f){ return( tot_visit == f.tot_visit && dir == f.dir ); }
    bool operator!= (DIR f){ return !(operator==(f)); }
    int tot_visit;
    int dir;
};
const DIR unknown,start_point(0,4);
//1 1 1
//1 R 1
//1 1 1

const char dir_c[4] = {'u','d','l','r'};

queue<int> que;
stack<int> stk[4];// far at top ,near at button
pair <int,int> lu(0,0),ru(0,0),ld(0,0),rd(0,0),zero(0,0);


void update_dist_and_pairs(int **dist,char **dir,bool **visited,int x,int y,int a,int b){
    if( dist[a][b]!=-1 ){
        if(!visited[a][b])
            { que.push(a); que.push(b); visited[a][b] = true; dist[a][b] = dist[x][y]+1; if( dir[x][y]!='o' ) dir[a][b] = dir[x][y]; }
        else{ //find min turn path
            if( dir[x][y]=='l' ){
                if( lu == zero && dir[a][b]=='u' ) lu = make_pair (a,b);
                else if( ld == zero && dir[a][b]=='d' ) ld = make_pair (a,b);
            }
            else if( dir[x][y]=='r' ){
                if( ru == zero && dir[a][b]=='u' ) ru = make_pair (a,b);
                else if( rd == zero && dir[a][b]=='d' ) rd = make_pair (a,b);
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
                 int x, int y, int a, int b, int from_dir){
    if( dist[a][b]!=-1 && dist[a][b]<dist[x][y] && ( dir[a][b]=='o' || dir[a][b]==dir[x][y] ) && from[a][b]==unknown ){
        int v = from[a][b].tot_visit = from[x][y].tot_visit + visited[a][b];
        from[a][b].dir = from_dir;
        heap_f.push_back(triple(v,a,b)); push_heap(heap_f.begin(),heap_f.end(),greater_triple());
    }
}

vector<pair<int,int>> from_R(int **dist, char **dir, bool **visited, DIR **from, int R_x, int R_y, int x, int y, int n, int m){
    vector<pair<int,int>> path;
    vector<pair<int,pair<int,int>>> heap_f;// I want priority queue
    pair<int,pair<int,int>> block;
    int v,i,j;
    for(i=1;i<=n;i++) for(j=1;j<=m;j++) from[i][j] = unknown;
    heap_f.push_back(triple(0,x,y)); from[x][y] = start_point;//start point
    while(!heap_f.empty()){//will never use this condition
        block = heap_f.front(); v = block.first; x = block.second.first; y = block.second.second;
        pop_heap(heap_f.begin(),heap_f.end(),greater_triple()); heap_f.pop_back();
        update_from(dist,dir,visited,from,heap_f,x,y,x-1,y,1);//u ,from d
        update_from(dist,dir,visited,from,heap_f,x,y,x+1,y,0);//d ,from u
        update_from(dist,dir,visited,from,heap_f,x,y,x,y-1,3);//l ,from r
        update_from(dist,dir,visited,from,heap_f,x,y,x,y+1,2);//r ,from l
        if(from[R_x][R_y]!=unknown) break;
    }
    x = R_x; y = R_y;
    while(1){
        if( from[x][y].dir==0 ) x--;
        else if( from[x][y].dir==1 ) x++;
        else if( from[x][y].dir==2 ) y--;
        else if( from[x][y].dir==3 ) y++;
        else { cout<<from[x][y].dir<<" "<<x<<" "<<y<<" noooooo!!!!!!!"<<endl; break;}
        if( from[x][y] != start_point ) { path.push_back(make_pair(x,y)); visited[x][y] = true; }
        else break;
    }
    return path;
}

int go_go(int **dist, char **dir, bool **visited, int R_x, int R_y, int n, int m, int max_dist){
    int i,j,k,x,y,total = 0;

    DIR **from = new DIR*[n+2];
    DIR *tmpf = new DIR[(n+2)*(m+2)];
    for(i=0;i<n+2;i++) from[i] = &(tmpf[i*(m+2)]);

    vector<pair<int,int>> go_path, back_path;
    vector<pair<int,int>>::iterator it;
    vector<pair<int,int>>::reverse_iterator rit;
    char direction = 'u';
    for(i=1;i<=n;i++) for(j=1;j<=m;j++) visited[i][j] = false;
    visited[0][0] = true;

    while(1){
        for(k=0;k<4;k++) if( direction == dir_c[k] ) break;
        x=0; y=0;
        while ( visited[x][y] && !stk[k].empty() ) get_stk_top(x,y,k);
        if(stk[k].empty()) break;
        //from R to (x,y)
        go_path = from_R(dist,dir,visited,from,R_x,R_y,x,y,n,m);
        for (it = go_path.begin() ; it != go_path.end(); ++it){
            cout<<it->first<<" "<<it->second<<endl;
            total++;
        }
        cout<<x<<" "<<y<<"       0000000000000"<<endl;
        back_path = from_R(dist,dir,visited,from,R_x,R_y,x,y,n,m);
        for (rit = back_path.rbegin() ; rit != back_path.rend(); ++rit){
            cout<<rit->first<<" "<<rit->second<<endl;
            total++;
        }
        cout<<R_x<<" "<<R_y<<"       11111111111111"<<endl;
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
    fstream fs;
    fs.open ("floor_3.data", ios::in );
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

    cout<<lu.first<<" "<<lu.second<<endl;
    cout<<ld.first<<" "<<ld.second<<endl;
    cout<<ru.first<<" "<<ru.second<<endl;
    cout<<rd.first<<" "<<rd.second<<endl;
    for(i=1;i<=n;i++) { for(j=1;j<=m-1;j++) cout<<setw(2)<<dir[i][j]<<" "; cout<<setw(2)<<dir[i][j]<<endl;}
    for(i=1;i<=n;i++) { for(j=1;j<=m-1;j++) cout<<setw(2)<<dist[i][j]<<" "; cout<<setw(2)<<dist[i][j]<<endl;}
    cout<<go_go(dist,dir,visited,R_x,R_y,n,m,max_dist)<<endl;

    //while(!stk[0].empty()){ cout<<stk[0].top()<<" "; stk[0].pop(); cout<<stk[0].top()<<endl; stk[0].pop(); }
    delete [] tmp; delete [] dist; delete [] tmpc; delete [] dir; delete [] tmpb; delete [] visited;
    return 0;
}
