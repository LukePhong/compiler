

#include "../include/DomTreeGen.h"

#include "Function.h"

/*-------------------------------------
 | build the graph from a list of arcs
 *------------------------------------*/

void DominatorGraph::buildGraph(int _nvertices, int _narcs, int _source, int *arclist)  
{
    const bool verbose = false;
    int v;

    if (verbose)
        fprintf(stderr, "Building graph...\n");
    deleteAll(); // just in case

    n = _nvertices;
    narcs = _narcs;
    source = _source;

    // initialize arrays
    // 我们这里只存arc不存节点，first_in是指向所有in_arc中该节点连接的第一个in_arc的指针，first_out也类似
    // 以first_in为例，first_in[v]里面保存了v这个点的第一个in_arc在哪，in_arc中的数字则意味着这个arc连到的另一个点是谁
    first_in = new intptr[n + 2];
    first_out = new intptr[n + 2];
    in_arcs = new int[narcs];
    out_arcs = new int[narcs];

    // temporarily, first_in and first_out will represent the degrees
    for (v = n + 1; v >= 0; v--)
    {
        first_in[v].value = first_out[v].value = 0;
    }

    // update the degrees of everybody
    // arclist里面存的两两是一条边(from, to)
    int *a = &arclist[0];
    int *stop = &arclist[2 * narcs];    // 这个应该已经出去arclist了
    while (a != stop)
    {
        int v = *(a++);
        int w = *(a++);
        first_in[w].value++;  // indegree of w increases
        first_out[v].value++; // outdegree of v increases
    }

    // make first_in and first_out point to the position after the last one
    // 点的标号应该从1开始
    first_in[0].value = first_out[0].value = 0;
    for (v = 1; v <= n + 1; v++)
    {
        first_in[v].value = first_in[v - 1].value + first_in[v].value;
        first_out[v].value = first_out[v - 1].value + first_out[v].value;
    }

    // insert the arcs; in the process, last_in and last_out will end up being correct
    a = &arclist[2 * narcs - 1];
    stop = &arclist[0];
    while (a >= stop)
    {
        int w = *(a--);
        int v = *(a--);                       // arc is (v,w)
        in_arcs[--(first_in[w].value)] = v;   //(v,w) is an incoming arc for w
        out_arcs[--(first_out[v].value)] = w; //(v,w) is an outgoing arc for v
    }

    onarcs = narcs;

    // eliminate duplicate arcs
    //  if (remove_duplicates) {
    //          //fprintf (stderr, "Eliminating duplicates...\n");
    //          eliminateDuplicates(first_in, in_arcs);
    //          eliminateDuplicates(first_out, out_arcs);
    //          narcs = first_in[n+1].value;
    //  }

    // convert indices to pointers for faster accesses
    for (int v = 1; v <= n + 1; v++)
    {
        first_in[v].ptr = &in_arcs[first_in[v].value];
        first_out[v].ptr = &out_arcs[first_out[v].value];
    }
}

/*------------------------
 | pre-dfs (with parents)
 *-----------------------*/

void DominatorGraph::rpreDFSp(int v, PreDFSParams &params)
{
    int *p, *stop, pre_v;
    pre_v = params.next;
    params.pre2label[params.next] = v;   // v will have the next label
    params.label2pre[v] = params.next++; // v's label is next (and next is incremented)
    getOutBounds(v, p, stop);
    for (; p < stop; p++)
    { // visit all outgoing neighbors
        if (!params.label2pre[*p])
        {
            params.parent[params.next] = pre_v;
            rpreDFSp(*p, params);
        }
    }
}

int DominatorGraph::preDFSp(int v, int *label2pre, int *pre2label, int *parent)
{ //, int &next) {
    PreDFSParams params;
    params.label2pre = label2pre;
    params.pre2label = pre2label;
    params.parent = parent;
    params.next = 1;

    for (int w = n; w >= 0; w--)
        params.label2pre[w] = 0; // everybody unvisited
    rpreDFSp(v, params);         // visit everybody reachable from the root
    return params.next - 1;
}

void DominatorGraph::snca(int root, int *idom)
{
    int bsize = n + 1;
    int *buffer = new int[5 * bsize];
    int *dom = &buffer[0 * bsize]; // not shared
    int *pre2label = &buffer[1 * bsize];
    int *parent = &buffer[2 * bsize]; // shared with ancestor
    int *label = &buffer[3 * bsize];
    int *semi = &buffer[4 * bsize];

    int *label2pre = idom; // indexed by label

    resetcounters();

    // initialize semi and label
    int i;
    for (i = n; i >= 0; i--)
        label[i] = semi[i] = i;

    int N;
    N = preDFSp(root, label2pre, pre2label, parent);
    for (i = 0; i < 10; i++)
        printf("%d: %d\n", i, pre2label[i]);
    printf("%d, %d\n", N, parent[root]);

    // N = readDFS("data.dimacs.parents", "data.dimacs.preorder", parent, pre2label, label2pre);
    // printf("%d, %d, %d, %d\n", N, parent[root], pre2label[0], label2pre[0]);

    /*----------------
     | semidominators
     *---------------*/
    for (i = N; i > 1; i--)
    {
        int *p, *stop;
        dom[i] = parent[i]; // can't put dom and parent together

        // process each incoming arc
        getInBounds(pre2label[i], p, stop);
        for (; p < stop; p++)
        {
            int v = label2pre[*p];
            if (v)
            {
                int u;
                incc();
                if (v <= i)
                {
                    u = v;
                } // v is an ancestor of i
                else
                {
                    rcompress(v, parent, label, i);
                    u = label[v];
                }
                incc();
                if (semi[u] < semi[i])
                    semi[i] = semi[u];
            }
        }
        label[i] = semi[i];
    }
    printf("%d: %d\n", 1, pre2label[1]);
    printf("root: %d\n", root);

    /*-----------------------------------------------------------
     | compute dominators using idom[w]=NCA(I,parent[w],sdom[w])
     *----------------------------------------------------------*/
    dom[1] = 1;
    idom[root] = root;
    for (i = 2; i <= N; i++)
    {
        int j = dom[i];
        while (j > semi[i])
        {
            j = dom[j];
            incc();
        }
        incc();
        dom[i] = j;
        idom[pre2label[i]] = pre2label[dom[i]];
    }

    // cleanup stuff
    delete[] buffer;
}

/*---------------------------------------------*/
DomTreeGen::DomTreeGen()
{
}

DomTreeGen::~DomTreeGen()
{
    delete idom;
}

void DomTreeGen::pass(Function *func)
{
    currFunc = func;

    int numBlk = func->getBlockList().size();
    int numArc = 0;
    int cnt = 0;
    for (auto &&i : func->getBlockList())
    {   
        cnt++;
        pIdx[i] = cnt;
        idxP[cnt] = i;
        numArc += i->getNumOfSucc();
    }
    int arcArr[2 * numArc];
    cnt = 0;
    // 外层获得func所有基本块
    for (vector<BasicBlock*>::iterator i = func->begin(); i != func->end(); i++)
    {
        // 内层获得某一个基本块的所有后继
        for (vector<BasicBlock*>::iterator j = (*i)->succ_begin(); j != (*i)->succ_end(); j++)
        {
            arcArr[cnt] = (i - func->begin()) + 1;
            cnt++;       
            arcArr[cnt] = std::find(func->begin(), func->end(), *j) - func->begin() + 1;
            cnt++;
        }
    }
    DominatorGraph dom;
    dom.buildGraph(numBlk, numArc, 1, arcArr);
    idom = new int[numBlk + 1];
    dom.snca(1, idom);
    cout<<"Print idom:"<<endl;
    cnt = 0;
    for (vector<BasicBlock*>::iterator i = func->begin(); i != func->end(); i++)
    {
        cnt++;
        // cout<<"("<<(*i)->getNo()<<","<<noIdx[(*i)->getNo()]<<")\t";
        // cout<<"the idom of "<<(*i)->getNo()<<" is "<<idxNo[idom[cnt]]<<endl;
        cout<<"the idom of "<<(*i)->getNo()<<" is "<<getIdom(*i)->getNo()<<endl;
    }
    cout<<endl;

    setIdom();
    ComputeDomFrontier();
    
    currFunc = nullptr;
}

BasicBlock* DomTreeGen::getIdom(BasicBlock* b){
    return idxP[idom[pIdx[b]]];
}

void DomTreeGen::setIdom(){
    for (auto &&i : currFunc->getBlockList()){
        if(currFunc->getEntry() == i){
            i->setIdom(nullptr);
            continue;
        }
        i->setIdom(getIdom(i));
    }
};

void DomTreeGen::ComputeDomFrontier(){
    // from Cooper's book
    for (auto &&i : currFunc->getBlockList())
    {
        if(i->getNumOfPred() >= 2){
            for (vector<BasicBlock*>::iterator j = i->pred_begin(); j != i->pred_end(); j++)
            {
                BasicBlock *runner = *j;
                while(runner != getIdom(i)){
                    i->addDomFrontier(runner);
                    runner = getIdom(runner);
                }
            }
        }
    }
    
}   
