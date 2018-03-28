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
	if (rank == 0) {
		get_node_stat(&nodes, &in_links, &out_links);
	}
	
	MPI_Bcast(&nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (rank != 0) {
		in_links = malloc(nodes * sizeof(int));
		out_links = malloc(nodes * sizeof(int));
	}
	
	MPI_Bcast(in_links, nodes, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(out_links, nodes, MPI_INT, 0, MPI_COMM_WORLD);
	
	double *ranks = malloc(nodes * sizeof(double));
	double *new_ranks = malloc(nodes * sizeof(double));
	double *tmp = malloc(nodes * sizeof(double));
	
	node_init(&nodehead, in_links, out_links, 0, nodes);
	
	double start, end;
	GET_TIME(start);
	
	double _n = ((double)1.0) / ((double)nodes);
	for (int i = 0; i < nodes; ++i) {
		new_ranks[i] = _n;
	}
	
	int per_thread = nodes / coms;
	int index = rank * per_thread;
	int nextIndex = (rank + 1) * per_thread;
	do {
		for (int i = index; i < nextIndex; ++i) {
			ranks[i] = new_ranks[i];
			new_ranks[i] = 0;
			for (int j = 0; j < nodehead[i].num_in_links; ++j) {
				new_ranks[i] += ranks[nodehead[i].inlinks[j]] / out_links[nodehead[i].inlinks[j]];
			}
			new_ranks[i] *= 0.85;
			new_ranks[i] += (1.0 - 0.85) * _n;
		}
		MPI_Allgather(new_ranks, per_thread, MPI_DOUBLE, tmp, per_thread, MPI_DOUBLE, MPI_COMM_WORLD);
		memcpy(new_ranks, tmp, nodes);
	} while (rel_error(new_ranks, ranks, nodes) >= e);
	
	if (rank == 0) {
		GET_TIME(end);
		Lab4_saveoutput(new_ranks, nodes, end - start);
	}
	MPI_Finalize();
	return 0;
}
