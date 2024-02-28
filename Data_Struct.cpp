#include<vector>
#include<queue>
#include<stack>
#include<utility>
#include<functional>
#include<random>

/// @brief セグ木
/// @tparam T モノイド
template<typename T> class SegmentTree{
  private:
    typedef std::function<T(T,T)> F;
    int n;
    T d0;
    std::vector<T> vertex;
    F f;
    F g;
  public:
    /// @brief セグ木作成
    /// @param f queryの適用関数
    /// @param g updateの適用関数
    /// @param d 零元
    SegmentTree(F f,F g,T d):d0(d),f(f),g(g){}
    void init(int _n){
        n=1;
        while(n<_n) n*=2;
        vertex.resize(2*n-1,d0);
    }
    void build(const std::vector<T> &v){
        int n_=v.size();
        init(n_);
        for(int i=0;i<n_;i++) vertex[n+i-1]=v[i];
        for(int i=n-2;i>=0;i--)
        vertex[i]=f(vertex[2*i+1],vertex[2*i+2]);
    }
    void update(int i,T x){
        int k=i+n-1;
        vertex[k]=g(vertex[k],x);
        while(k>0){
            k=(k-1)/2;
            vertex[k]=f(vertex[2*k+1],vertex[2*k+2]);
        }
        return;
    }
    /// @brief [l,r]のqueryを求める
    /// @param l 
    /// @param r 
    /// @return 
    T query(int l,int r){
        T vl=d0,vr=d0;
        l+=n-1;
        r+=n-1;
        for(;l<=r;l/=2,r=r/2-1){
            if(~l&1) vl=f(vl,vertex[l]);
            if(r&1) vr=f(vertex[r],vr);
        }
        return f(vl,vr);
    }
};


template<typename T>
class dynamic_segtree{
  private:
    using F = std::function<T(T,T)>;
    F f;
    F g;
    T ei;
    struct node_t{
        int L,R;
        T val;
        node_t *ch[2];
        node_t(int L,int R,T val):L(L),R(R),val(val){
            ch[0]=NULL;
            ch[1]=NULL;
        }
    };
    node_t *root;
    node_t *update(node_t* nd,int k,T x){
        if(nd->L==nd->R){
            nd->val = g(nd->val,x);
            return nd;
        }
        if(k<=(nd->R+nd->L)/2){
            if(!nd->ch[0]) nd->ch[0] = new node_t(nd->L,(nd->L+nd->R)/2,ei);
            nd->ch[0] = update(nd->ch[0],k,x);
        }
        else{
            if(!nd->ch[1]) nd->ch[1] = new node_t((nd->L+nd->R)/2+1,nd->R,ei);
            nd->ch[1] = update(nd->ch[1],k,x);
        }
        nd->val = f(nd->ch[0]?nd->ch[0]->val:ei,nd->ch[1]?nd->ch[1]->val:ei);
        return nd;
    }
    T query(node_t *nd,int l,int r){
        if(nd->R<l||r<nd->L) return ei;
        if(l<=nd->L&&nd->R<=r) return nd->val;
        return f(nd->ch[0]?query(nd->ch[0],l,r):ei,nd->ch[1]?query(nd->ch[1],l,r):ei);
    }
  public:
    dynamic_segtree(F f,T ei):f(f),ei(ei){}
    ~dynamic_segtree(){delete root;root = nullptr;}
    void init(int n){
        root = new node_t(0,n-1,ei);
    }
    void update(int k,T x,F fc){
        g=fc;
        update(root,k,x);
    }
    T query(int l,int r){
        return query(root,l,r);
    }
};

template<typename T,typename E>
class LazySegmentTree{
    private:
        using F = std::function<T(T,T)>;
        using G = std::function<T(T,E)>;
        using H = std::function<E(E,E)>;
        using HH = std::function<E(E)>;
        int n;
        std::vector<T> dat;
        std::vector<E> laz;
        T t0;
        E e0;
        F f;
        G g;
        H h;
        HH hh;
        void eval(int k){
            if(laz[k]==e0) return;
            if(k<n-1){
                laz[2*k+1]=h(laz[2*k+1],hh(laz[k]));
                laz[2*k+2]=h(laz[2*k+2],hh(laz[k]));
            }
            dat[k]=g(dat[k],laz[k]);
            laz[k]=e0;
        }
        void merge_dat(int k){
            eval(2*k+1);
            eval(2*k+2);
            dat[k]=f(dat[2*k+1],dat[2*k+2]);
        }
    public:
        LazySegmentTree(F f,G g,H h,T t0,E e0,HH hh=[](E x){return x;}):f(f),g(g),h(h),t0(t0),e0(e0),hh(hh){}
        void init(int n_){
            n=1;
            while(n<n_){
                n*=2;
            }
            dat.resize(2*n-1,t0);
            laz.resize(2*n-1,e0);
        }
        void build(const std::vector<T> &v){
            int n_=v.size();
            init(n_);
            for(int i=0;i<n_;i++) dat[n+i-1]=v[i];
            for(int i=n-2;i>=0;i--)
            dat[i]=f(dat[2*i+1],dat[2*i+2]);
        }
        void _update(int a,int b,E x,int k,int l,int r){
            if(b<l||r<a||k>=2*n-1) return;
            eval(k);
            if(a<=l&&r<=b){
                laz[k]=h(laz[k],x);
                eval(k);
            }
            else{
                _update(a,b,x,2*k+1,l,(l+r)/2);
                _update(a,b,x,2*k+2,(l+r)/2+1,r);
                dat[k]=f(dat[2*k+1],dat[2*k+2]);
            }
        }
        void update(int l,int r,E x){
            l+=n-1,r+=n-1;
            for(;l<=r;l>>=1,r=r/2-1){
                if(~l&1){
                    laz[l]=h(laz[l],x);
                    int k=l;
                    while(k>0&&~k&1){
                        eval(k);
                        k=k/2-1;
                        merge_dat(k);
                    }
                    eval(k);
                }
                if(r&1){
                    laz[r]=h(laz[r],x);
                    int k=r;
                    while(k&1){
                        eval(k);
                        k>>=1;
                        merge_dat(k);
                    }
                    eval(k);
                }
                x=h(x,x);
            }
            while(l>0){
                l=(l-1)/2;
                merge_dat(l);
            }
        }
        T _query(int a,int b,int k,int l,int r){
            if(b<l||r<a||k>=2*n-1) return t0;
            eval(k);
            if(a<=l&&r<=b) return dat[k];
            return f(_query(a,b,2*k+1,l,(l+r)/2),_query(a,b,2*k+2,(l+r)/2+1,r));
        }
        T query(int l,int r){
            return _query(l,r,0,0,n-1);
        }
        void set_val(int i,T x){
        int k=i+n-1;
        dat[k]=x;
        laz[k]=e0;
        while(k>0){
            k=(k-1)/2;
            dat[k]=f(dat[2*k+1],dat[2*k+2]);
        }
        return;
    }
};

class UnionFind
{ //UnionFind木
  private:
    std::vector<int> Parent, es;
    std::vector<long long> diff_weight;

  public:
    UnionFind(int N)
    {
        es.resize(N, 0);
        Parent.resize(N, -1);
        diff_weight.resize(N, 0LL);
    }

    int root(int A)
    {
        if (Parent[A] < 0)
            return A;
        else
        {
            int r = root(Parent[A]);
            diff_weight[A] += diff_weight[Parent[A]]; // 累積和をとる
            return Parent[A] = r;
        }
    }
    bool issame(int A, int B)
    {
        return root(A) == root(B);
    }
    long long weight(int x)
    {
        root(x); // 経路圧縮
        return diff_weight[x];
    }
    long long diff(int x, int y)
    {
        return weight(y) - weight(x);
    }
    int size(int A)
    {
        return -Parent[root(A)];
    }
    int eize(int A)
    {
        return es[root(A)];
    }
    bool connect(int A, int B)
    {
        A = root(A);
        B = root(B);
        if (A == B)
            return false;
        if (size(A) < size(B))
            std::swap(A, B);
        Parent[A] += Parent[B];
        es[A] += es[B] + 1;
        Parent[B] = A;
        return true;
    }
    void unite(int A, int B)
    {
        A = root(A);
        B = root(B);
        if (A == B)
        {
            es[A]++;
            return;
        }
        if (size(A) < size(B))
            std::swap(A, B);
        Parent[A] += Parent[B];
        es[A] += es[B] + 1;
        Parent[B] = A;
        return;
    }
    bool merge(int A, int B, long long w)
    {
        // x と y それぞれについて、 root との重み差分を補正
        w += weight(A);
        w -= weight(B);
        A = root(A);
        B = root(B);
        if (A == B)
            return false;
        if (size(A) < size(B))
            std::swap(A, B), w = -w;
        Parent[A] += Parent[B];
        Parent[B] = A;
        // x が y の親になるので、x と y の差分を diff_weight[y] に記録
        diff_weight[B] = w;
        return true;
    }
};

class HLD{
  private:
    using Graph=std::vector<std::vector<int>>;
    int root,N;
    std::vector<int> size,par,Restore,newid,front,next,Depth,leaf;
    Graph graph;
    int dfs(int u){
        int cnt=1,MAX=0;
        for(auto v:graph[u]){
            if(v==par[u]) continue;
            par[v]=u;
            cnt+=dfs(v);
            if(size[v]>MAX){
                MAX=size[v];
                next[u]=v;
            }
        }
        return size[u]=cnt;
    }
    void hld(int r){
        std::stack<int> que;
        int id=0;
        que.push(r);
        while(!que.empty()){
            int u=que.top();que.pop();
            newid[u]=id;
            Restore[id++]=u;
            front[u]=u;
            while(next[u]!=-1){
                for(auto v:graph[u]){
                    if(v==par[u]) continue;
                    if(v!=next[u]){
                        Depth[v]=Depth[u]+1;
                        que.push(v);
                    }
                    else{
                        newid[v]=id;
                        Restore[id++]=v;
                        front[v]=front[u];
                        Depth[v]=Depth[u];
                    }
                }
                u=next[u];
            }
        }
    }
  public:
    HLD(Graph &g,int root=0):graph(g),root(root),N(g.size()),
    size(N),par(N,-1),Restore(N),newid(N),front(N),next(N,-1),Depth(N),leaf(N,-1){
        dfs(root);
        hld(root);
    }
    int lca(int u,int v)const{
        while(front[u]!=front[v]){
            if(Depth[u]>Depth[v]) std::swap(u,v);
            v = par[front[v]];
        }
        return newid[u]<newid[v]?u:v;
    }
    /// @brief u,v間のパスにおける連続したノードid頂点組クエリ
    /// 計算量 : O(logV)
    /// @param u パス端点にある頂点u
    /// @param v パスの端点にある頂点v
    /// @return パス(u,v)上にある頂点が全て含まれる、区間[l,r]の集合
    std::vector<std::pair<int,int>> vquery(int u,int v)const{
        std::vector<std::pair<int,int>> res;
        int rt=lca(u,v);
        while(Depth[rt]<Depth[u]){
            res.emplace_back(newid[front[u]],newid[u]);
            u = par[front[u]];
        }
        while(Depth[rt]<Depth[v]){
            res.emplace_back(newid[front[v]],newid[v]);
            v = par[front[v]];
        }
        res.emplace_back(newid[rt],std::max(newid[u],newid[v]));
        return res;
    }
    /// @brief u,v間のパスにおける連続した辺id頂点組クエリ]
    /// 計算量O(logV)
    /// @param u パス端点にある頂点u
    /// @param v パスの端点にある頂点v
    /// @return パス(u,v)上にある辺のidが全て含まれる、区間[l,r]の集合
    std::vector<std::pair<int,int>> equery(int u,int v)const{
        //頂点idから親に向かう辺の番号をid-1とする
        std::vector<std::pair<int,int>> res;
        int rt=lca(u,v);
        while(Depth[rt]<Depth[u]){
            res.emplace_back(newid[front[u]]-1,newid[u]-1);
            u = par[front[u]];
        }
        while(Depth[rt]<Depth[v]){
            res.emplace_back(newid[front[v]]-1,newid[v]-1);
            v = par[front[v]];
        }
        int R = std::max(newid[u],newid[v]);
        if(newid[rt]!=R) res.emplace_back(newid[rt],R-1);
        return res;
    }
    /// @brief 頂点uの部分木情報
    /// 計算量償却O(1)
    /// @param u 頂点
    /// @return 頂点uを根とする部分木の区間[l, r]
    std::pair<int,int> tquery(int u){
        if(leaf[u]!=-1) return std::make_pair(newid[u],leaf[u]);
        leaf[u]=newid[u];
        for(auto v:graph[u]){
            if(v==par[u]) continue;
            tquery(v);
            leaf[u]=std::max(leaf[u],leaf[v]);
        }
        return std::make_pair(newid[u],leaf[u]);
    }
    /// @brief  頂点uのid、頂点uから根に向かう辺はid-1
    int id(int u)const{return newid[u];}
    // idが示す頂点の元の番号
    int restore(int ID)const{return Restore[ID];}
    /// @brief 頂点uの深さ
    int depth(int u)const{return Depth[u];}
    // 頂点uの親
    int parent(int u)const{return par[u];}
};

template<typename T>
class DAG{
  private:
    int v;
    std::vector<std::vector<std::pair<int,T>>> to;
    std::vector<int> rank;
    std::vector<T> dp;
  public:
    DAG(int v,const T &x):v(v){
        to.resize(v);
        rank.resize(v);
        dp.resize(v,x);
        std::iota(rank.begin(),rank.end(),0);
    }
    void add(int a,int b,const T &c=0){
        to[a].push_back(std::make_pair(b,c));
        if(rank[a]>rank[b]) std::swap(rank[a],rank[b]);
    }
    void set(int i,const T &x){
        dp[i]=x;
    }
    std::vector<T> calc(std::function<T(T,T)> f){
        std::vector<int> vertex(v);
        std::vector<T> tmp=dp;
        for(int i=0;i<v;++i) vertex[rank[i]]=i;
        for(int i=0;i<v;++i){
            for(auto e:to[vertex[i]]){
                tmp[e.first]=f(tmp[e.first],tmp[i]+e.second);
            }
        }
        return tmp;
    }
};

template<typename T>
class FFT{
  private:
    using Complex = std::complex<double>;
    std::vector<Complex> C;
    void DFT(std::vector<Complex> &F,size_t n,int sig=1){
        if(n==1) return;
        std::vector<Complex> f0(n/2),f1(n/2);
        for(size_t i=0;i<n/2;++i){
            f0[i]=F[2*i];
            f1[i]=F[2*i+1];
        }
        DFT(f0,n/2,sig);
        DFT(f1,n/2,sig);
        Complex z(cos(2.0*PI/n),sin(2.0*PI/n)*sig),zi=1;
        for(size_t i=0;i<n;++i){
            if(i<n/2) F[i]=f0[i]+zi*f1[i];
            else F[i]=f0[i-n/2]+zi*f1[i-n/2];
            zi*=z;
        }
        return;
    }
    void invDFT(std::vector<Complex> &f,size_t n){
        DFT(f,n,-1);
        for(size_t i=0;i<n;++i){
            f[i]/=n;
        }
        return;
    }
  public:
    FFT(const std::vector<T> &A,const std::vector<T> &B){
        size_t n=1;
        while(n<=A.size()+B.size()){
            n*=2;
        }
        std::vector<Complex> g(n,Complex(0));
        C.resize(n,Complex(0));
        size_t i_len=std::max(A.size(),B.size());
        for(size_t i=0;i<i_len;++i){
            if(i<A.size()) C[i]=Complex(A[i]);
            if(i<B.size()) g[i]=Complex(B[i]);
        }
        DFT(C,n);
        DFT(g,n);
        for(size_t i=0;i<n;++i) C[i]=C[i]*g[i];
        invDFT(C,n);
        for(size_t i=0;i<n;++i) if(T(C[i].real())!=C[i].real()){
            C[i]=Complex(C[i].real()+0.5);
        }
    }
    T operator[](int k)const{return T(C[k].real());}
};

/// @brief Treap
/// @tparam T : キーの型
template<class T>
class Treap {
    std::random_device rd;
    std::mt19937 mt;

    struct Node{
        T key;
        int priority;
        Node *l;
        Node *r;
        Node(T key, int priority)
        :key(key),priority(priority), l(nullptr),r(nullptr) {

        }
    };
    Node *root;
    void deconstruct(Node *root) {
        if(root->l) deconstruct(root->l);
        if(root->r) deconstruct(root->r);
        delete root;
    }
    /// @brief Nodeをkeyで左右に分割する
    /// @param root Treapの根
    /// @param key 
    /// @param l 左の部分木
    /// @param r 右の部分木
    void split(Node *root, T key, Node **l, Node **r) {
        if(!root) {
            *l = *r = nullptr;
        }
        else if(key < root->key) {
            split(root->l, key, l, root->l);
            *r = root;
        }
        else {
            split(root->r, key, root->r, r);
            *l = root;
        }
    }

    void insert(Node **root, Node *item) {
        if(!root) {
            *root = item;
        }
        else if(item->priority > root->priority){
            split(root, item->key, &(item->l), &(item->r));
            *root = item;
        } else {
            if(item->key < root->key) {
                insert(root->l, item);
            }
            else {
                insert(root->r, item);
            }
        }
    }

    void merge(Node **root, Node *left, Node *right) {
        if(!left) {
            *root = right;
        }
        else if(!right) {
            *root = left;
        }
        else if(left->priority, right->priority) {
            merge(left->r, left->r, right);
            *root = left;
        }
        else {
            merge(right->l, left, right->l);
            *root = right;
        }
    }

    void erase(Node **root, T key) {
        if(*root->key == key) {
            merge(root, *root->l, *root->r);
        }
        else {
            if(key < *root->key) {
                erase(*root->l, key);
            }
            else {
                erase(*root->r, key);
            }
        }
    }

    bool find(Node *root, T key) {
        if(!root) {
            return false;
        }
        else if(root->key == key) {
            return true;
        }
        else {
            if(key < root->key){
                return find(root->l, key);
            }
            else {
                return find(root->r, key);
            }
        }
    }

public:
    Treap() :mt(rd()), root(nullptr){};
    ~Treap() { deconstruct(root); }
    /// @brief keyを持つノードを挿入する
    /// @param key 
    void insert(T key) {
        insert(&root, new Node(key, mt()));
    }

    /// @brief keyを持つノードを1つ削除する
    /// @param key 
    void erase(T key) {
        erase(&root, key);
    }

    /// @brief keyが存在するか判定する
    /// @param key 
    /// @return 
    bool find(T key) {
        return find(root, key);
    }
};

/// @brief 操作をO(logN)でできる配列(insert, erase, reverse, rotate, range min, range add)
/// @tparam T : 配列要素の型
template<class T>
class ImplicitTreap {
    std::random_device rd;
    std::mt19937 mt;
    T INF;
    struct Node {
        T value, min, lazy;
        int priority, cnt;
        bool rev;
        Node *l, *r;
        Node(int value, int priority, T INF)
        : value(value), min(INF), lazy(0), priority(priority), cnt(1), rev(false), l(nullptr), r(nullptr) 
        {

        }
    } *root = nullptr;
    using Tree = Node *;

    int cnt(Tree t) {
        return t ? t->cnt : 0;
    }

    int get_min(Tree t) {
        return t ? t->min : INF;
    }

    void update_cnt(Tree t) {
        if (t) {
            t->cnt = 1 + cnt(t->l) + cnt(t->r);
        }
    }

    void update_min(Tree t) {
        if (t) {
            t->min = min(t->value, min(get_min(t->l), get_min(t->r)));
        }
    }

    void pushup(Tree t) {
        update_cnt(t), update_min(t);
    }

    void pushdown(Tree t) {
        if (t && t->rev) {
            t->rev = false;
            swap(t->l, t->r);
            if (t->l) t->l->rev ^= 1;
            if (t->r) t->r->rev ^= 1;
        }
        if (t && t->lazy) {
            if (t->l) {
                t->l->lazy += t->lazy;
                t->l->min += t->lazy;
            }
            if (t->r) {
                t->r->lazy += t->lazy;
                t->r->min += t->lazy;
            }
            t->value += t->lazy;
            t->lazy = 0;
        }
        pushup(t);
    }
    
    void split(Tree t, int key, Tree& l, Tree& r) {
        if (!t) {
            l = r = nullptr;
            return;
        }
        pushdown(t);
        int implicit_key = cnt(t->l) + 1;
        if (key < implicit_key) {
            split(t->l, key, l, t->l), r = t;
        } else {
            split(t->r, key - implicit_key, t->r, r), l = t;
        }
        pushup(t);
    }
    
    void insert(Tree& t, int key, Tree item) {
        Tree t1, t2;
        split(t, key, t1, t2);
        merge(t1, t1, item);
        merge(t, t1, t2);
    }

    void merge(Tree& t, Tree l, Tree r) {
        pushdown(l);
        pushdown(r);
        if (!l || !r) {
            t = l ? l : r;
        } else if (l->priority > r->priority) {
            merge(l->r, l->r, r), t = l;
        } else {
            merge(r->l, l, r->l), t = r;
        }
        pushup(t);
    }
    
    void erase(Tree& t, int key) {
        Tree t1, t2, t3;
        split(t, key + 1, t1, t2);
        split(t1, key, t1, t3);
        merge(t, t1, t2);
    }

    void add(Tree t, int l, int r, T x) {
        Tree t1, t2, t3;
        split(t, l, t1, t2);
        split(t2, r - l, t2 , t3);
        t2->lazy += x;
        t2->min += x;
        merge(t2, t2, t3);
        merge(t, t1, t2);
    }

    int findmin(Tree t, int l, int r) {
        Tree t1, t2, t3;
        split(t, l, t1, t2);
        split(t2, r - l, t2, t3);
        T ret = t2->min;
        merge(t2, t2, t3);
        merge(t, t1, t2);
        return ret;
    }

    void reverse(Tree t, int l, int r) {
        if (l > r) return;
        Tree t1, t2, t3;
        split(t, l, t1, t2);
        split(t2, r - l, t2, t3);
        t2->rev ^= 1;
        merge(t2, t2, t3);
        merge(t, t1, t2);
    }

    // [l, r)の先頭がmになるように左シフトさせる。std::rotateと同じ仕様
    void rotate(Tree t, int l, int m, int r) {
        reverse(t, l, r);
        reverse(t, l, l + r - m);
        reverse(t, l + r - m, r);
    }

    void dump(Tree t) {
        if (!t) return;
        pushdown(t);
        dump(t->l);
        cout << t->value << " ";
        dump(t->r);
    }
    
public:
    ImplicitTreap(): mt(rd()), INF() {}
    explicit ImplicitTreap(T t_max)
    :ImplicitTreap(){ INF = t_max;}

    ImplicitTreap(std::vector<T> array, T t_max)
    :ImplicitTreap(t_max) {
        for(size_t idx = 0; idx < array.size(); idx++) {
            insert(idx, array[idx]);
        }
    }

    /// @brief posの前にxを追加する
    /// @param pos 
    /// @param x 
    void insert(int pos, T x) {
        insert(root, pos, new Node(x, mt(), INF));
    }

    /// @brief [l, r)の範囲にxを加算する
    /// @param l 
    /// @param r 
    /// @param x 
    void add(int l, int r, T x) {
        add(root, l, r, x);
    }

    /// @brief [l, r)の範囲の最小値を探索する
    /// @param l 
    /// @param r 
    /// @return [l, r)内の最小値
    int findmin(int l, int r) {
        return findmin(root, l, r);
    }

    /// @brief posの位置にある要素を削除する
    /// @param pos 
    void erase(int pos) {
        erase(root, pos);
    }

    /// @brief [l, r)の範囲を反転する
    /// @param l 
    /// @param r 
    void reverse(int l, int r) {
        reverse(root, l, r);
    }

    /// @brief [l, r)の範囲を、mが先頭に来るように回転する
    /// @param l 
    /// @param m 
    /// @param r 
    void rotate(int l, int m, int r) {
        rotate(root, l, m, r);
    }

    T at(int pos) {

    }

    void dump() {
        dump(root);
        cout << endl;
    }

    T operator[](int pos) {
        return findmin(pos, pos+1);
    }
};


/// @brief 直線集合に対して、あるxの最小値をO((N+Q)(logN)^2)で求める
/// @tparam T 直線と戻り値の型
template<typename T>
class ConvexHullTrick {
private:
    /// @brief f(x) = ax + b 
    struct Linear {
        T a;
        T b;
        bool is_query;
        mutable std::function<const Linear *(void)> getSuc;
        bool operator<(const Linear &rhs)const{
            // lower_boundで二分探索するロジック
            if (is_query) {
				const Linear* suc = getSuc();
				if (suc == nullptr) return true;
				return (suc->a - rhs.a) * a + suc->b - rhs.b > 0;
			}
			if (rhs.is_query) {
				const Linear* suc = getSuc();
				if (suc == nullptr) return false;
				return (suc->a - a) * rhs.a + suc->b - b < 0;
			}
            // ソートのための順序
            return this->a < rhs.a;
        }
        /// @brief あるxにおける直線のy座標
        /// @param x x座標
        /// @return y座標
        T f(const T &x)const{
            return this->a * x + b;
        }
        Linear(const T &a, const T &b, const bool &is_query = false):
        a(a), 
        b(b),
        is_query(is_query)
        {}
    };
    std::set<Linear> lines;

    /// @brief イテレータが示す線が不要か判定する
    /// @param iter 対象iterator
    /// @return 不要かどうか
    bool is_bad(const typename std::set<Linear>::iterator iter) {
        // 前後に直線が二つなければ必要な線
        auto nex = std::next(iter);
        if(iter == lines.begin() || nex == lines.end()) return false;
        auto pre = std::prev(iter);
        // 前後の直線の交点の下を通るか判定する
		return (iter->b - pre->b) * (nex->a - iter->a) >= (nex->b - iter->b) * (iter->a - pre->a);
    }
public:
    ConvexHullTrick() {}
    /// @brief y=ax+bである直線を追加する
    /// @param a 傾き
    /// @param b y切片
    void add_line(const T &a, const T &b){
        auto result = lines.insert(Linear(a, b));
        if(!result.second) {
            // 同じ直線がある場合追加しない
            return;
        }
        auto iter = result.first;
        iter->getSuc = [=] {return next(iter) == lines.end() ? nullptr : &*next(iter); };
        if(is_bad(iter)) {
            // 追加不要なら削除
            lines.erase(iter);
            return;
        }
        auto nex = std::next(iter);
        while(nex != lines.end() && is_bad(nex)){
            // 傾きが大きいものが不要なら削除する
            iter = std::prev(lines.erase(nex));
            nex = std::next(iter);
        }
        while(iter != lines.begin() && is_bad(std::prev(iter))){
            // 傾きが小さいものが不要なら削除する
            iter = lines.erase(std::prev(iter));
            
        }
    }

    /// @brief あるxにおける直線群の最小値を求める
    /// @param x x座標
    /// @return y座標の最小値
    T min(const T &x) {
        // (単調減少区間) | (単調増加区間)
        // この区切りを二分探索で調べると、そこが最小値になる
        auto res = lines.lower_bound(Linear(x, 0, true));

        return res->f(x);
    }

    void print(T x){
        cerr << "cht data print start" << endl;
        cerr << x << endl;
        for(auto &line: lines){
            cerr << line.a << " " << line.b << " " << line.f(x) << endl;
        }
        cerr << "cht data print end" << endl;
    }
};

/// @brief 平方分割リスト構造
/// @tparam T リスト構造に乗せる型
/// 平方分割された各バケットは先頭ノードが次のバケットの先頭ノードへのポインタを持つ
/// バケットのサイズはsqrt(N)で、小数点以下切り捨てとする
/// 区間クエリ[l,r)もできる、交換則と逆元を持つこと
template<class T>
class SquareDivisionList{
private:
    using QueryFunc = std::function<T(T,T)>;
    using RevFunc = std::function<T(T,T)>; // 逆元

    static T func_init(T a, T b){return a;}

    template<class U>
    struct ListElement{
        ListElement *nxt;
        ListElement *prv;
        ListElement *sq_jump;
        T value;
        ListElement(const T& value):
            nxt(nullptr),
            prv(nullptr),
            sq_jump(nullptr),
            value(value) {

        }
    };

    size_t length;
    size_t sq_length; // sq_lengthはinsert時のみ更新
    ListElement<T> *front;
    std::vector<T> query_backets; // バケット単位のquery_func適用結果

    QueryFunc query_func;
    RevFunc rev_func;
    T id_e;

    struct SearchResult{
        ListElement<T> *backet_front;
        ListElement<T> *searched_element;
        size_t backet_index;
    };

    /// @brief ノード検索
    /// @param idx 
    /// @return 先頭から見てidx番目のノード
    SearchResult search(size_t idx){
        // std::cerr << "search:" << idx << std::endl;
        ListElement<T> *backet_front = front;
        size_t b_idx = 0;
        while(idx >= sq_length && backet_front->sq_jump){
            backet_front = backet_front->sq_jump;
            idx -= sq_length;
            b_idx++;
        }
        ListElement<T> *elem = backet_front;
        while(idx-- > 0){
            elem = elem->nxt;
        }
        // std::cerr << "search result:element value=" << elem->value << ":backet index=" << b_idx << std::endl; 
        return {backet_front, elem, b_idx};
    }

    /// @brief 平方分割用リンク作成
    /// @param  先頭からsqrt(N)個毎にリンクを貼って、バケットを初期化する
    void make_square_division_link(void){
        ListElement<T> *backet_front = front;
        ListElement<T> *element = front;
        T b = id_e;
        query_backets.clear();
        // std::cerr << "pointA" << std::endl;
        for(int i = 1; i <= length; i++){
            // std::cerr << "pointB" << std::endl;
            b = query_func(b, element->value);
            // std::cerr << "pointC" << std::endl;
            element = element->nxt;
            if(i % sq_length == 0){
                // sqrt(N)回遷移したところにリンクを貼る
                backet_front->sq_jump = element;
                backet_front = element;
                // 対応するバケットに演算結果を入れる
                query_backets.push_back(b);
                b = id_e;
            }
        }
        if(length % sq_length != 0){
            // 後ろが余ったら結果をバケットに入れておく
            query_backets.push_back(b);
        }
    }

    size_t decide_backet_size(size_t full_length){
        return std::sqrt(full_length) + 1;
    }
public:
    explicit SquareDivisionList(
        T id_e = T(), 
        QueryFunc qf = func_init, 
        RevFunc rev_func = func_init
    ):
        length(0),
        sq_length(0),
        front(nullptr),
        query_backets(std::vector<T>()),
        query_func(qf),
        rev_func(rev_func),
        id_e(id_e)
    {

    }
    explicit SquareDivisionList(
        const std::vector<T> &vec, 
        T id_e = T(), 
        QueryFunc qf=func_init, 
        RevFunc rev_func = func_init
    ):
        length(vec.size()),
        front(nullptr),
        query_backets(std::vector<T>()),
        query_func(qf),
        rev_func(rev_func),
        id_e(id_e)
    {
        sq_length = decide_backet_size(length);
        front = new ListElement<T>(vec[0]);
        ListElement<T> *prv = front;
        for(int i = 1; i < vec.size(); i++){
            ListElement<T> *element = new ListElement<T>(vec[i]);
            //std::cerr << element->value << ", ";
            // [i-1, i]を双方向に連結
            prv->nxt = element;
            element->prv = prv;
            prv = element;
        }
        //std::cerr << endl;
        // [0, n-1]を双方向に連結
        front->prv = prv;
        prv->nxt = front;
        //std::cerr << front->nxt->value << std::endl;
        make_square_division_link();
    }

    ~SquareDivisionList(){
        ListElement<T> *e;
        for(size_t i = 0; i < length; i++){
            e = front->nxt;
            delete front;
            front = e;
        }
    }

    void insert(size_t idx, const T& value){
        // 指定位置に挿入
        // std::cerr << "insert:" << idx << " " << value << std::endl;
        ListElement<T> *element = new ListElement<T>(value);
        if(!front){
            front = element;
            front->nxt = front;
            front->prv = front;
        }
        else{
            SearchResult result = search(idx);
            ListElement<T> *nxt = result.searched_element;
            ListElement<T> *prv = nxt->prv;
            element->nxt = nxt; // p e -> n
            prv->nxt = element; // p -> e -> n
            nxt->prv = element; // p -> e <-> n
            element->prv = prv; // p<-> e <-> n
            // 先頭に挿入したときは特殊
            if(idx == 0){
                front = element;
            }
        }
        // 平方分割リンクを更新して、バケットの値を再計算する
        size_t sq_length_new = decide_backet_size(++length);
        // std::cerr << sq_length << " " << sq_length_new << std::endl;
        if(sq_length != sq_length_new){
            // 分割サイズが変わったら再構築
            sq_length = sq_length_new;
            make_square_division_link();
        }
        else{
            // 先頭からバケットごとに走査していく
            // バケットが挿入されたところから後ろは1つずれていく
            size_t now_idx = 0;
            size_t backet_index = 0;
            ListElement<T> *backet_front = front;
            while(now_idx + sq_length < length - 1){
                if(now_idx + sq_length == idx){
                    // 次のバケットの先頭に要素が挿入された場合
                    backet_front->sq_jump = backet_front->sq_jump->prv;
                }
                else if(now_idx < idx && idx < now_idx + sq_length){
                    // バケット内の先頭以外に挿入された要素がある場合
                    // 正しいバケットの先頭にリンクする
                    backet_front->sq_jump = backet_front->sq_jump->prv;
                    // バケットの末尾が出ていく
                    query_backets[backet_index] = rev_func(query_backets[backet_index], backet_front->sq_jump->value);
                    query_backets[backet_index] = query_func(query_backets[backet_index], value);
                }
                else if(idx <= now_idx){
                    // バケット先頭以前に要素が挿入済みの場合
                    // 挿入前のバケット先頭は後ろにずれている
                    ListElement<T> *prev_bf = backet_front->nxt;
                    backet_front->sq_jump = prev_bf->sq_jump->prv;
                    // バケットの末尾が出ていく
                    query_backets[backet_index] = rev_func(query_backets[backet_index], backet_front->sq_jump->value);
                    // 今のバケット先頭が新たに入ってきた要素である
                    query_backets[backet_index] = query_func(query_backets[backet_index], backet_front->value);
                }
                now_idx += sq_length;
                backet_index++;
                backet_front = backet_front->sq_jump;
            }
            // 最も後ろのバケットは次のバケットへのリンクがない
            if(length % sq_length == 1){
                // 新しいバケットを追加
                ListElement<T> *back = front->prv;
                backet_front->sq_jump = back;
                query_backets.push_back(back->value);
                // 出ていく要素の分を計算する(末尾挿入のときは後で余分に加算する)
                query_backets[backet_index] = rev_func(query_backets[backet_index], back->value);
            }
            if(now_idx <= idx && idx <= now_idx + sq_length){
                // バケット先頭以降に要素が挿入されているとき
                // バケット内には挿入された要素が入る(末尾挿入でバケット追加されたときは出ていく要素としてキャンセルされる)
                query_backets[backet_index] = query_func(query_backets[backet_index], value);
            }
            else{
                // バケット先頭より前に要素が挿入されているとき
                // 先頭自身がバケットに入ってきた要素である
                query_backets[backet_index] = query_func(query_backets[backet_index], backet_front->value);
            }
        }
    }

    /// @brief idxの要素をeraseする
    /// @param idx 
    void erase(size_t idx){
        if(idx >= length) return;
        // リストの張り直し、バケットの更新
        // 要素が削除されるところから前にずれる
        SearchResult result = search(idx);
        size_t now_idx = 0;
        size_t backet_index = 0;
        ListElement<T> *back = front->prv;
        if(idx == 0){
            front = front->nxt;
        }
        ListElement<T> *backet_front = front;
        while(now_idx + sq_length < length - 1){
            if(now_idx + sq_length == idx) {
                // 次のバケットの先頭が削除されるとき
                // 正しいバケットの先頭にリンクする
                backet_front->sq_jump = backet_front->sq_jump->nxt;
            }
            else if(now_idx < idx && idx < now_idx + sq_length){
                // バケット内の先頭以外に削除される要素がある場合
                // 次のバケットの先頭が入ってくる
                query_backets[backet_index] = query_func(query_backets[backet_index], backet_front->sq_jump->value);
                query_backets[backet_index] = rev_func(query_backets[backet_index], result.searched_element->value);
                // 正しいバケットの先頭にリンクする
                backet_front->sq_jump = backet_front->sq_jump->nxt;
            }
            else if(idx <= now_idx){
                // バケット先頭以前に要素が削除済みの場合
                // 1つ前の要素が先頭だったので拾ってくる
                ListElement<T> *prv_elem = backet_front->prv;
                backet_front->sq_jump = prv_elem->sq_jump->nxt;
                // 次のバケットの先頭が入ってくる
                query_backets[backet_index] = query_func(query_backets[backet_index], prv_elem->sq_jump->value);
                // 今のバケットの前の要素が出ていく
                query_backets[backet_index] = rev_func(query_backets[backet_index], prv_elem->value);
            }
            now_idx += sq_length;
            backet_index++;
            backet_front = backet_front->sq_jump;
        }
        // 末尾バケット
        if(length % sq_length == 1){
            // ラス1で末尾バケットは削除する
            query_backets.pop_back();
            backet_front->sq_jump = nullptr;

            //末尾の要素を末尾バケットに加算
            query_backets[backet_index] = query_func(query_backets[backet_index], back->value);
        }
        if(now_idx <= idx && idx <= now_idx + sq_length){
            // バケット先頭以降の要素が削除されているとき
            // バケット内から削除される要素が出ていく(末尾削除でバケット削除されたときはキャンセルされる)
            query_backets[backet_index] = rev_func(query_backets[backet_index], result.searched_element->value);
        }
        else{
            // バケット先頭より前に要素が挿入されているとき
            // 先頭の前の要素がバケットから出ていく要素である
            query_backets[backet_index] = rev_func(query_backets[backet_index], backet_front->prv->value);
        }
        // 指定位置の要素を削除
        ListElement<T> *prv = result.searched_element->prv;
        ListElement<T> *nxt = result.searched_element->nxt;
        prv->nxt = nxt;
        nxt->prv = prv;
        delete result.searched_element;
        length--;
        if(length == 0){
            front = nullptr;
        }
    }

    /// @brief [l, r)の区間クエリを取得する
    /// @param l 
    /// @param r 
    /// @return 
    T query(size_t l, size_t r){
        // std::cerr << l << " " << r << std::endl;
        SearchResult ret_l = search(l);
        T q = id_e;
        ListElement<T> *element = ret_l.searched_element;
        //printf("pointA\n");
        if(r - l <= sq_length /2){
        	// full calc
        	while(l++ < r){
        		q = query_func(q, element->value);
        		element = element->nxt;
        	}
        	return q;
        }
        else if(l % sq_length <= sq_length / 2){
        	//printf("pointD\n");
        	// [bf, bf_nxt) - [bf, l)
        	q = query_func(q, query_backets[ret_l.backet_index]);
        	while(element != ret_l.backet_front){
        		element = element->prv;
        		l--;
        		q = rev_func(q, element->value);
        	}
        	//printf("pointF\n");
        	if(element->sq_jump){
        		element = element->sq_jump;
        		l += sq_length;
        	}
        	else{
        		element = front;
        		l = length;
        	}
        	// - [r, bf_next)
        	//std::cerr << r << " " << l << " " << length<<std::endl;
        	while(r < l){
        		element =element->prv;
        		l--;
        		q = rev_func(q, element->value);
        	}
        	//printf("pointG\n");
        	if(r==l){
        		return q;
        	}
        }
        else if(r > (ret_l.backet_index+1)*sq_length){
        	//printf("pointE\n");
        	// [l, bf_nxt)
        	while(element != ret_l.backet_front->sq_jump){
        		q = query_func(q, element->value);
        		element = element->nxt;
        		l++;
        	}
        }
        else{
        	// [l, r)
        	while(l++ < r){
        		q = query_func(q, element->value);
        		element = element->nxt;
        	}
        	return q;
        }
        //printf("pountB\n");
        // [bf_nxt, bf_last)
        while(r - l >= sq_length){
        	q = query_func(q, query_backets[++ret_l.backet_index]);
        	element = element->sq_jump;
        	l += sq_length;
        }
        //printf("poibtC\n");
        if(r - l < sq_length / 2){
        	//[bf_last, r)
        	while(l++ < r){
        		q = query_func(q, element->value);
        		element = element->nxt;
        	}
        }
        else{
        	// [bf_last, bf_last_nxt) - [r, bf_last_nxt)
        	q = query_func(q, query_backets[++ret_l.backet_index]);
        	if(element->sq_jump){
        		element = element->sq_jump;
        		l += sq_length;
        	}
        	else{
        		element = front;
        		l = length;
        	}
        	while(r < l--){
        		element = element->prv;
        		q = rev_func(q, element->value);
        	}
        }
        return q;
    }

    void change(size_t idx, const T& value){
        SearchResult result = search(idx);
        // 逆元を適用して再計算
        T& backet = query_backets[result.backet_index];
        backet = rev_func(backet, result.searched_element->value);
        backet = query_func(backet, value);
        result.searched_element->value = value;
    }

    const T& at(size_t idx){
        if(idx >= length){
            throw std::out_of_range("index is out of range");
        }
        return search(idx).searched_element->value;
    }

    void debug_print(void){
        std::cerr << "list elements:\r\n[";
        ListElement<T> *e = front;
        for(size_t i = 0; i < length; i++){
            std::cerr << e->value << ", ";
            e = e->nxt;
        }
        std::cerr << "]\r\nreverse link:\r\n[";
        for(size_t i = 0; i < length; i++){
            e = e->prv;
            std::cerr << e->value << ", ";
        }
        std::cerr << "]\r\nsquare_link:\r\n[";
        while(e){
            std::cerr << e->value << ", ";
            e = e->sq_jump;
        }
        std::cerr << "]\r\nquery_backets:\r\n[";
        for(auto q: query_backets){
            std::cerr << q << ", ";
        }
        std::cerr << "]" << std::endl;
    }
};

template<class T>
class BlockLinkedList{
private:
	using QueryFunc = std::function<T(T,T)>;
	template<class U>
	struct BlockElement{
		BlockElement *nxt;
		U value;
		BlockElement(const T& value):
			nxt(nullptr),
			value(value)
		{
			
		}
	};
	
	template<class U>
	struct BlockNode{
		BlockElement<U> *front;
		BlockNode *nxt_node;
		size_t size;
		U acc_value;
		BlockNode(void):
			front(nullptr),
			nxt_node(nullptr),
			size(0)
		{
			
		}
	};
	
	size_t length;
	BlockNode<T> *front_node;
	
	QueryFunc query_func;
	
	BlockNode<T> *connect(BlockNode<T> *node){
		if(!node->nxt_node || node->size + node->nxt_node->size > std::sqrt(length)){
			return node;
		}
		BlockNode<T> *nxt_node = node->nxt_node;
		node->nxt_node = nxt_node->nxt_node;
		node->size += nxt_node->size;
		node->acc_value = query_func(node->acc_value, nxt_node->acc_value);
		BlockElement<T> *element = node->front;
		while(element->nxt){
			element = element->nxt;
		}
		element->nxt = nxt_node->front;
		delete nxt_node;
		return node;
	}
	
	void calc_node_acc(BlockNode<T> *node){
		BlockElement<T> *element = node->front;
		node->acc_value = element->value;
		while(element->nxt){
			element = element->nxt;
			node->acc_value = query_func(node->acc_value, element->value);
		}
	}
	
	BlockNode<T> *cut(BlockNode<T> *node){
		// std::cerr << "cut start" << std::endl;
		size_t sq_length = std::sqrt(length);
		if(node->size < 2*sq_length){
			return node;
		}
		BlockElement<T> *element = node->front;
		node-> acc_value = element->value;
		for(int i = 1; i < sq_length; i++){
			element = element->nxt;
			node->acc_value = query_func(node->acc_value, element->value);
		}
		BlockNode<T> *nxt_node = new BlockNode<T>();
		nxt_node->nxt_node = node->nxt_node;
		node->nxt_node = nxt_node;
		nxt_node->size = node->size - sq_length;
		node->size = sq_length;
		nxt_node->front = element->nxt;
		element->nxt = nullptr;
		// calc new node accumulate
		calc_node_acc(nxt_node);
		return node;
	}
	
	struct SearchResult{
		BlockNode<T> *node;
		BlockElement<T> *element;
		size_t elem_index;
	};
	SearchResult search(size_t idx){
		BlockNode<T> *node = front_node;
		if(idx >= length){
			return {nullptr, nullptr, 0};
		}
		while(node){
			node = connect(node);
			// std::cerr << "search:node size=" << node->size << std::endl;
			if(idx >= node->size){
				idx -= node->size;
				node = node->nxt_node;
			}
			else {
				break;
			}
		}

		BlockElement<T> *element = nullptr;
		if(node){
			element = node->front;
			for(size_t i = 0; i < idx; i++){
				element = element->nxt;
			}
		}
		return {node, element, idx};
	}
	
public:
	explicit BlockLinkedList(QueryFunc q=[](T a,T b){return a+b;}):
		length(0),
		front_node(new BlockNode<T>()),
		query_func(q)
	{
		
	}
	explicit BlockLinkedList(const std::vector<T> &vec, QueryFunc q = [](T a, T b){return a+b;}):
		length(vec.size()),
		front_node(new BlockNode<T>()),
		query_func(q)
	{
		if(length == 0){
			return;
		}
		size_t sq_length = std::sqrt(length);
		BlockNode<T> *node = front_node;
		BlockElement<T> *element = nullptr;
		for(auto v : vec){
			if(node->size >= sq_length){
				node->nxt_node = new BlockNode<T>();
				node = node->nxt_node;
			}
			if(!node->size){
				element = new BlockElement<T>(v);
				node->front = element;
				node->acc_value = v;
			}
			else {
				element->nxt = new BlockElement<T>(v);
				element = element->nxt;
				node->acc_value = query_func(node->acc_value, v);
			}
			node->size++;
		}
	}
	~BlockLinkedList(){
		BlockNode<T> *node = front_node;
		while(node){
			BlockNode<T> *prv_node = node;
			BlockElement<T> *element = node->front;
			while(element){
				BlockElement<T> *prv_elem = element;
				element = element->nxt;
				delete prv_elem;
			}
			node = node->nxt_node;
			delete prv_node;
		}
	}
	
	void insert(size_t idx, const T& value){
		// std::cerr << "insert start" << std::endl;
		BlockNode<T> *node;
		if(length == 0){
			// std::cerr << "first insert point" << std::endl;
			front_node->front = new BlockElement<T>(value);
			front_node->acc_value = value;
			node = front_node;
			// std::cerr << "first insert end" <<std::endl;
		}
		else if(idx == 0){
			node = front_node;
			BlockElement<T> *element = node->front;
			node->front = new BlockElement<T>(value);
			node->front->nxt = element;
			calc_node_acc(node);
		}
		else{
			SearchResult result = search(idx-1);
			node = result.node;
			if(!result.element->nxt){
				result.element->nxt = new BlockElement<T>(value);
				node->acc_value = query_func(node->acc_value, value);
			}
			else {
				BlockElement<T> *element = new BlockElement<T>(value);
				element->nxt = result.element->nxt;;
				result.element->nxt = element;
				calc_node_acc(node);
			}
		}
		node->size++;
		length++;
		cut(node);
		// std::cerr<<"insert end" << std::endl;
	}
	
	void erase(size_t idx){
		BlockNode<T> *node;
		BlockElement<T> *element;
		assert(idx < length);
		if(idx==0){
			node = front_node;
			element = node->front;
			if(node->size == 1){
				front_node = node->nxt_node;
				delete node;
			}
			else{
				node->front = element->nxt;
				node->size--;
				calc_node_acc(node);
			}
		}
		else{
			SearchResult result = search(idx - 1);
			element = result.element->nxt;
			node = result.node;
			if(!element){
				BlockNode<T> *nxt_node = node->nxt_node;
				if(nxt_node->size <= 1){
					element = nxt_node->front;
					node->nxt_node = nxt_node->nxt_node;
					delete nxt_node;
				}
				else{
					node = node->nxt_node;
					element = node->front;
					node->front = element->nxt;
					node->size--;
					calc_node_acc(node);
				}
			}
			else{
				// node size is absolutely >= 2
				result.element->nxt = element->nxt;
				node->size--;
				calc_node_acc(node);
			}
		}
		delete element;
		length--;
	}
	
	T query(size_t l, size_t r){
		SearchResult result = search(l);
		BlockNode<T> *node = result.node;
		BlockElement<T> *element = result.element;
		T res;
		// std::cerr << "query input=" << l << " " << r <<std::endl;
		// std::cerr << "first element index=" << result.elem_index<< std::endl;
		if(result.elem_index > 0 || r - l < node->size){
			res = element->value;
			l++;
			while(l < r && element->nxt){
				element = element->nxt;
				l++;
				res = query_func(res, element->value);
			}
		}
		else{
			res = node->acc_value;
			l += node->size;
		}
		node = node->nxt_node;
		// std::cerr << "first res:l:r:" << res << " " << l << " " << r << std::endl;
		while(node && r - l >= node->size){
			res = query_func(res,node->acc_value);
			l += node->size;
			node = node->nxt_node;
		}
		// std::cerr << "backet res:l:r:" << res << " " << l << " " << r << std::endl;
		if(node){
			BlockElement<T> *element = node->front;
			while(l++ < r){
				//std::cerr << element->value <<std::endl;
				res = query_func(res, element->value);
				element = element->nxt;
			}
		}
		// std::cerr << "last res:l:r:" << res << " " << l << " " << r << std::endl;
		return res;
	}
	
	void change(size_t idx, const T& value){
		SearchResult result = search(idx);
		result.element-> value = value;
		calc_node_acc(result.node);
	}
	
	const T& operator[](size_t idx){
		return search(idx).element->value;
	}
	
	void debug_print(){
		BlockNode<T> *node = front_node;
		bool ass = false;
		std::cerr << "----------node info----------" << std::endl;
		while(node){
			std::cerr << "node size = " << node->size << std::endl;
			if(node->size == 0){
				ass = true;
			}
			std::cerr << "node acc = " << node->acc_value << std::endl;
			std::cout << "node lists:\r\n[";
			BlockElement<T> *element = node->front;
			while(element){
				std::cerr << element->value << ", ";
				element = element->nxt;
			}
			std::cerr << "]" << std::endl;
			node = node->nxt_node;
		}
		std::cerr << "----------info end----------" << std::endl;
		if(ass){
			exit(0);
		}
	}
};
/*封印
template<std::uint_fast64_t p=100000>
class MultiInt{
  private:
    std::vector<long long> _num;
    constexpr void shift(std::vector<long long> &a)const{
        size_t k=0;
        for(;k+1<a.size();++k){
            if(a[k]>=0){
                a[k+1] += a[k]/p;
                a[k] %= p;
            }
            else{
                long long n=(-a[k]+p-1)/p;
                a[k] += p*n;
                a[k+1] -= n;
            }
        }
        while(a[k]<=-(long long)p){
            long long n=(-a[k]+p-1)/p;
            a[k++] += p*n;
            a.push_back(-n);
        }
        while(a[k]>=(long long)p){
            a.push_back(0);
            a[k+1] += a[k]/p;
            a[k++] %= p;
        }
        while(a.back()==0) a.pop_back();
        if(a.empty()) a.push_back(0);
    }
    struct Decimal{
        MultiInt x;
        size_t k;
        Decimal cut(size_t t){if(t==x._num.size()) x=MultiInt(0);else x>>=t;k-=t;return *this;}
        Decimal(){}
        Decimal(const MultiInt &xt,const size_t &kt):x(xt),k(kt){}
        Decimal operator+(const Decimal &y)const{
            if(k<=y.k){
                MultiInt z=x;
                z<<=y.k-k;
                z+=y.x;
                return Decimal(z,y.k);
            }
            else{
                MultiInt z=y.x;
                z<<=k-y.k;
                z+=x;
                return Decimal(z,k);
            }
        }
        Decimal operator-(const Decimal &y)const{
            if(k<=y.k){
                MultiInt z=x;
                z<<=y.k-k;
                z-=y.x;
                return Decimal(z,y.k);
            }
            else{
                MultiInt z=y.x;
                z<<=k-y.k;
                z-=x;
                return Decimal(z,k);
            }
        }
        Decimal operator*(const Decimal &y)const{
            return Decimal(x*y.x,k+y.k);
        }
        Decimal operator+(const MultiInt &y)const{
            MultiInt z=y;z<<=k;z+=x;
            return Decimal(z,k);
        }
        Decimal operator-(const MultiInt &y)const{
            MultiInt z=y;z<<k;z-=x;
            return Decimal(z,k);
        }
        Decimal operator*(const MultiInt &y)const{
            return Decimal(x*y,k);
        }
        Decimal operator+=(const Decimal &y){*this = *this + y;return *this;}
        Decimal operator-=(const Decimal &y){*this = *this - y;return *this;}
        Decimal operator*=(const Decimal &y){x*=y.x;k+=y.k;return *this;}
        Decimal operator+=(const MultiInt &y){*this = *this + y;return *this;}
        Decimal operator-=(const MultiInt &y){*this = *this - y;return *this;}
        Decimal operator*=(const MultiInt &y){x*=y;return *this;}
    };
  public:
    constexpr size_t size()const{return _num.size();};
    constexpr int operator[](int k)const{return _num[k];}
    explicit constexpr MultiInt(long long num=0){
        if(num>=0){
            while(num){
                _num.push_back(num%p);
                num /= p;
            }
            if(_num.empty()) _num.push_back(0);
        }
        else{
            _num.push_back(num);
            shift(_num);
        }
    }
    explicit constexpr MultiInt(std::vector<long long> num):_num(num){shift(_num);}
    constexpr MultiInt(const MultiInt &cp):_num(cp._num){}
    constexpr MultiInt operator+(const MultiInt &x)const{
        size_t sz = std::max(_num.size(),x._num.size());
        std::vector<long long> c(sz);
        for(size_t i=0;i<sz;++i){
            if(i<_num.size()) c[i] += _num[i];
            if(i<x._num.size()) c[i] += x._num[i];
        }
        return MultiInt(c);
    }
    constexpr MultiInt operator-(const MultiInt &x)const{
        size_t sz = std::max(_num.size(),x._num.size());
        std::vector<long long> c(sz);
        for(size_t i=0;i<sz;++i){
            if(i<_num.size()) c[i] += _num[i];
            if(i<x._num.size()) c[i] -= x._num[i];
        }
        return MultiInt(c);
    }
    constexpr MultiInt operator-(void)const{
        return MultiInt(0)-*this;
    }
    constexpr MultiInt operator*(const MultiInt &x)const{
        size_t N = _num.size(),M = x._num.size();
        size_t i=0,k=0;
        if(N>=M){
            std::vector<long long> res(N+3*M),L(M);
            while(i<N){
                size_t D=i;
                for(;i-D<M;++i){
                    if(i<N) L[i-D]=_num[i];
                    else L[i-D]=0;
                }
                FFT<long long> r(L,x._num);
                for(size_t j=0;j<2*M-1;++j) res[j+k*M] += r[j];
                ++k;
            }
            return MultiInt(res);
        }
        else{
            std::vector<long long> res(M+3*N),R(N);
            while(i<M){
                size_t D=i;
                for(;i-D<N;++i){
                    if(i<M) R[i-D]=x._num[i];
                    else R[i-D]=0;
                }
                FFT<long long> r(_num,R);
                for(size_t j=0;j<2*N-1;++j) res[j+k*N] += r[j];
                ++k;
            }
            return MultiInt(res);
        }
    }
    constexpr MultiInt operator/(const MultiInt &x)const{
        MultiInt m(0),a(1),c2(2);
        size_t n=_num.size()+x._num.size();
        a<<=_num.size();
        c2<<=n;
        while(m!=a){
            m=a;
            a*=c2 - a*x;
            a>>=n;
        }
        a*=(*this);
        a>>=n;
        
        while((a+1)*x<=(*this)){
            a+=1;
        }
        return a;
    }
    constexpr MultiInt operator%(const MultiInt &x)const{return *this - (*this/x)*x;}
    constexpr MultiInt operator+(const long long t)const{
        std::vector<long long> res=_num;
        res[0]+=t;
        if(res[0]>=(long long)p) shift(res);
        return MultiInt(res);
    }
    constexpr MultiInt operator-(const long long &t)const{
        std::vector<long long> res = _num;
        res[0]-=t;
        if(res[0]<0) shift(res);
        return MultiInt(res);
    }
    constexpr MultiInt operator*(const long long &t)const{
        std::vector<long long> res=_num;
        for(size_t i=0;i<res.size();++i) res[i]*=t;
        shift(res);
        return MultiInt(res);
    }
    constexpr MultiInt operator/(const long long &t)const{return *this/MultiInt(t);}
    constexpr MultiInt operator%(const long long &t)const{return *this%MultiInt(t);}

    constexpr bool operator<(const MultiInt &x)const{
        if(_num.back()<0&&x._num.back()<0){
            if(_num.size()!=x._num.size()) return _num.size() > x._num.size();
            else for(size_t i=_num.size()-1;i>=0;--i){
                if(_num[i]!=x._num[i]) return _num[i] < x._num[i];
            }
        }
        else if(_num.back()*x._num.back()<0) return _num.back() < 0;
        else{
            if(_num.size()!=x._num.size()) return _num.size() < x._num.size();
            else for(size_t i=_num.size()-1;i>=0;--i){
                if(_num[i]!=x._num[i]) return _num[i] < x._num[i];
            }
        }
        return false;
    }
    constexpr bool operator>=(const MultiInt &x)const{return !(*this<x);}
    constexpr bool operator==(const MultiInt &x)const{
        if(_num.size()!=x._num.size()) return false;
        for(size_t i=0;i<_num.size();++i){
            if(_num[i]!=x._num[i]) return false;
        }
        return true;    
    }
    constexpr bool operator!=(const MultiInt &x)const{return _num!=x._num;}
    constexpr bool operator>(const MultiInt &x)const{return !(*this==x||*this<x);}
    constexpr MultiInt operator<<(size_t k)const{
        std::vector<long long> res(_num.size()+k);
        for(size_t i=k;i<_num.size()+k;++i) res[i]=_num[i-k];
        return MultiInt(res);
    }
    constexpr bool operator<=(const MultiInt &x)const{return !(*this>x);}
    constexpr MultiInt operator>>(size_t k)const{
        if(_num.size()<k){
            return MultiInt(0);
        }
        std::vector<long long> res(_num.size()-k);
        for(size_t i=0;i+k<_num.size();++i) res[i]=_num[i+k];
        return MultiInt(res);
    }

    constexpr MultiInt& operator+=(const MultiInt &x){
        size_t sz = std::max(_num.size(),x._num.size());
        if(_num.size()<sz) _num.resize(sz);
        for(size_t i=0;i<x._num.size();++i) _num[i]+=x._num[i];
        shift(_num);
        return *this;
    }
    constexpr MultiInt& operator-=(const MultiInt &x){
        size_t sz = std::max(_num.size(),x._num.size());
        if(_num.size()<sz) _num.resize(sz);
        for(size_t i=0;i<x._num.size();++i) _num[i]-=x._num[i];
        shift(_num);
        return *this;
    }
    constexpr MultiInt& operator*=(const MultiInt &x){*this = *this * x;return *this;}
    constexpr MultiInt& operator/=(const MultiInt &x){*this = *this / x;return *this;}
    constexpr MultiInt& operator%=(const MultiInt &x){*this = *this % x;return *this;}

    constexpr MultiInt& operator+=(const long long &a){_num[0]+=a;if(_num[0]>=(long long)p){shift(_num);}return *this;}
    constexpr MultiInt& operator-=(const long long &a){_num[0]-=a;if(_num[0]<0){shift(_num);}return *this;}
    constexpr MultiInt& operator*=(const long long &a){
        for(size_t i=0;i<_num.size();++i) _num[i]*=a;
        shift(_num);
        return *this;
    }
    constexpr MultiInt& operator/=(const long long &a){*this/=MultiInt(a);return *this;}
    constexpr MultiInt& operator<<=(size_t k){
        _num.resize(_num.size()+k);
        for(int i=_num.size()-1-k;i>=0;--i){
            _num[i+k]=_num[i];
            _num[i]=0;
        }
        return *this;
    }
    constexpr MultiInt& operator>>=(size_t k){
        if(_num.size()<k){
            _num={0};
            return *this;
        }
        for(size_t i=0;i+k<_num.size();++i) _num[i]=_num[i+k];
        while(k--) _num.pop_back();
        if(_num.empty()) _num.push_back(0);
        return *this;
    }
    explicit operator long long()const{
        long long q=1,res=0;
        for(size_t i=0;i<_num.size();++i){
            res+=_num[i]*q;q*=p;
        }
        return res;
    }
    explicit operator int()const{
        int q=1,res=0;
        for(size_t i=0;i<_num.size();++i){
            res+=_num[i]*q;q*=p;
        }
        return res;
    }
    friend constexpr MultiInt operator+(const long long a,const MultiInt &x){return x+a;}
    friend constexpr MultiInt operator-(const long long a,const MultiInt &x){return MultiInt(a)-x;}
    friend constexpr MultiInt operator*(const long long a,const MultiInt &x){return x*a;}
    friend constexpr MultiInt operator/(const long long a,const MultiInt &x){return MultiInt(a)/x;}
    friend constexpr std::ostream& operator<<(std::ostream &os,const MultiInt &x){
        size_t i=x.size();
        if(x._num.back()>=0){
            os<<x[--i];
            while(i--) printf("%05d",x[i]);
        }
        else os<<'-'<<MultiInt(-x);
        return os;
    }
    friend std::istream& operator>>(std::istream &is,MultiInt &x){
        long long val;is>>val;
        x=MultiInt(val);
        return is;
    }
};
*/