#define LAB4_EXTEND
#include "Lab4_IO.h"
#include "timer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const double e = 1e-5;

int main(int argc, char* argv[]){
	double start, end;
	GET_TIME(start);
	
	int nodes;
	int* in_links;
	int* out_links;
	get_node_stat(&nodes, &in_links, &out_links);
	
	struct node *nodehead;
	node_init(&nodehead, in_links, out_links, 0, nodes);
	
	double *ranks = malloc(nodes * sizeof(double));
	double *new_ranks = malloc(nodes * sizeof(double));
	double _n = ((double)1.0) / ((double)nodes);
	for (int i = 0; i < nodes; ++i) {
		new_ranks[i] = _n;
	}
	
	do {
		for (int i = 0; i < nodes; ++i) {
			ranks[i] = new_ranks[i];
			new_ranks[i] = 0;
			for (int j = 0; j < nodehead[i].num_in_links; ++j) {
				new_ranks[i] += ranks[nodehead[i].inlinks[j]] / out_links[nodehead[i].inlinks[j]];
			}
			new_ranks[i] *= 0.85;
			new_ranks[i] += (1.0 - 0.85) * _n;
		}
	} while (rel_error(new_ranks, ranks, nodes) >= e);
	
	GET_TIME(end);
	Lab4_saveoutput(new_ranks, nodes, end - start); // TODO:
	return 0;
}
