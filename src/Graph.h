#ifndef GRAPH_H
#define GRAPH_H
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <climits>
#define NOCOLOR -1
using namespace std;

struct Clique{
	int v1;
	int v2;
	int v3;
};

void RemoveSpace(string &s){
	for(int i = 0; i < s.length(); i ++){
		if(s[i] == ' '){
			s.erase(s.begin() + i);
			i --;
		}
	}
}

class Graph{
	public:
		vector<Clique> cliques;
		set<int> _vertices;
		map<int, string> _nameTable;
		map<string, int> _idTable;
		map<int, vector<int> > _edges;
		map<int, int> _colors;
		int _newVId;
		vector<pair<int, int> > _edgePairs;//single direction
		Graph(){
			_newVId = 0;
		}

		Graph(const Graph &g){
			_vertices = g._vertices;
			_nameTable = g._nameTable;
			_idTable = g._idTable;
			_edges = g._edges;
			_colors = g._colors;
			_newVId = g._newVId;
			_edgePairs = g._edgePairs;
			UpdateCliques();
		}

		bool SameClique(Clique c1, Clique c2){
			set<int> c;
			c.insert(c1.v1);
			c.insert(c1.v2);
			c.insert(c1.v3);
			if(c.find(c2.v1) != c.end()) return true;
			if(c.find(c2.v2) != c.end()) return true;
			if(c.find(c2.v3) != c.end()) return true;
			return false;
		}		

		void UpdateCliques(){
                        int v1;
                        int v2;
                        for(pair<int, int> p : _edgePairs){
                                v1 = p.first;
                                v2 = p.second;
                                for(int v3 : _edges[v2]){
                                        if(HasEdge(v1, v3)){
                                                Clique c{v1, v2, v3};
                                                if(cliques.size() == 0){
                                                        cliques.push_back(c);
                                                }
                                                else{
                                                        for(Clique ori : cliques){
                                                                if(!SameClique(ori, c)){
                                                                        cliques.push_back(c);
                                                                        break;
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                        /*
                        for(Clique c : s){
                                cout<<c.v1<<" "<<c.v2<<" "<<c.v3<<endl;
                        }
                        */
		}

		set<int> GetVertices(){
			return _vertices;
		}

		map<int, vector<int> > GetEdges(){
			return _edges;
		}

		map<int, string> GetNameTable(){
			return _nameTable;
		}

		map<string, int> GetIdTable(){
			return _idTable;
		}

		map<int, int> GetColors(){
			return _colors;
		}

		int GetNewVId(){
			return _newVId;
		}

		int EdgeCount(){
			int count = 0;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				count += it->second.size();
			}
			return count / 2;
		}

		bool HasVertex(int id){
			return _vertices.find(id) != _vertices.end();
		}

		bool HasVertex(string pkg){
			if(_idTable.find(pkg) == _idTable.end()){
				cout<<"Error! Graph data inconsistent. No such pkg: "<<pkg<<endl;
			}
			return _vertices.find(_idTable[pkg]) != _vertices.end();
		}

		string GetPkgNameById(int id){
			if(_nameTable.find(id) == _nameTable.end()){
				cout<<"No such ID: "<<id<<endl;
				return "";
			}
			return _nameTable[id];
		}

		int GetPkgIdByName(string name){
			if(_idTable.find(name) == _idTable.end()){
				cout<<"No such Pkg: "<<name<<endl;
				return -1;
			}
			return _idTable[name];
		}

		vector<int> GetEdgesById(int id){
			if(_edges.find(id) == _edges.end()){
				cout<<"No such vertex with ID = "<<id<<endl;
				return {};
			}
			return _edges[id];
		}

		vector<int> GetEdgesByName(string pkg){
			int id = GetPkgIdByName(pkg);
			return GetEdgesById(id);
		}

		bool HasEdge(int a, int b){
			//cout<<GetPkgNameById(a)<<" ? "<<GetPkgNameById(b);
			if(!HasVertex(a)){
				cout<<"No such ID = "<<a<<endl;
				return false;
			}
			if(!HasVertex(b)){
				cout<<"No such ID = "<<b<<endl;
				return false;
			}
			if(_edges.find(a) == _edges.end()){
				return false;
			}
			vector<int> edgesOfA = _edges[a];
			bool r = find(edgesOfA.begin(), edgesOfA.end(), b) != edgesOfA.end();
			//cout<<" is "<<r<<endl;
			return r;
		}
		
		bool HasEdge(string a, string b){
			return HasEdge(GetPkgIdByName(a), GetPkgIdByName(b));
		}

		//Add a vertex using pkg name to the graph, assigning the color to be NOCOLOR (default)
		void AddVertex(string name){
			if(_idTable.find(name) != _idTable.end()){
				return;
			}
			int id = _newVId ++;
			//cout<<"adding vertex "<<name<<" new id = "<<id<<endl;
			_vertices.insert(id);
			_nameTable[id] = name;
			_idTable[name] = id;
			_colors[id] = NOCOLOR;
		}

		void AddEdge(int a, int b){
			if(HasEdge(a, b)){
				//cout<<"Edge "<<a<<" - "<<b<<" already added"<<endl;
				return;
			}
			if(_edges.find(a) == _edges.end()){
				_edges[a] = vector<int>();
			}
			_edges[a].push_back(b);
			pair<int, int> p{a, b};
			for(pair<int, int> pa : _edgePairs){
				if(pa.first == a && pa.second == b || pa.second == a && pa.first == b)
					return;
			}
			_edgePairs.push_back(p);
		}

		void AddEdge(string a, string b){
			AddEdge(GetPkgIdByName(a), GetPkgIdByName(b));
		}

		//Assign a color id to a vertex id
		void AssignColor(int vID, int colorID){
			if(!HasVertex(vID)){
				cout<<"Cannot add color, No such Edge: "<<vID<<endl;
			}
			if(_colors[vID] != NOCOLOR){
				//cout<<"Overwriting color of ID = "<<vID<<" from colorID = "<<_colors[vID]<<" to colorID = "<<colorID<<endl;
			}
			_colors[vID] = colorID;
		}

		int GetColor(int id){
			return _colors[id];
		}

		bool IsValidColoring(){
			int tempColor1;
			int tempColor2;
			vector<int> tempEdges;
			for(map<int, vector<int>>::iterator it = _edges.begin(); it != _edges.end(); it ++){
				if(_colors.find(it->first) == _colors.end()){
					cout<<"Error! Graph data inconsistent. No such ID = "<<it->first<<" in colors dictionary"<<endl;
					return false;
				}
				tempColor1 = _colors[it->first];
				if(tempColor1 == NOCOLOR) continue;
				tempEdges = it->second;
				for(int i = 0; i < tempEdges.size(); i ++){
					if(_colors.find(tempEdges[i]) == _colors.end()){
						cout<<"Error! Graph data inconsistent. No such ID = "<<it->first<<" in colors dictionary"<<endl;
						return false;
					}
					tempColor2 = _colors[tempEdges[i]];
					if(tempColor1 == tempColor2){
						cout<<"Vid1 = "<<it->first<<endl;
						cout<<"Vid2 = "<<tempEdges[i]<<endl;
						return false;
					}
				}
			}
			return true;
		}

		int GetColorCount(){
			set<int> colors;
			for(map<int, int>::iterator it = _colors.begin(); it != _colors.end(); it ++){
				colors.insert(it->second);
			}
			return colors.size();
		}

		void Input(string fileName){
			cout<<"Reading from "<<fileName<<endl;
			ifstream infile;
			infile.open(fileName.c_str());
			if(!infile){
				cout<<"Error! No such file = "<<fileName<<endl;
				return;
			}
			string temp;
			string pkg1;
			string pkg2;
			size_t pos;
			while(getline(infile, temp)){
				if((pos = temp.find(",")) != string::npos){
					pkg1 = temp.substr(0, pos);
					RemoveSpace(pkg1);
					temp.erase(0, pos + 1);
					pkg2 = temp;
					RemoveSpace(pkg2);

					if(pkg1 == pkg2){
						cout<<"Found: "<<pkg1<<endl;
					}
					//cout<<"pkg1 = "<<pkg1<<endl;
					//cout<<"pkg2 = "<<pkg2<<endl;
					AddVertex(pkg1);
					AddVertex(pkg2);
					AddEdge(pkg1, pkg2);
					AddEdge(pkg2, pkg1);
				}
				else{
					cout<<"Error! Cannot file space in line = "<<temp<<endl;
					return;
				}
			}
			infile.close();
		}
		
		void OutputSpecial(string fileName){
			ofstream outfile;
			outfile.open(fileName.c_str());
			int count = 0;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				count += (it->second).size();
			}

			outfile<<"edge "<<_vertices.size()<<" "<<count / 2;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				for(int i = 0; i < it->second.size(); i ++){
					if(it->first > (it->second)[i]){
						continue;
					}
					outfile<<endl<<"e "<<it->first<<" "<<(it->second)[i];
				}
			}
			outfile.close();
		}

		void Output(string fileName){
			cout<<"Generating "<<fileName<<endl;
			ofstream outfile;
			outfile.open(fileName.c_str());

			outfile<<"Number of vertices"<<endl;
			outfile<<_vertices.size()<<endl;
			outfile<<endl;
			outfile<<"Number of edges (both directions)"<<endl;
			int count = 0;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				count += (it->second).size();
			}
			outfile<<count<<endl;
			outfile<<endl;
			outfile<<"Number of colors used"<<endl;
			outfile<<GetColorCount()<<endl;
			outfile<<endl;
			outfile<<"Vertex List (ID, Name, Color)"<<endl;
			for(int i = 0; i < _vertices.size(); i ++){
				outfile<<i<<" "<<_nameTable[i]<<" "<<_colors[i]<<endl;
			}
			outfile<<endl;
			outfile<<"Edge List (ID ID;)"<<endl;
			for(map<int, vector<int> >::iterator it = _edges.begin(); it != _edges.end(); it ++){
				for(int i = 0; i < it->second.size(); i ++){
					outfile<<it->first<<" "<<(it->second)[i]<<";"<<endl;
				}
			}
			outfile.close();
		}
};
#endif
