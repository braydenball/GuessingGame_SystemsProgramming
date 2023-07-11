//include files
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int guess[2];
static int dirs[2];
static int sgn[4];

//intializing mutexes
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condition_1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_3 = PTHREAD_COND_INITIALIZER;

//checks the thread
int checkThread(int val, const char *msg)
{
    if (val == 0) return 0;
    errno = val;
    perror(msg);
    exit(EXIT_FAILURE);
}

//if there is an error
int checkError(int val, const char *msg)
{
    if (val == -1){
        if (errno == EINTR) return val;
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int rngRand(int first, int last)
{
    int rng = (last - first) + 1;
    double perc = ((double) rand()) / (((double) RAND_MAX)+1);
    int offst = rng * perc;

    return first + offst;
}

void * player1(void * args)
{
    while (1) 
    {
        int max = 100;
        int min = 0;
        
        pthread_mutex_lock(&mutex_3);
        
        while (sgn[2] == 0)
        {
            pthread_cond_wait(&condition_3, &mutex_3);
        }
        
        pthread_mutex_unlock(&mutex_3);
        sgn[2] = 0;
        while (1) 
        {
            guess[0] = (min + max) / 2;
            pthread_mutex_lock(&mutex_1);
            
            while (sgn[0] == 0) 
            {
                pthread_cond_wait(&condition_1, &mutex_1);
            }
            pthread_mutex_unlock(&mutex_1);
            
            sgn[0] = 0;

            if (dirs[0] == 1) 
            { max = guess[0]; } 
            
            else if (dirs[0] == -1)
            { min = guess[0]; } 
            
            else
            { break; }
        }
    }
}

void * player2(void *args)
{
    srand(time(NULL));
    while (1) 
    {
        int min = 0;
        int max = 100;
        
        pthread_mutex_lock(&mutex_3);
        while (sgn[3] == 0)
        {
            pthread_cond_wait(&condition_3, &mutex_3);
        }
        pthread_mutex_unlock(&mutex_3);
        sgn[3] = 0;
        
        while (1) 
        {
            guess[1] = (rand() % (max - min + 1) + min);
            pthread_mutex_lock(&mutex_2);
            while (sgn[1] == 0) 
            {
                pthread_cond_wait(&condition_2, &mutex_2);
            }
            pthread_mutex_unlock(&mutex_2);
            sgn[1] = 0;
            if (dirs[1] == 1) { max = guess[1]; } 
            else if (dirs[1] == -1) { min = guess[1]; } 
            else { break; }
        }
    }

}

void * referee(void *args)
{
    srand(time(NULL));
    
    int player1 = 0;
    int player2 = 0;
    int goal;

    for (int i = 0; i < 10; i++) 
    {
        pthread_mutex_lock(&mutex_3);
        
        goal = rand() %100 +1;
        sgn[2] = 1;
        sgn[3] = 1;
        
        pthread_cond_broadcast(&condition_3);
        pthread_mutex_unlock(&mutex_3);

        printf("\nGame %d\n", i + 1);
        printf("------------------\n");
        printf("------------------\n");
        
        printf("Player 1 wins = %d\n", player1);
        printf("Player 2 wins = %d\n\n", player2);
        
        //forever loop
        while (1)
        {
            //wait 1 sec
            sleep(1);
            pthread_mutex_lock(&mutex_1);
            pthread_mutex_lock(&mutex_2);
            sgn[0] = 0;
            sgn[1] = 0;

            if (guess[0] > goal)
            {
                dirs[0] = 1;
            }
            else if (guess[0] < goal)
            {
                dirs[0] = -1;
            }

            if (guess[1] > goal)
            {
                dirs[1] = 1;
            } 
            else if (guess[1] < goal)
            {
                dirs[1] = -1;
            }

            if (guess[0] == goal && guess[1] == goal)
            {
                printf("Player1 Guessed... %d = %d\n", guess[0], goal);
                printf("Player2 Guessed... %d = %d\n", guess[1], goal);
                
                printf("\tBoth players are awarded a point, it's a tie\n");
                dirs[0] = dirs[1] = 0;
                
                player1++;if (guess[0] == goal && guess[1] == goal){
                printf("Player1 Guessed... %d = %d\n", guess[0], goal);
                printf("Player2 Guessed... %d = %d\n", guess[1], goal);
                printf("\tBoth players are awarded a point, it's a tie\n");
                
                dirs[0] = dirs[1] = 0;
                player1++;
                player2++;
                sgn[0] = 1;
                sgn[1] = 1;
                pthread_cond_broadcast(&condition_1);
                pthread_cond_broadcast(&condition_2);
                pthread_mutex_unlock(&mutex_1);
                pthread_mutex_unlock(&mutex_2);
                break;
              }
            
            else
            {
                if (dirs[0] == -1)
                {
                    printf("Player1 Guessed... %d < %d\n", guess[0], goal);
                } else if (dirs[0] == 1)
                {
                    printf("Player1 Guessed...  %d > %d\n", guess[0], goal);
                }
                if (dirs[1] == -1)
                {
                    printf("Player2 Guessed... %d < %d\n", guess[1], goal);
                } else if (dirs[1] == 1)
                {
                    printf("Player2 Guessed... %d > %d\n", guess[1], goal);
                }
            }
                player2++;
                sgn[0] = 1;
                sgn[1] = 1;
                pthread_cond_broadcast(&condition_1);
                pthread_cond_broadcast(&condition_2);
                pthread_mutex_unlock(&mutex_1);
                pthread_mutex_unlock(&mutex_2);
                break;
            }
            
            else if (guess[0] == goal)
            {
                printf("Player1 Guessed... %d = %d\n", guess[0], goal);
                printf("\n\tPlayer1 wins this game\n");
                if (dirs[1] == -1)
                {
                    printf("Player2 Guessed... %d < %d\n", guess[1], goal);
                } else if (dirs[1] == 1)
                {
                    printf("Player2 Guessed... %d > %d\n", guess[1], goal);
                }
                
                dirs[0] = dirs[1] = 0;
                player1++;
                sgn[0] = 1;
                sgn[1] = 1;
                
                pthread_cond_broadcast(&condition_1);
                pthread_cond_broadcast(&condition_2);
                pthread_mutex_unlock(&mutex_1);
                pthread_mutex_unlock(&mutex_2);
                break;
            }
            
            else if (guess[1] == goal)
            {
                if (dirs[0] == -1)
                {
                    printf("Player1 Guessed... %d < %d\n", guess[0], goal);
                } 
                else if (dirs[0] == 1)
                {
                    printf("Player1 Guessed... %d > %d\n", guess[0], goal);
                }

                printf("Player2 Guessed: %d = %d\n", guess[1], goal);
                printf("\n\tPlayer2 wins this game\n");

                dirs[0] = dirs[1] = 0;

                player2++;

                sgn[0] = 1;
                sgn[1] = 1;

                pthread_cond_broadcast(&condition_1);
                pthread_cond_broadcast(&condition_2);
                pthread_mutex_unlock(&mutex_1);
                pthread_mutex_unlock(&mutex_2);

                break;
            }
           
            else{
                if (dirs[0] == -1)
                {
                    printf("Player1 Guessed... %d < %d\n", guess[0], goal);
                }
                 else if (dirs[0] == 1)
                {
                    printf("Player1 Guessed... %d > %d\n", guess[0], goal);
                }
                if (dirs[1] == -1)
                {
                    printf("Player2 Guessed... %d < %d\n", guess[1], goal);
                } 
                else if (dirs[1] == 1)
                {
                    printf("Player2 Guessed... %d > %d\n", guess[1], goal);
                }
            }

            printf("\n");
            sgn[0] = 1;
            sgn[1] = 1;

            pthread_cond_broadcast(&condition_1);
            pthread_cond_broadcast(&condition_2);
            pthread_mutex_unlock(&mutex_1);
            pthread_mutex_unlock(&mutex_2);
        }
    }

    printf("\nThe ending scores for both players are:\n");
    printf("Player1: %d\n", player1);
    printf("Player2: %d\n", player2);

    return NULL;
}

int main (int argc, char *argv[])
{
    sgn[0] = 0;
    sgn[1] = 0;
    sgn[2] = 0;
    sgn[3] = 0;

    srand(time(NULL));
    
    pthread_t play1, play2, ref1;
    
    pthread_create(&ref1, NULL, referee, NULL);
    pthread_create(&play1, NULL, player1, NULL);
    pthread_create(&play2, NULL, player2, NULL);

    pthread_join(ref1, NULL);
    return 0;
}