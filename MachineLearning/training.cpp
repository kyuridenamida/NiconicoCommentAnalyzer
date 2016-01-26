#include <bits/stdc++.h>
using namespace std;
const vector<int> targetTags = {24177};
// 泣ける淫夢: 24177
// 真夏の夜の淫夢: 4068
// クッキー☆: 17521

class Reader{
public:
	static vector<string> split(string s,char c=','){
		for(int i = 0 ; i < s.size() ; i++){
			if( s[i] == c ) s[i] = ' ';
		}
		stringstream ss(s);
		vector<string> ans;
		string t;
		while(ss >> t ) ans.push_back(t);
		return ans;
	}


	//(ケースの固有番号列,{タグ集合}列,学習ベクトル(どの単語が何回使われてるか)列)のtupleを返す．
	static tuple<map<int,int>,vector<int>,vector<set<int>>,vector<vector< pair<int,double> >>> readMainData(string filename,int wordCutThrethold=5){
		vector<int> id;
		vector<vector< pair<int,double> >> trainVecs;
		vector<set<int>>tagsSet;
		map<int,int> relabel;
		map<int,int> counter;
		
		//単語の出現頻度を数える関数・それ以外の情報は無視！動画に複数含まれる場合は1つと考える
		auto preProcessing = [&](){
			ifstream ifs(filename);
			assert( ifs.is_open() );
			int x;
			string str;
			
			while( ifs >> x >> str ){
				int numOfTags;
				ifs >> numOfTags;
				for(int i = 0 ; i < numOfTags ; i++){
					ifs >> x;
				}
				int numOfWords;
				ifs >> numOfWords;
				set<int> vec;
				for(int j = 0 ; j < numOfWords; j++){
					int x;
					ifs >> x;
					vec.insert(x);
				}
				for( auto i : vec ) counter[i]++;
			}
			ifs.close();
		};
		
		
		// ソート済み重複あり列から(単語,回数)の列にする．
		auto weightedArray = [](const vector<int> &arr){
			vector< pair<int,double> > res;
			for( const auto &x : arr ){
				if( res.size() != 0 && res.back().first == x ) res.back().second += 1.0;
				else res.push_back({x,1.0});
			}
			return res;
		};
		
		auto relabelProcessing = [&](){
			int k = 0;
			for( auto x : counter ){
				if( x.second <  wordCutThrethold ) continue;
				// cout << x.first << " " << x.second << endl;
				relabel[x.first] = k++;
			}
		};
		
		auto mainProcessing = [&](){
			ifstream ifs(filename);
			assert( ifs.is_open() );

			int x;
			string str;
			while( ifs >> x >> str ){
				
				id.push_back(x);
				int numOfTags;
				ifs >> numOfTags;
				set<int> tags;
				for(int i = 0 ; i < numOfTags ; i++){
					ifs >> x;
					tags.insert(x);
				}
				int numOfWords;
				ifs >> numOfWords;
				vector<int> vec;
				for(int j = 0 ; j < numOfWords; j++){
					int x;
					ifs >> x;
					if( relabel.count(x) ){
						vec.push_back(relabel[x]);
					}
				}
				sort(vec.begin(),vec.end());
				tagsSet.push_back(tags);
				trainVecs.push_back(weightedArray(vec));
			}
			ifs.close();
		};
		preProcessing();
		relabelProcessing();
		mainProcessing();
		return tuple<map<int,int>,vector<int>,vector<set<int>>,vector<vector< pair<int,double>>>>(relabel,id,tagsSet,trainVecs);
	}
};




class Array{
public:
	int cols;
	vector<double> data;
	Array(){}	
	Array(int cols) : cols(cols){
		data.resize(cols,0);
	}

	Array operator + (const Array &op) const{
		assert( cols == op.cols );
		Array res(cols);
		for(int i = 0 ; i < cols ; i++)
			res.data[i] = data[i] + op.data[i];
		return res;
	}
	Array operator - (const Array &op) const{
		assert( cols == op.cols );
		Array res(cols);
		for(int i = 0 ; i < cols ; i++)
			res.data[i] = data[i] - op.data[i];
		return res;
	}
	Array operator * (const Array &op) const{
		assert( cols == op.cols );
		Array res(cols);
		for(int i = 0 ; i < cols ; i++)
			res.data[i] = data[i] * op.data[i];
		return res;
	}
	Array operator * (const double &b) const{
		Array res(cols);
		for(int i = 0 ; i < cols ; i++)
			res.data[i] = data[i] * b;
		return res;
	}
	Array operator + (const double &b) const{
		Array res(cols);
		for(int i = 0 ; i < cols ; i++)
			res.data[i] = data[i] + b;
		return res;
	}
	Array abs(){
		Array res(cols);
		for(int i = 0 ; i < cols ; i++)
			res.data[i] = fabs(data[i]);
		return res;
	}
	string to_string(){
		stringstream ss;
		ss << "[";
		for(int i = 0 ; i < cols ; i++){
			if(i) ss << ",";
			ss << data[i];
		}
		ss << "]";
		return ss.str();
	}
	
};

class SparseMatrix{
public:
	int rows,cols;
	vector<vector< pair<int,double> >> data;
	SparseMatrix(int rows,int cols) : rows(rows),cols(cols){
		data.resize(rows);
		
	}
	SparseMatrix(int rows,int cols,const vector<vector<pair<int,double>>> &mat_raw) : rows(rows),cols(cols){
		data.resize(rows);
		for(int i = 0 ; i < mat_raw.size() ; i++)
			for(auto j : mat_raw[i] )
				set(i,j.first,j.second);
	}
	void set(int i,int j,double v){
		assert( 0 <= i && i < rows );
		assert( 0 <= j && j < cols );
		data[i].push_back({j,v});
	}
	Array dot(const Array &c) const{
		assert( c.cols == cols );
		
		Array res(rows);
		for(int i = 0 ; i < rows ; i++){
			for( auto j: data[i] ){
				res.data[i] += c.data[j.first] * j.second;
			}
		}
		return res;
	}
};

Array p_y_given_x(const SparseMatrix &x,const Array &w,double b){
	auto sigmoid = [](Array const &a){
		Array res(a.cols);
		for(int i = 0 ; i < a.cols ; i++){
			res.data[i] = 1.0 / (1.0+exp(-a.data[i]));
		}
		return res;
	};
	return sigmoid(x.dot(w)+b);
}
Array meanT(const SparseMatrix &x,const Array &bias){
	Array res(x.cols);
	for(int i = 0 ; i < x.rows ; i++){
		for( auto j: x.data[i] ){
			res.data[j.first] += j.second * bias.data[i];
		}
	}
	for(int i = 0 ; i < res.cols ; i++)
		res.data[i] /= x.rows;
	return res;
}
double mean(Array x){
	double res=0;
	for(int i = 0 ; i < x.cols ; i++){
		res += x.data[i];
	}
	res /= x.cols;
	return res;
}

pair<Array,double> grad(const SparseMatrix &x,const Array &y,const Array &w,double b){
    // 現予測値から勾配を計算
    Array error = y - p_y_given_x(x, w, b);
    auto w_grad = meanT(x,error*(-1.0));
    auto b_grad = -mean(error);
    return {w_grad, b_grad};
}

double gd(const SparseMatrix &x, const Array &y,Array &w,double &b,double eta=0.3){
	// 入力をまとめて処理
	Array w_grad;
	double b_grad;
	tie(w_grad,b_grad) = grad(x, y, w, b);
	w = w - w_grad * eta;
	b = b - b_grad * eta;
	double e = 0;
	return e;
}


pair<double,double> correct_rate(const SparseMatrix &x, const Array &y,Array &w,double &b){
	auto res = p_y_given_x(x, w, b);
	double usedThrethold = 0;
	double maximumRate = 0;
	for(int k = 0 ; k < 1000 ; k++){
		int table[2][2] = {};
		double K = k / 1000.;
		for(int i = 0 ; i < y.cols ; i++){
			int predict = res.data[i] >= K;
			table[(int)y.data[i]][predict]++;
		}
		double rate = 1.0 * (table[0][0] + table[1][1]) / y.cols;
	
		if( rate > maximumRate ){
			usedThrethold = K;
			maximumRate = rate;
		}
		
	}
	return {maximumRate,usedThrethold};
}

Array answerVector(const vector<set<int>> &tags,int target){
	Array res(tags.size());
	double ok = 0;
	for(int i = 0 ; i < tags.size() ; i++)
		if( tags[i].count(target) ) res.data[i] = 1, ok++;
	// cout << ok / tags.size() << "<<" << endl;
	return res;
}
// YES=>1,NO=>0のベクトルを返す．
vector<int> finalResult(const SparseMatrix &x,Array w,double b,double th){
	Array res = p_y_given_x(x, w, b);
	vector<int> answer;
	for(int i = 0 ; i < x.rows ; i++)
		answer.push_back(res.data[i]>=th);
	
	return answer;
}

// lgmする．
tuple<Array,double,double> lgm(const SparseMatrix& train_mat,const Array &train_y,const SparseMatrix &test_mat,const Array& test_y,int numberOfTrain,Array w,double b){
	assert(train_mat.cols == test_mat.cols);
	
	for(int i = 0 ; i < numberOfTrain; i++){
		if( i % 10 == 0 ){
			gd(train_mat,train_y,w,b);
			double A,B;
			tie(A,B) = correct_rate(test_mat,test_y,w,b);
			cerr << i << ": " << A << " " << B << endl;
			tie(A,B) = correct_rate(train_mat,train_y,w,b);
			cerr << i << ": " << A << " " << B <<  " < fitting " << endl;
		}else{
			gd(train_mat,train_y,w,b);
		}
	}
	auto res = correct_rate(test_mat,test_y,w,b);
	double th = res.second;
	return tuple<Array,double,double>(w,b,th);
}


int main(){

	map<int,int> relabel;
	int colSize;
	vector<set<int>> tags_raw;
	vector<int> id_raw;
	vector<vector<pair<int,double>>> mat_raw;
	tie(relabel,id_raw,tags_raw,mat_raw) = Reader::readMainData("trainingdata.txt");
	colSize = relabel.size();
	SparseMatrix all_mat(mat_raw.size(),colSize,mat_raw);
	
	vector<int> part_id_raw[2];
	vector<set<int>> part_tags_raw[2];
	vector<vector<pair<int,double>>> part_mat_raw[2];
	for(int i = 0 ; i < mat_raw.size() ; i++){
		int t=rand()%2;
		part_id_raw[t].push_back(id_raw[i]);
		part_tags_raw[t].push_back(tags_raw[i]);
		part_mat_raw[t].push_back(mat_raw[i]);
	}
	
	map<int,Array> last_w;
	for( int x : targetTags ) last_w[x] = Array(colSize);
	map<int,double> last_b;
	map<int,double> last_th;
	
	SparseMatrix train_mat(part_mat_raw[0].size(),colSize,part_mat_raw[0]);
	SparseMatrix test_mat(part_mat_raw[1].size(),colSize,part_mat_raw[1]);
	
	auto work1 = [&](int target,int trainCount){
		Array test_y = answerVector(part_tags_raw[1],target);
		Array train_y = answerVector(part_tags_raw[0],target);
		
		tie(last_w[target],last_b[target],last_th[target]) = lgm(train_mat,train_y,test_mat,test_y,trainCount,last_w[target],last_b[target]);
	};

	//学習投入
	for(int i = 0 ; i < 10 ; i++){
		map<int,std::thread> threads;
		
		for( int target : targetTags ){
			cerr << "targetTag:" << target << endl;
			threads[target] = std::thread(work1,target,100);
		}
		
		//学習終了待ち
		for( int target : targetTags ){
			threads[target].join();
		}
		
	}
	
	for( int target : targetTags ){
		map<int,int> rev;
		for( auto e : relabel ){
			rev[e.second] = e.first;
		}
		vector< pair<double,int> > v;
		for(int i = 0 ; i < colSize ; i++){
			v.push_back({last_w[target].data[i],rev[i]});
		}
		sort(v.rbegin(),v.rend());
		for( auto x : v )
			cout << x.second << " " << x.first << endl;
	}
	
}