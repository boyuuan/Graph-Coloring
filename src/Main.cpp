#include "GreedyColoring.h"
#include "GeneticColoring.h"
#include <chrono>
int main(int argc, char* argv[]){
	string inFile;
	int x;
	string inputs[6] = {"128", "512", "2048", "8192", "32768", "test"};
	int guess[6] = {2, 4, 35, 63, 81, 3};
	while(1){
		cout<<"select input graph:"<<endl;
		for(int i = 0; i < 6; i ++){
			cout<<"["<<i<<"]: "<<inputs[i]<<endl;
		}
		cin>>x;
		if(x >=0 && x <= 5){
			break;
		}
	}
	inFile = "data/" + inputs[x];
	Graph g;
	g.Input(inFile);
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	GreedyColor(g);
	ofstream time;
	string s = "time";
	s += inputs[x];
	time.open(s);
	//GeneticColor(g, guess[x]);
	g.Output("../results/G" + inFile.substr(5));
	//g.OutputSpecial("../results/graphs/G" + inFile.substr(5));
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout<<"Elapsed time : "<<chrono::duration_cast<chrono::milliseconds>(end - begin).count()<<"[ms]"<<endl;
	time<<"Elapsed time : "<<chrono::duration_cast<chrono::milliseconds>(end - begin).count()<<"[ms]"<<endl;
	time.close();
	return 0;
}
