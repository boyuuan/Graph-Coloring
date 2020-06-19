#ifndef GREEDYCOLORING_H
#define GREEDYCOLORING_H
#include"Graph.h"

//Assign every vertex using the smallest colorId that's not used by its adjacent vertices
void GreedyColor(Graph &g){
	int vCount = g._vertices.size();
	int curColor = 0;

	set<int> colorsUsed;
	vector<int> tempEdges;

	bool debug = false;
	for(int i = 0; i < vCount; i ++){	//vertex id
		colorsUsed.clear();
		tempEdges = g.GetEdgesById(i);
		for(int j = 0; j < tempEdges.size(); j ++){	//for each edge of this vertex
			colorsUsed.insert(g.GetColor(tempEdges[j]));
		}
		int k;

		for(k = 0; k <= g.GetColorCount(); k ++){
			if(colorsUsed.find(k) == colorsUsed.end()){
				break;
			}
		}
		g.AssignColor(i, k);
	}
}
#endif
