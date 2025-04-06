#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int MPI_BinomialBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int rank, size, step;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int i = 1;
    while (i < size) {
        if (rank < i && rank + i < size) {
            MPI_Send(buffer, count, datatype, rank + i, 0, comm);
        } else if (rank >= i && rank < 2 * i) {
            MPI_Recv(buffer, count, datatype, rank - i, 0, comm, MPI_STATUS_IGNORE);
        }
        i *= 2;
    }
    return MPI_SUCCESS;
}

int MPI_FlattreeColectiva(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op,
                          int root, MPI_Comm comm) {
    int rank, size, temp, total = 0;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        total = *((int *)sendbuf);
        for (int i = 1; i < size; i++) {
            MPI_Recv(&temp, 1, datatype, i, 0, comm, MPI_STATUS_IGNORE);
            total += temp;
        }
        *((int *)recvbuf) = total;
    } else {
        MPI_Send(sendbuf, 1, datatype, root, 0, comm);
    }
    return MPI_SUCCESS;
}

int main(int argc, char *argv[]) {
    int rank, numprocs, n, i, count = 0, total_count = 0;
    double x, y, z, pi, PI25DT = 3.141592653589793238462643;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (rank == 0) {
        printf("Enter the number of points: (0 quits) \n");
        scanf("%d", &n);
    }

    // Reemplazamos MPI_Bcast con nuestra implementación binomial
    MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n == 0) {
        MPI_Finalize();
        return 0;
    }

    srand(time(NULL) + rank * 1000);

    for (i = rank; i < n; i += numprocs) {
        x = ((double) rand()) / RAND_MAX;
        y = ((double) rand()) / RAND_MAX;
        z = sqrt((x * x) + (y * y));
        if (z <= 1.0)
            count++;
    }

    // Reemplazamos MPI_Reduce con nuestra colectiva propia tipo flat tree
    MPI_FlattreeColectiva(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        pi = ((double) total_count / (double) n) * 4.0;
        printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    }

    MPI_Finalize();
    return 0;
}
