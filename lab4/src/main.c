#define LAB4_EXTEND
#include "Lab4_IO.h"
#include "timer.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const double e = 1e-5;

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	
	int coms, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &coms);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int nodes = 0;
	int* in_links;
	int* out_links;
	struct node* nodehead;
	
	get_node_stat(&nodes, &in_links, &out_links);
	
	int per_thread = nodes / coms;
	int index = rank * per_thread;
	int nextIndex = (rank + 1) * per_thread;
	
	double *ranks = malloc(nodes * sizeof(double));
	double *new_ranks = malloc(nodes * sizeof(double));
	double *local = malloc(per_thread * sizeof(double));
	
	node_init(&nodehead, in_links, out_links, 0, nodes);
	
	double start, end;
	GET_TIME(start);
	
	double _n = ((double)1.0) / ((double)nodes);
	for (int i = 0; i < nodes; ++i) {
		new_ranks[i] = _n;
	}
	
	do {
		vec_cp(new_ranks, ranks, nodes);
		for (int i = index; i < nextIndex; ++i) {
			local[i-index] = 0;
			for (int j = 0; j < nodehead[i].num_in_links; ++j) {
				local[i-index] += ranks[nodehead[i].inlinks[j]] / out_links[nodehead[i].inlinks[j]];
			}
			local[i-index] *= 0.85;
			local[i-index] += (1.0 - 0.85) * _n;
		}
		MPI_Allgather(local, per_thread, MPI_DOUBLE, new_ranks, per_thread, MPI_DOUBLE, MPI_COMM_WORLD);
	} while (rel_error(new_ranks, ranks, nodes) >= e);
	
	if (rank == 0) {
		GET_TIME(end);
		Lab4_saveoutput(new_ranks, nodes, end - start);
	}
	MPI_Finalize();
	return 0;
}
