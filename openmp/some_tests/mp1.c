
    // Начало параллельной части
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        master();
    }
    else
    {
        slave();
    }

    // Конец параллельной части
    MPI_Finalize();

