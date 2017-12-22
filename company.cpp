#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <string>
#include <utility>
#include <stack>
#include <limits>


class Graph {
    private:
        std::vector <std::vector <size_t> > Edges;
    public:
        std::vector<size_t>::const_iterator list_of_edges_fromvertice_begin (size_t vertice) const {
            return Edges[vertice].cbegin();
        }
        std::vector<size_t>::const_iterator list_of_edges_fromvertice_end (size_t vertice) const {
            return Edges[vertice].cend();
        }
        Graph() {}
        Graph (size_t graphsize): Edges (graphsize) {}
        Graph (const Graph& other): Edges(other.Edges) {}
        size_t size() const {
            return Edges.size();
        }
        void pushedge (size_t outcome, size_t income);
        Graph transposedgraph();
};



void Graph::pushedge (size_t outcome, size_t income) {
    if ( outcome >= Edges.size() )
         Edges.resize (outcome+1);
    if ( income >= Edges.size() )
         Edges.resize (income+1);
    Edges[outcome].push_back(income);
}

Graph readgraphfromkeyboard () {
    size_t graphsize, gamesnumber;
    std::cin >> graphsize >> gamesnumber;
    Graph Ourgraph(graphsize);
    for (size_t i=0; i<gamesnumber; ++i) {
        size_t firstplayer, secondplayer, gameresult;
        std::cin >> firstplayer >> secondplayer >> gameresult;
        if (gameresult == 3)
            continue;
        if (gameresult == 2)
            std::swap (firstplayer, secondplayer);
        Ourgraph.pushedge (firstplayer-1, secondplayer-1);
    }
    return Ourgraph;
}


Graph Graph::transposedgraph() {
    Graph Reversedgraph (size() );
    for (size_t verticenumber=0; verticenumber < size(); ++verticenumber) {
        auto it_current = list_of_edges_fromvertice_begin (verticenumber) ;
        auto it_end = list_of_edges_fromvertice_end (verticenumber) ;
        for (; it_current != it_end; ++it_current)
            Reversedgraph.pushedge(*it_current, verticenumber);
    }
    return Reversedgraph;
}



template <typename CallbackType>
class DFS {
    private:
        const Graph* Graph_ptr;
        CallbackType* ourcallback_ptr;
        std::stack <size_t> verticestoinvestigate;
        std::vector <bool> used;
        std::vector <bool> ever_been_pushed;

    public:
        DFS (const Graph& graph, CallbackType& callback = CallbackType () ) :
                                            Graph_ptr (&graph), ourcallback_ptr(&callback) {
            used.resize (Graph_ptr -> size(), false);
            ever_been_pushed.resize (Graph_ptr -> size(), false);
        }
        void DFS_traversal_from_vertice (size_t verticenumber);
        void DFS_execution (const std::vector <size_t> & vertice_order);
        void DFS_execution ();
};

template <typename CallbackType>
void DFS <CallbackType>:: DFS_execution () {
    std::vector <size_t> default_vertice_order (Graph_ptr->size(), 0);
    iota (default_vertice_order.begin(), default_vertice_order.end(), 0);
    DFS_execution (default_vertice_order);
}



template <typename CallbackType>
void DFS <CallbackType>:: DFS_execution (const std::vector <size_t> & vertice_order ) {

	for (size_t number_in_order=0; number_in_order < Graph_ptr-> size(); ++number_in_order ) {

        if ( !used [vertice_order[number_in_order] ] ) {
            verticestoinvestigate.push (vertice_order[number_in_order] );
            ever_been_pushed [vertice_order[number_in_order] ] = true;
        }

        while ( !verticestoinvestigate.empty() )
            DFS_traversal_from_vertice (verticestoinvestigate.top() );
	}
}


template <typename CallbackType>
void DFS <CallbackType>:: DFS_traversal_from_vertice (size_t verticenumber) {

        if ( !used [verticenumber] ) {
          ourcallback_ptr-> onVertexEnter(verticenumber);
          used [verticenumber] = true;

          for (auto vertice_neighboor_it = Graph_ptr-> list_of_edges_fromvertice_begin (verticenumber);
            vertice_neighboor_it!= Graph_ptr-> list_of_edges_fromvertice_end (verticenumber); ++vertice_neighboor_it) {
                    if ( !ever_been_pushed [*vertice_neighboor_it] ) {
                         verticestoinvestigate.push (*vertice_neighboor_it);
                         ever_been_pushed[*vertice_neighboor_it] = true;
                    }
                    ourcallback_ptr-> onEdge(verticenumber, *vertice_neighboor_it);
          }
        }

        else {
            verticestoinvestigate.pop();
            ourcallback_ptr-> onVertexFinish(verticenumber);
        }
}



struct Callbacks {
    public:
        void onEdge (size_t vertice_from, size_t vertice_to) {}
        void onVertexEnter (size_t vertice) {}
        void onVertexFinish (size_t vertice) {}
};

struct CallBack_toget_order: public Callbacks {
    public:
        std::vector <size_t> order;
        void onVertexFinish (size_t vertice) {
            order.push_back(vertice);
        }
        CallBack_toget_order (size_t graphsize){}
};


struct CallBack_toget_components: public Callbacks { /*looking for a minimal strongly connected component size possible for
                                                    a strongly connected component with no edges from it
                                                    to other strongly connected components*/
    public:
        size_t current_minimum_comp_size;
        size_t current_comp_size;
        size_t number_of_vertices_in_component_left_to_leave;
        std::vector <bool> is_in_current_component;
        bool there_is_edge_outside_the_component;
        std::vector <size_t> component_vertices;
        std::vector <bool> used;


        void onEdge (size_t vertice_from, size_t vertice_to) {
            if (used[vertice_to]  && !is_in_current_component[vertice_to] )
                  there_is_edge_outside_the_component = true;
        }


        void onVertexEnter (size_t vertice) {
            used[vertice] = true;
            component_vertices.push_back (vertice);
            is_in_current_component[vertice] = true;
            ++number_of_vertices_in_component_left_to_leave;
            ++current_comp_size;
        }


        void onVertexFinish (size_t vertice) {
            --number_of_vertices_in_component_left_to_leave;

            if (number_of_vertices_in_component_left_to_leave == 0) {
                if (!there_is_edge_outside_the_component && current_comp_size < current_minimum_comp_size)
                      current_minimum_comp_size = current_comp_size;

                while( !component_vertices.empty() ) {
                    is_in_current_component[component_vertices.back() ] = false;
                    component_vertices.pop_back();
                }
                there_is_edge_outside_the_component = false;
                current_comp_size = 0;
            }
        }

        CallBack_toget_components () {
            current_minimum_comp_size = std::numeric_limits <size_t>::max();
            there_is_edge_outside_the_component = false;
            current_comp_size = 0;
            number_of_vertices_in_component_left_to_leave = 0;
        }
        CallBack_toget_components (size_t graphsize): CallBack_toget_components () {
            is_in_current_component.resize (graphsize, false);
            used.resize (graphsize, false);
        }
};



template <typename CallbackType>
CallbackType DoDFS (const Graph& Ourgraph, const std::vector <size_t>& vertice_order) {
    CallbackType ourcallback(Ourgraph.size() );
    DFS <CallbackType> dfs_for_our_callback (Ourgraph, ourcallback);
    dfs_for_our_callback.DFS_execution (vertice_order);
    return ourcallback;
}


template <typename CallbackType>
CallbackType DoDFS (const Graph& Ourgraph) {
    std::vector <size_t> vertice_order (Ourgraph.size(), 0);
    iota(vertice_order.begin(), vertice_order.end(), 0);
    return DoDFS <CallbackType> (Ourgraph, vertice_order);
}


size_t answer (const Graph& Ourgraph, const Graph& Reversedgraph) {
    CallBack_toget_order our_callBack_toget_order = DoDFS <CallBack_toget_order> (Ourgraph);
    std::reverse (our_callBack_toget_order.order.begin(), our_callBack_toget_order.order.end() );

    CallBack_toget_components our_callBack_toget_components = DoDFS <CallBack_toget_components>
                                                                (Reversedgraph, our_callBack_toget_order.order);

    return Ourgraph.size() - our_callBack_toget_components.current_minimum_comp_size + 1;
}

int main() {
    Graph Ourgraph = readgraphfromkeyboard();
    Graph Reversedgraph = Ourgraph.transposedgraph();
    std::cout <<answer(Ourgraph, Reversedgraph) <<std::endl;
    return 0;
}
