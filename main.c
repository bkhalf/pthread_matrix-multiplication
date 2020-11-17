#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int **arrA;                   //global pointer
int **arrB;
int **arrR1;
int **arrR2;
int n1=0,m1=0,n2=0,m2=0;

struct arg {                 //struct to send in thread creat
   int n;
   int m;
};

void getDim(FILE * fp,int *i,int *j);
void readArray(FILE * fp,int f);
void pp();
void * calcRow(void*t);
void *calcElement(void *t);

int main(int argc, char **argv)
{

   // printf("%s *---* %s *---* %s *---* %s\n",argv[0],argv[1],argv[2],argv[3]);
    size_t l = 15;
    char *file1;
    char *file2;
    char *file3;
    file1 = (char *)malloc(l * sizeof(char));
    file2 = (char *)malloc(l * sizeof(char));
    file3 = (char *)malloc(l * sizeof(char));
  //  scanf("%[^\n| ]s",file1);
    if(argv[1]==NULL){
        file1="a.txt";
        file2="b.txt";
        file3="c.txt";
    }else{
        file1=argv[1];
        file2=argv[2];
        file3=argv[3];
    }

    FILE * fp;
    fp = fopen(file1, "r");                     //open file a
    if (fp == NULL){
        printf("Error in reading file A");
        exit(EXIT_FAILURE);
    }
    getDim(fp,&n1,&m1);                           //read array a's size
//************************************************************
    arrA=malloc(sizeof(arrA[0])*n1);               //because it is global array and the user will define it's size
    for (int i = 0; i < n1; i++) {
        arrA[i] = malloc(m1 * sizeof arrA[i][0]);
    }
//************************************************************
    readArray(fp,0);                               //read array a
    fclose(fp);                                   //close file a



    fp = fopen(file2, "r");                    //open file b
    if (fp == NULL){
        printf("Error in reading file B");
        exit(EXIT_FAILURE);
    }
    getDim(fp,&n2,&m2);                             //read array a's size
//********************************************************
    arrB=malloc(sizeof(arrB[0])*n2);                    //because it is global array and the user will define it's size
    for (int i = 0; i < n2; i++) {
        arrB[i] = malloc(m2 * sizeof arrB[i][0]);
    }
    readArray(fp,1);                    //read array b
    fclose(fp);                         //close file b


//********************Threads with one row*************************

    arrR1=malloc(sizeof(arrR1[0])*n1);              //because it is global array and the user will define it's size
    for (int i = 0; i < n1; i++) {
        arrR1[i] = malloc(m2 * sizeof arrR1[i][0]);
        for(int j=0;j<m2;j++){
            arrR1[i][j]=0;                         //intialize it with 0
        }
    }

//********************************************
    struct timeval stop1, start1;
    gettimeofday(&start1, NULL); //start checking time

    pthread_t threads [n1];               //threads array

    int rc;
    for(int i=0;i<n1;i++){
            rc=pthread_create(&threads[i],NULL,calcRow,(void *)i);            //passing row index
            if(rc){
                printf("ERROR: return code from pthread_create() is %d\n",rc);
                exit(-1);
            }
    }

    for(int i=0;i<n1;i++){                                //wait untill all threads finish
        pthread_join(threads[i],NULL);
    }

    gettimeofday(&stop1, NULL); //end checking time
    printf("thread with row\n");
    printf("Seconds taken %lu\n", stop1.tv_sec - start1.tv_sec);
    printf("Microseconds taken: %lu\n", stop1.tv_usec - start1.tv_usec);
    printf("number of threads = %d\n",n1);
    printf("\n-----------------------------------------\n\n");


  /*  if((fp=fopen("c.out", "w"))==NULL) {                    //open c.out file
        printf("Cannot open file.\n");
        exit(0);
    }
    if(fwrite(arrR1, sizeof(float), m1*n2, fp) != m1*n2)           //save the result matrix
        printf("File write error.");
    fclose(fp);
*/
//*******************************Thread with one element***************************************


    arrR2=malloc(sizeof(arrR2[0])*n1);            //because it is global array and the user will define it's size
    for (int i = 0; i < n1; i++) {
        arrR2[i] = malloc(m2 * sizeof arrR2[i][0]);
        for(int j=0;j<m2;j++){
            arrR2[i][j]=0;                        //intialize it with 0
        }
    }

//********************************************************

    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time

    int number_threads=n1*m2;                           //number of threads= number of array A rows * number of array B column
    pthread_t threads_2 [number_threads];                //threads array
    struct arg tdata[number_threads] ;                  //arrray of strcuct arg
    int v=0;                                            //index to thread array && asr array
    int rc_2;
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            tdata[v].n=i;
            tdata[v].m=j;
            rc_2=pthread_create(&threads_2[v++],NULL,calcElement,(void *) &tdata[v]);
            if(rc_2){
                printf("ERROR: return code from pthread_create() is %d\n",rc);
                exit(-1);
            }
           // free(tdata);
        }
    }
    for(int i=0;i<number_threads;i++){                      //wait untill all threads finish
        pthread_join(threads_2[i],NULL);
    }

    gettimeofday(&stop, NULL); //end checking time
    printf("thread with one element\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("number of threads = %d\n",number_threads);

     if((fp=fopen(file3, "w"))==NULL) {                    //open c.out file
        printf("Error: Cannot open file to save result.");
        exit(0);
    }
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            fprintf(fp, "%d", arrR2[i][j]);

            if(j+1==m2)fputc('\n',fp);
            else fputc('\t',fp);
        }
    }
    fclose(fp);


//*********************************************************************************************
   // pp();                                                            //print all arrays
    free(arrA);free(arrB);free(arrR1);free(arrR2);                   //free all arrays memory
    pthread_exit(NULL);                                              //exit the main thread
    return 0;
}
//***************************************************functions**********************************
void pp(){

    printf("_____________________array A_________________________\n");
    for(int i=0;i<n1;i++){
        for(int j=0;j<m1;j++){
            printf("%d\t",arrA[i][j]);
        }
        printf("\n");
    }
    printf("_____________________array B_________________________\n");
    for(int i=0;i<n2;i++){
        for(int j=0;j<m2;j++){
            printf("%d\t",arrB[i][j]);
        }
        printf("\n");
    }
    printf("_____________________result with row thread_________________________\n");
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            printf("%d\t",arrR1[i][j]);
        }
        printf("\n");
    }
    printf("_____________________result whit element thread_________________________\n");
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            printf("%d\t",arrR2[i][j]);
        }
        printf("\n");
    }

}
void readArray(FILE * fp,int f){                   //read array from file

    char * line = NULL;                             //buffer string
    size_t len = 1;
    ssize_t read;

    line = (char *)malloc(len * sizeof(char));        //allocate memory to line
    if( line == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    int i=0,j=0;
    while ((read = getline(&line, &len, fp)) != -1) {                    //read line by line till the end of file
        //printf("Retrieved line of length %zu:\n len=%d\n %s---%d\n", read,len,line,strlen(line));
        char m[10];int v=0;
        for(int k=0;k<read;k++){                               //size of line == read
            if(!isspace(line[k])){
                m[v++]=line[k];                                //if not a white space then add it to string m
            }else{
                if(f==0)                                       //flag to check if it is array a or b
                arrA[i][j++]=atoi(m);                          //convert m to int and store in in array a
                else arrB[i][j++]=atoi(m);                     //convert m to int and store in in array a
                v=0;                                           //reset the counter v
                memset(m,0,sizeof(m));                         //reset m
            }
        }
        i++;
        j=0;
    }
  //  if (line)
        free(line);                                           //free line
}

void getDim(FILE * fp,int *i,int *j){               //read array size
      char * line ;                                 //buffer string
    size_t len = 15;
    ssize_t read;                                          //to get the size in getline function
    line = (char *)malloc(len * sizeof(char));             //allocate memory
    if((read = getline(&line, &len, fp)) != -1){          //read the first line in the file
        char m[10];                                       //string with len 10
        int r=0;
        int k=4;                                          //the digits starts in index 4
        while(!isspace(line[k]))                          //if not a white space add it to string m
            m[r++]=line[k++];
        m[r]='\0';                                        //close the string
       *i=atoi(m);                                        //convert it to int
        char n[10];                                      //repeat to get column size
       k+=5;r=0;
       while(!isspace(line[k]))
            n[r++]=line[k++];
        n[r]='\0';
        *j=atoi(n);
    }
        free(line);                                    //free the line memory
}

void * calcRow(void*t){                         //calculate the entire row
    int c=t;
    for(int i=0;i<m2;i++){
        for(int j=0;j<m1;j++){
            arrR1[c][i]+=arrA[c][j]*arrB[j][i];
        }
    }
    pthread_exit(NULL);                       //thread exit
}


void *calcElement(void *t){
    struct arg *data;                        //calculate one element
    data=(struct arg*)t;
    int l=data->n;                           //retrieve data from struct
    int r=data->m;
    for(int k=0;k<m1;k++){
        arrR2[l][r]+=arrA[l][k]*arrB[k][r];
    }
    pthread_exit(NULL);                      //thread exit
}
