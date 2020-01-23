#include<vector>
#include<utility>
#include<functional>
template<typename T> class SegmentTree{
  private:
    typedef std::function<T(T,T)> F;
    int n;
    T d0;
    std::vector<T> vertex;
    F f;
    F g;
  public:
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
    T query(int l,int r){
        T vl=d0,vr=d0;
        l+=n-1;
        r+=n-1;
        for(;l<=r;l/=2,r=r/2-1){
            if(l%2==0) vl=f(vl,vertex[l]);
            if(r&1) vr=f(vr,vertex[r]);
        }
        return f(vl,vr);
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