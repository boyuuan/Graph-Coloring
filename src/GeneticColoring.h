#ifndef GENETIC_COLORING
#define GENETIC_COLORING
#include "Graph.h"

ofstream outfile;

void TestLog(int a){
	cout<<"testing "<<a<<"=========================="<<endl;
}

class Individual : public Graph{
	public:
		int fitness;//number of bad edges: the smaller the fitter
		int guess; //number of colors needed by greedy-coloring

		Individual(){
		}

		
		Individual(const Individual &in){
			_vertices = in._vertices;
			_edges = in._edges;
			_idTable = in._idTable;
			_nameTable = in._nameTable;
			_colors = in._colors;
			_newVId = in._newVId;
			fitness = in.fitness;
			_edgePairs = in._edgePairs;
			guess = in.guess;
			cliques = in.cliques;
		}
		

		Individual(const Graph &g, int gs){
			_vertices = g._vertices;
			_edges = g._edges;
			_idTable = g._idTable;
			_nameTable = g._nameTable;
			_colors = g._colors;
			_newVId = g._newVId;
			_edgePairs = g._edgePairs;
			guess = gs;
			cliques = g.cliques;
			AssignRand();
			UpdateFitness();
		}


		void AssignRand(){
			int colorID = 0;
			double value;
			int nv = _vertices.size();
			for(int i = 0; i < nv; i ++){
				colorID = rand() % (guess + 5);
				AssignColor(i, colorID);
			}
		}

		//Larger means fitter
		void UpdateFitness(){
			int v1;
			int fit = 0;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				v1 = it->first;
				for(int v2 : it->second){
					if(_colors[v1] != _colors[v2]){
						fit ++;
					}
				}
			}
			fit /= 2;
			if(fit == EdgeCount()){
				fit += _vertices.size() - GetColorCount();
			}
			fitness = fit;
		}

		int GetFitness(){
			UpdateFitness();
			return fitness;
		}

		set<int> GetNeiborColors(int vid){
			set<int> result;
			for(int neibor : _edges[vid]){
				result.insert(_colors[neibor]);
			}
			return result;
		}

		/*
		void MutateLocal(){
			int v1;
			set<int> neiborColors;
			int newColor;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				v1 = it->first;
				for(int v2 : it->second){
					if(_colors[v1] == _colors[v2]){
						while(true){
							neiborColors = GetNeiborColors(v1);
							newColor = rand() % GetColorCount();
							if(neiborColors.find(newColor) == neiborColors.end()){
								AssignColor(v1, newColor);
								break;
							}
							if(neiborColors.size() >= GetColorCount()){
								AssignColor(v1, GetColorCount());
								break;
							}
						}
					}
				}
			}
		}
		*/

		set<int> GetAvailableColors(int v){
			set<int> result;
			int allC = GetColorCount();
			set<int> neiborColors = GetNeiborColors(v);
			for(int i = 0; i < allC + 1; i ++){
				if(neiborColors.find(i) == neiborColors.end()){
					result.insert(i);
				}
			}
			return result;
		}

		void MutateLocal(){
			int allC = GetColorCount();
			int offset = 0;
			set<int> neiborColors;
			set<int> avail;
			int newColor;
			int r;
			set<int>::iterator x;
			for(pair<int, int> p : _edgePairs){
				if(_colors[p.first] == _colors[p.second]){
					avail = GetAvailableColors(p.first);
					x = avail.begin();
					r = rand() % avail.size();
					advance(x, r);
					newColor = *x;
					AssignColor(p.first, newColor);
				}
			}
		}

		void MutateGlobal(){
			int v1;
			int newColor;
			set<int> neiborColors;
			set<int>::iterator x;
			for(pair<int, int> p : _edgePairs){
				if(_colors[p.first] == _colors[p.second]){
					neiborColors = GetNeiborColors(p.first);
					x = neiborColors.begin();
					advance(x, rand() % neiborColors.size());
					newColor = *x;
					AssignColor(p.first, newColor);
				}
			}
		}

		void MutateRotation(){
			int temp;
			for(Clique c : cliques){
				temp = _colors[c.v1];
				AssignColor(c.v1, _colors[c.v2]);
				AssignColor(c.v2, _colors[c.v3]);
				AssignColor(c.v3, temp);
			}
		}


		void PrintInd(){
			outfile<<"Print Individual"<<endl;
			for(map<int, int>::iterator it = _colors.begin(); it != _colors.end(); it ++){
				outfile<<it->first<<" -> "<<it->second<<endl;
			}
			outfile<<endl;
		}

		int WrongColorCount(){
			if(GetFitness() > EdgeCount()){
				return 0;
			}
			return EdgeCount() - GetFitness();
		}

};

vector<Individual> CrossOver(Individual p1, Individual p2){
	int temp;
	int crossPoint = rand() % p1.GetVertices().size();
	for(int i = 0; i < crossPoint; i ++){
		temp = p1.GetColor(i);
		p1.AssignColor(i, p2.GetColor(i));
		p2.AssignColor(i, temp);
	}
	vector<Individual> result;
	result.push_back(p1);
	result.push_back(p2);
	return result;
}

Individual SelectParentRand(vector<Individual> population){
	int r1 = rand() % population.size();
	int r2 = rand() % population.size();
	Individual p1 = population[r1];
	Individual p2 = population[r2];
	if(p1.GetFitness() < p2.GetFitness()){
		return p2;
	}
	else{
		return p1;
	}
}

int SelectBestIndividual(vector<Individual> population){
	int bestIndex;
	int bestFitness = INT_MIN;
	for(int i = 0; i < population.size(); i ++){
		if(population[i].GetFitness() > bestFitness){
			bestIndex = i;
			bestFitness = population[i].GetFitness();
		}
	}
	return bestIndex;
}

int SelectWorstIndividual(vector<Individual> population){
	int worstID;
	int worstFitness = INT_MAX;
	for(int i = 0; i < population.size(); i ++){
		if(population[i].GetFitness() < worstFitness){
			worstID = i;
			worstFitness = population[i].GetFitness();
		}
	}
	return worstID;
}

int OverallFitness(vector<Individual> population){
	int fit = 0;
	for(Individual ind : population){
		fit += ind.GetFitness();
	}
	return fit;
}

void PrintPop(vector<Individual> population){
	map<int, int> coloring;
	for(int i = 0; i < population.size(); i ++){
		//outfile<<"Individual"<<i<<":"<<endl;
		coloring = population[i]._colors;
		for(map<int, int>::iterator it = coloring.begin(); it != coloring.end(); it ++){
			outfile<<it->first<<" -> "<<it->second<<endl;
		}
	}
}

void GeneticColor(Graph &g, int guess){
	ofstream fitnessOutfile;
	outfile.open("log" + to_string(guess));
	fitnessOutfile.open("fitness" + to_string(guess));
	int prevOverallFit = -1;
	int stableCounter = 0;
	srand(time(0));
	int N = 200;
	int threshold = 4;
	float MutationP = 0.7;
	Individual best;
	Individual result;
	Individual p1;
	Individual p2;
	int parent1;
	int parent2;
	vector<Individual> children;
	float mutationProb;
	int worst;
	vector<Individual> population;
	int n_pop = 25;
	for(int i = 0; i < n_pop; i ++){
		Individual temp(g, guess);
		population.push_back(temp);
	}
	//PrintPop(population);
	result = population[SelectBestIndividual(population)];
	cout<<"loop starts"<<endl;
	//main loop
	while(N --){
		outfile<<"N = "<<N<<endl;
		int x = SelectBestIndividual(population);
		best = population[x];
		if(best.GetFitness() > result.GetFitness()){
			result = best;
		}
		//planA
		if(best.WrongColorCount() > threshold){
			outfile<<"PlanA"<<endl;
			worst = SelectWorstIndividual(population);
			outfile<<"The first worst Individual.fitness = "<<population[worst].GetFitness()<<endl;
			population.erase(population.begin() + worst);
			worst = SelectWorstIndividual(population);
			population.erase(population.begin() + worst);
			p1 = SelectParentRand(population);
			p2 = SelectParentRand(population);
			children = CrossOver(p1, p2);
			mutationProb = (double)rand() / RAND_MAX;
			if(mutationProb < MutationP){
				outfile<<"children[0] before mutation: fit = "<<children[0].GetFitness()<<endl;
				children[0].PrintInd();
				children[0].MutateLocal();
				outfile<<"children[0] after mutation: fit = "<<children[0].GetFitness()<<endl;
				children[0].PrintInd();
			}
			mutationProb = (double)rand() / RAND_MAX;
			if(mutationProb < MutationP){
				children[1].MutateLocal();
			}
			population.push_back(children[0]);
			population.push_back(children[1]);
		}
		//planB
		else{
			outfile<<"PlanB"<<endl;
			parent1 = SelectBestIndividual(population);
			p1 = population[parent1];
			outfile<<"p1 before mutation: fit = "<<p1.GetFitness()<<" Colors = "<<p1.GetColorCount()<<endl;
			p1.PrintInd();
			p1.MutateGlobal();
			outfile<<"p1 after mutation: fit = "<<p1.GetFitness()<<" Colors = "<<p1.GetColorCount()<<endl;
			p1.PrintInd();
			population.erase(population.begin() + parent1);
			population.push_back(p1);
		}
		if(prevOverallFit == OverallFitness(population)){
			stableCounter ++;
		}
		else{
			stableCounter = 0;
		}
		prevOverallFit = OverallFitness(population);

		if(stableCounter >= 10){
			if(result.GetColorCount() < guess){
				cout<<"GetColorCount = "<<result.GetColorCount()<<endl;
				cout<<"guess = "<<guess<<endl;
				break;
			}
			outfile<<"------------------MutateRotation------------------"<<endl;
			for(int i = 0; i < population.size(); i ++){
				population[i].MutateRotation();
			}
			stableCounter = 0;
		}
		fitnessOutfile<<"OverallFitness = "<<OverallFitness(population)<<endl;
		outfile<<endl;
	}
	cout<<"loop ends"<<endl;
	cout<<"Best Solution: fitness = "<<result.GetFitness()<<", number of colors = "<<result.GetColorCount()<<endl;
	outfile<<"Best Solution: fitness = "<<result.GetFitness()<<", number of colors = "<<result.GetColorCount()<<endl;
	outfile.close();
	fitnessOutfile.close();
}

#endif
