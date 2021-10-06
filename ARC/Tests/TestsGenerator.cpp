#include <cstdio>
#include <cstdlib>
#include <ctime>

const size_t BUF_SIZE = 256;
const size_t NUM_REQUESTS = 100000;

// first name = atoi (argv[1]).task, argv[2] - amount of tests
// argv[3] - sizeof(cache), argv[4] - number of pages
int main (int argc, char** argv) // ToDo: getopt
{
    srand (time (NULL));
    if (argc != 5)
    {
        printf ("Bad params.\n");
        return 0;
    }

    int first_test = atoi (argv[1]),
        num_tests  = atoi (argv[2]),
        cache_size = atoi (argv[3]),
        num_pages  = atoi (argv[4]); // <= 2147483647

    char  buf_in[BUF_SIZE] = "";
    char buf_out[BUF_SIZE] = "";

    if (first_test < 1 || num_tests < 0 || cache_size < 1 || num_pages < 1)
    {
        printf ("Bad params.\n");
        return 0;
    }

    for (int i_test = 0; i_test < num_tests; i_test++)
    {
        sprintf (buf_in, "Tasks/%d.task", i_test + first_test);
        FILE* file_test = fopen (buf_in, "w");
        if (!file_test)
        {
            printf ("Error: can't open file %s.\n", buf_in);
            return 0;
        }

        fprintf (file_test, "%d %zu\n", cache_size, NUM_REQUESTS);
        for (int i_req = 0; i_req < NUM_REQUESTS; i_req++)
            fprintf (file_test, "%d\n", rand() % num_pages);
        
        fclose (file_test);

        sprintf (buf_out, "./../a.out < %s > Answers/%d.answer", buf_in, i_test + first_test);
        system (buf_out);
    }

    return 0;
}
