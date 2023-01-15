
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h> 
#include <math.h>
#include <vector>
#include <map>
using namespace std;

class DominatorGraph {
	private:
		int n; //number of vertices in the graph
		int narcs;  //number of arcs in the graph
		int onarcs; //original number of arcs in the graph (before duplicates are removed)
		int source;

#ifdef COUNTOPS
		#define incc() {ccount++;}
		#define inci() {icount++;}
		#define incs() {if(semi[i]==parent[i])scount++;}
		#define resetcounters() {icount=scount=ccount=0;}
#else
		#define incc() {}
		#define inci() {}
		#define incs() {}
		#define resetcounters() {}
#endif

		//aggregate type for DFS parameters
		typedef struct {
			union {int *label2post; int *label2pre;}; 
			union {int *post2label; int *pre2label;};
			int next;
			int *parent;
		} DFSParams;

		typedef DFSParams PostDFSParams;
		typedef DFSParams PreDFSParams;

		/*----------------
		 | adjacency list 
		 *---------------*/

		//structure used for building the graph
		typedef union {
			int value;	
			int *ptr;
		} intptr;

		intptr *first_in;  //first_in[v]: pointer to first element in 'in_arcs' representing a neighbor of v
		intptr *first_out; //first_out[v]: pointer to first element in 'out_arcs' representing a neighbor of v
		int *in_arcs;   //list of incoming arcs (arcs with the same destination are contiguous)
		int *out_arcs;  //list of outgoing arcs (arcs with different destinations are contiguous)

		inline void getOutBounds (int v, int * &start, int * &stop) const  {
			start = first_out[v].ptr;
			stop = first_out[v+1].ptr;
		}

		inline void getInBounds (int v, int *&start, int *&stop) const {
			start = first_in[v].ptr;
			stop = first_in[v+1].ptr;
		}
	
		inline int *getFirstIn(int v) const {return first_in[v].ptr;}
		inline int *getBoundIn(int v) const {return first_in[v+1].ptr;}


		/*----------------
		 | initialization 
		 *---------------*/
		void deleteAll() {
			if (first_in) delete [] first_in;
			if (first_out) delete [] first_out;
			if (in_arcs) delete [] in_arcs;
			if (out_arcs) delete [] out_arcs;
		}

		void reset() {
			icount=scount=ccount=0;
			in_arcs = out_arcs = NULL;
			first_out = first_in = NULL;
			n = narcs = source = 0;
		}

		inline void rcompress (int v, int *parent, int *label, int c) {
			incc();
			int p;
			if ((p=parent[v])>c) {
				rcompress (p, parent, label, c); //does not change parent[v]
				incc();
				if (label[p]<label[v]) label[v] = label[p];
				parent[v] = parent[p];
			}
		}
	
	public:
		int ccount; //comparison counter
		int icount; //iteration counter
		int scount; //sdom=parent counter

		inline int getNVertices() const {return n;}
		inline int getNArcs() const {return narcs;}
		inline int getOriginalNArcs() const {return onarcs;}
		inline int getSource() const {return source;}

		/*-----------------------------
		 | initialization / destructor 
		 *----------------------------*/
		DominatorGraph() {reset();}
		void buildGraph (int _nvertices, int _narcs, int _source, int *arclist); //from list of arcs
		~DominatorGraph() {deleteAll();}

		void rpreDFSp (int v, PreDFSParams &params);
		int preDFSp (int v, int *label2pre, int *pre2label, int *parent);

		void snca (int r, int *idom); //former snca_v2

};


class Function;
class BasicBlock;


class DomTreeGen
{
private:
	Function* currFunc;
	map<BasicBlock*, int> pIdx;
    map<int, BasicBlock*> idxP;
	int* idom;

public:
    DomTreeGen();
    ~DomTreeGen();

public:
    void pass(Function *func);
private:
	void ComputeDomFrontier();
	BasicBlock* getIdom(BasicBlock* b);	// 返回块号
	void setIdom();
};


