#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


/* Driver Code */
int main(int argc, char* argv[]){
    
    /* in = input file
       out = output file */
    FILE *in=NULL, *out=NULL, *bin=NULL;

    /* Height, Width and Maximum Pixel Value of the Image */
    int height,width,maxpix;

    /* Magic No of the Image and Str is for storing the strings read */
    char c,magicno[2],str[500];

    /* Total 3 files are gonna open for 3 operations */
    /* Opening Input File in Read Mode */
    in=fopen(argv[1],"r");

    /* Opening Output File in Write Mode */
    out=fopen(argv[2],"w");
    
    /* Opening file for filtration */
    bin=fopen(argv[3],"w+");
    if((in!=NULL)&&(out!=NULL)&&(bin!=NULL)){
        printf("Files Are Under Processing...\n");
    }
    else{
        printf("Cannot Process The Files!\n");
    }

    /* Reads and Writes the Magic No of the Image */
    fgets(magicno, 3, in);                      // reads from input file
    fscanf(in,"\n");                            // reads the newline character
    printf("Magic No - %s\n",magicno);
    fputs(magicno,out);                      // writes in output file
    fputs("\n",out);                         // writes the newline character
    fputs(magicno,bin);                         // writes in output file
    fputs("\n",bin);                            // writes the newline character
    

    // fgets(str, 50, in); // reads the comment out line
    

    /* Reads and Writes the Width and Height of the Image */
    fscanf(in, "%d %d", &width, &height);       // reads
    fscanf(in,"\n");                            // reads the newline character
    // printf("Width = %d\tHeight = %d\n", width,height);
    fprintf(out, "%d %d", width,height);     // writes
    fprintf(out,"\n");                       // writes the newline character
    fprintf(bin, "%d %d", width,height);        // writes
    fprintf(bin,"\n");                          // writes the newline character
    

/* ---------------- Reads and Writes the Maximum Pixel Value ---------------- */
    fscanf(in, "%d", &maxpix);
    fprintf(out, "%d", maxpix);
    fprintf(bin, "%d", maxpix);
    fscanf(in,"\n");                            // reads the newline character
    fprintf(out,"\n");                       // writes the newline character
    fprintf(bin,"\n");                          // writes the newline character


    printf("Image Details Read Successfully. :)\n");
    printf("Image Details Written Successfully. :)\n");


    /* 2-D Image Array to store Pixel Values */
    /* 'image' is the array of pointers,
        each of its part stores the base address 
        of the nested arrays that's why we should use double star(int **image) */
    int **image;
    int **cpyimg;
    
    image = (int**)malloc(height * sizeof(int*));
    cpyimg = (int**)malloc(height * sizeof(int*));
    if(image == NULL) printf("SORRY! Memory Allocation Failure!\n");
    if(cpyimg == NULL) printf("Memory Allocation Failed for Filtering!\n");
    else printf("Memory for the Image File Allocated.\n");
    // window = (int*)malloc(9*sizeof(int));

    
/* ------------------ Reads and Writes Every Pixel's Value ------------------ */
    for(int i=0; i<height; i++){
        
        image[i] = (int*)malloc(width * sizeof(int));
        cpyimg[i] = (int*)malloc(width * sizeof(int));

        for(int j=0; j<width; j++){ 
            int x;
            fscanf(in, "%d", &x);
            fscanf(in, "\n");
            image[i][j]=x;
            fprintf(out, "%d ", image[i][j]);
            if(feof(in)) break;
        }
        fprintf(out, "\n");
    }

/*=====================================================================================================*/
/*=====================================================================================================*/

// ANCHOR =======================NOTES=======================
// ANCHOR inter-class variance  = variance between 2 classes
// ANCHOR intra-class variance  = variance within 1 class

/* -------------------------------------------------------------------------- */
/*                          HISTOGRAM FOR THE IMAGE                           */
/* -------------------------------------------------------------------------- */

    int hist_count[256] = {0};
    float pixel_probability[256] = {0};
    
/* ------------------- COUNT THE NUMBER OF THE EACH PIXEL ------------------- */
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            hist_count[image[i][j]]++;
        }
    }

    int total_pixel = height * width;

/* ---------------------- PROBABILITY OF EACH PIXEL ---------------------- */
    for(int i=0; i<256; i++){
        pixel_probability[i] = ((float)hist_count[i])/total_pixel;
    }

    

    // Inter-Class Variance
    float interClassVariance=0;
    int threshold_value;
    
    // calculate class probabilities for each value of pixel 0 to t=0,1,2....255
    for(int t=1; t<=256; t++){
        // Class Probabilities
        float w1=0;     // Probability for class 1
        float w2=0;     // Probability for class 2
        
        // Class Means
        float u1=0;     // Mean for class 1 
        float u2=0;     // Mean for class 2
        
        // calculate class probabilities
        for(int i=0; i<t; i++){
            w1+=pixel_probability[i];
        }
        for(int j=t; j<=256; j++){
            w2+=pixel_probability[j];
        }

        // calculate class means
        for(int i=0; i<t; i++){
            u1+=(i*pixel_probability[i]);
        }
        u1=u1/w1;
        for(int j=t; j<=256; j++){
            u2+=(j*pixel_probability[j]);
        }
        u2=u2/w2;
        float diff = abs(u1-u2);
        float temp_variance = w1*w2*pow(diff, 2);
        if(temp_variance > interClassVariance) {
            interClassVariance = temp_variance;
            threshold_value = t;
        }
    }

    printf("Your auto-generated Threshold Value is = %d\n", threshold_value);

    // lower = total number of pixel values below the threshold value
    // greater_equal = total number of pixel values above/equal to the threshold value
    /*
    int lower=0, greater_equal=0;
    for(int h=0; h<threshold_value; h++){
        lower+=hist_count[h];
    }
    printf("No. of pixels below threshold = %d\n",lower);

    for(int h=threshold_value; h<256; h++){
        greater_equal+=hist_count[h];
    }
    printf("No. of pixels greater/equal to threshold = %d\n",greater_equal);
    */

    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            fscanf(out, "%d", &image[i][j]);
            fscanf(out, " ");
            if(image[i][j]<threshold_value){
                cpyimg[i][j]=0;
                fprintf(bin, "%d ", cpyimg[i][j]);
            }
            else{
                cpyimg[i][j]=255;
                fprintf(bin, "%d ", cpyimg[i][j]);
            }
        }
    }

    printf("*Image Data Copied Successfully. :)\n");
    fclose(in);
    fclose(out);
    fclose(bin);
    printf("Files have been closed\n");
    // free(image);
    // free(cpyimg);
    // printf("Image Memory has been cleared\n");
}