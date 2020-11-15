#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int **arrA;
int **arrB;
int **arrR1;
int **arrR2;
int n1=0,m1=0,n2=0,m2=0;

struct arg {
   int n;
   int m;
};

void getDim(FILE * fp,int *i,int *j);
void readArray(FILE * fp,int f);
void pp();

void * calcRow(void*t){
    int c=t;
    for(int i=0;i<m2;i++){
        for(int j=0;j<m1;j++){
            arrR1[c][i]+=arrA[c][j]*arrB[j][i];
        }
    }
    pthread_exit(NULL);
}

void *calcElement(void *t){
 //   printf("ippppppppppppppp\n");
    struct arg *data;
    data=(struct arg*)t;
    int l=data->n;
    int r=data->m;
    printf("%d-----------%d\n",l,r);
    for(int k=0;k<m1;k++){
        arrR2[l][r]+=arrA[l][k]*arrB[k][r];
    }
    pthread_exit(NULL);
}

int main()
{

    FILE * fp;
    fp = fopen("a.txt", "r");                     //open file a
    if (fp == NULL){
        printf("Error in reading file");
        exit(EXIT_FAILURE);
    }
    getDim(fp,&n1,&m1);
//************************************************************
    arrA=malloc(sizeof(arrA[0])*n1);
    for (int i = 0; i < n1; i++) {
        arrA[i] = malloc(m1 * sizeof arrA[i][0]);
    }
//************************************************************
    readArray(fp,0);
    fclose(fp);                                   //close file a



    fp = fopen("b.txt", "r");                    //open file b
    if (fp == NULL){
        printf("Error in reading file");
        exit(EXIT_FAILURE);
    }
    getDim(fp,&n2,&m2);
//********************************************************
    arrB=malloc(sizeof(arrB[0])*n2);
    for (int i = 0; i < n2; i++) {
        arrB[i] = malloc(m2 * sizeof arrB[i][0]);
    }
    //*****************
    arrR1=malloc(sizeof(arrR1[0])*n1);
    for (int i = 0; i < n1; i++) {
        arrR1[i] = malloc(m2 * sizeof arrR1[i][0]);
    }
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            arrR1[i][j]=0;
        }
    }
    //***********************
    arrR2=malloc(sizeof(arrR2[0])*n1);
    for (int i = 0; i < n1; i++) {
        arrR2[i] = malloc(m2 * sizeof arrR2[i][0]);
    }
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            arrR2[i][j]=0;
        }
    }
//********************************************************
    readArray(fp,1);
    fclose(fp);                         //close file b


//********************Threads with one row*************************

    pthread_t threads [n1];

    int rc;
    for(int i=0;i<n1;i++){
            rc=pthread_create(&threads[i],NULL,calcRow,(void *)i);
            if(rc){
                printf("ERROR: return codr from pthread_create() is %d\n",rc);
                exit(-1);
            }
    }

    for(int i=0;i<n1;i++){
        pthread_join(threads[i],NULL);
    }

//*******************************Thread with one element***************************************
    int number_threads=n1*m2;
    pthread_t threads_2 [number_threads];
    struct arg tdata[number_threads] ;
    int v=0;
    int rc_2;
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){

            tdata[v].n=i;
            tdata[v].m=j;
            rc_2=pthread_create(&threads_2[v++],NULL,calcElement,(void *) &tdata[v]);
            if(rc_2){
                printf("ERROR: return codr from pthread_create() is %d\n",rc);
                exit(-1);
            }
           // free(tdata);
        }
    }
    for(int i=0;i<number_threads;i++){
        pthread_join(threads_2[i],NULL);
    }

//*********************************************************************************************
    pp();
    free(arrA);free(arrB);free(arrR1);free(arrR2);
    pthread_exit(NULL);
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
    printf("_____________________array result1_________________________\n");
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            printf("%d\t",arrR1[i][j]);
        }
        printf("\n");
    }
    printf("_____________________array result2_________________________\n");
    for(int i=0;i<n1;i++){
        for(int j=0;j<m2;j++){
            printf("%d\t",arrR2[i][j]);
        }
        printf("\n");
    }

}
void readArray(FILE * fp,int f){

    char * line = NULL;
    size_t len = 1;
    ssize_t read;

    line = (char *)malloc(len * sizeof(char));
    if( line == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }

    int i=0,j=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n len=%d\n %s---%d\n", read,len,line,strlen(line));
        char m[5];int v=0;
        for(int k=0;k<read;k++){
            if(!isspace(line[k])){
                m[v++]=line[k];
            }else{
                if(f==0)
                arrA[i][j++]=atoi(m);
                else arrB[i][j++]=atoi(m);
                v=0;
                memset(m,0,sizeof(m));
            }
        }
        i++;
        j=0;
    }
  //  if (line)
        free(line);
}

void getDim(FILE * fp,int *i,int *j){
      char * line ;
    size_t len = 15;
    ssize_t read;
    line = (char *)malloc(len * sizeof(char));
    if((read = getline(&line, &len, fp)) != -1){
        char m[5];
        int r=0;
        int k=4;
        while(!isspace(line[k]))
            m[r++]=line[k++];
        m[r]='\0';
       *i=atoi(m);
        char n[5];
       k+=5;r=0;
       while(!isspace(line[k]))
            n[r++]=line[k++];
        n[r]='\0';
        *j=atoi(n);
    }
        free(line);
}

