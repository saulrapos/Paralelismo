#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, numprocs, n, i, count_local = 0, count_total = 0;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    // El proceso 0 realiza la entrada de datos
    if (rank == 0) {
        printf("Enter the number of points: (0 quits)\n");
        scanf("%d", &n);
    }

    // Enviar el número de puntos a todos los procesos
    MPI_Send(&n, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    for (int p = 1; p < numprocs; p++) {
        MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
    }

    // Recibir los puntos locales generados
    if (rank != 0) {
        for (i = rank; i <= n; i += numprocs) {
            x = ((double) rand()) / ((double) RAND_MAX);
            y = ((double) rand()) / ((double) RAND_MAX);
            z = sqrt((x * x) + (y * y));
            if (z <= 1.0) {
                count_local++;
            }
        }
        // Enviar los resultados al proceso 0
        MPI_Send(&count_local, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // El proceso 0 recoge los resultados de todos los procesos
    if (rank == 0) {
        // Obtener el resultado del proceso 0
        count_total = count_local;  // Procesar el conteo del propio proceso 0
        for (int p = 1; p < numprocs; p++) {
            MPI_Recv(&count_local, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count_total += count_local;
        }

        // Calcular y mostrar el valor estimado de π
        pi = ((double) count_total / (double) n) * 4.0;
        printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
    }

    // Finalización de MPI
    MPI_Finalize();
    return 0;
}
