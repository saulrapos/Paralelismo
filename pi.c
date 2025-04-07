#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int rank, numprocs, n = 0, i, count = 0, total_count = 0, done = 0;
    double x, y, z, pi, PI25DT = 3.141592653589793238462643;

    MPI_Init(&argc, &argv);                    //Inicia la colaboración entre procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);      //Id del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);  //Número total de procesos
    
    //Para asegurar aleatoriedad distinta por proceso
    srand(time(NULL) + rank * 1000);

    while (!done){
        if (rank == 0) {
            printf("Enter the number of points: (0 quits) \n");
            scanf("%d", &n);

            //Enviamos n a los demás procesos
            for (int dest = 1; dest < numprocs; dest++){
                MPI_Send(&n, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
        } 

        //Recibimos n desde el proceso 0
        else MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Si n es 0, terminamos
        if (n == 0){
            MPI_Finalize();
            done = 1;
        }


        //Repartimos el trabajo entre procesos
        for (i = rank; i < n; i += numprocs){
            x = ((double) rand()) / RAND_MAX;
            y = ((double) rand()) / RAND_MAX;
            z = sqrt(x * x + y * y);
            if (z <= 1.0) count++;
        }

        //Devolvemos la aproximación de pi y el error
        if (rank == 0){
            total_count = count;
            int temp;

            //Recibimos los conteos de los otros procesos
            for (int source = 1; source < numprocs; source++){
                MPI_Recv(&temp, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                total_count += temp;
            }

            pi = ((double) total_count / (double) n) * 4.0;
            printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        } 
    
        //Enviamos nuestro resultado a proceso 0
        else MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    //Finaliza la colaboración entre procesos
    MPI_Finalize();
    return 0;
}
