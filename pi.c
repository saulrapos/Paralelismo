#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int rank, numprocs, n, i, count = 0, total_count = 0;
    double x, y, z, pi, PI25DT = 3.141592653589793238462643;
    int points_per_process;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      // id del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  // número total de procesos

    if (rank == 0) {
        printf("Enter the number of points: (0 quits) \n");
        scanf("%d", &n);
    }

    // Broadcast n a todos los procesos
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n == 0) {
        MPI_Finalize();
        return 0;
    }

    // Para asegurar aleatoriedad distinta por proceso
    srand(time(NULL) + rank * 1000);

    // Repartimos el trabajo entre procesos
    for (i = rank; i < n; i += numprocs) {
        x = ((double) rand()) / RAND_MAX;
        y = ((double) rand()) / RAND_MAX;
        z = sqrt((x * x) + (y * y));
        if (z <= 1.0)
            count++;
    }

    // Recogida de resultados en el proceso 0
    MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        pi = ((double) total_count / (double) n) * 4.0;
        printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    }

    MPI_Finalize();
    return 0;
}
