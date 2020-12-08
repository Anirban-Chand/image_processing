#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ll long long
#define MAF printf("Sorry, Memory Allocation Failed. Cannot Process Further.\n");
#define FAF printf("Cannot process The File.\n");
#define FUP printf("Files Are Under Processing...\n");

// CREATED A LINKED LIST TO STORE THE EQUIVALENT LABELS
typedef struct equiv_node
{
    int label1;
    int label2;
    struct equiv_node *next;
} equiv_node;

equiv_node *equiv_head = NULL;

equiv_node *createEquiv_node(int val1, int val2)
{
    equiv_node *temp = (equiv_node *)malloc(sizeof(equiv_node));
    temp->label1 = val1;
    temp->label2 = val2;
    temp->next = NULL;
    return temp;
}

void equiv_Append(int val1, int val2)
{
    equiv_node *temp = createEquiv_node(val1, val2);

    equiv_node *iter = equiv_head;

    // EMPTY LIST CASE
    if (equiv_head == NULL)
    {
        equiv_head = temp;
    }

    // NON-EMPTY LIST CASE
    else
    {
        while (iter->next != NULL)
        {
            iter = iter->next;
        }
        iter->next = temp;
    }
}

// LINKED LIST TO STORE THE LABEL VALUES
typedef struct labelNode
{
    int label;
    int topx, topy;
    int rightx, righty;
    int bottomx, bottomy;
    int leftx, lefty;
    struct labelNode *next;
} labelNode;

labelNode *labelHead = NULL;

labelNode *createLabelNode(int val)
{
    labelNode *temp = (labelNode *)malloc(sizeof(labelNode));
    temp->label = val;
    temp->next = NULL;
    return temp;
}
void check(int);

void labelAppend(int val)
{
    labelNode *temp = createLabelNode(val);

    labelNode *t = labelHead;

    // EMPTY LIST CASE
    if (labelHead == NULL)
    {
        labelHead = temp;
    }

    // NON-EMPTY LIST CASE
    else
    {
        while (t->next != NULL)
        {
            t = t->next;
        }
        t->next = temp;
    }
}

void removeDuplicates()
{
    labelNode *ptr1 = labelHead, *ptr2, *dup;

    while (ptr1 != NULL && ptr1->next != NULL)
    {
        ptr2 = ptr1;
        while (ptr2->next != NULL)
        {
            if (ptr1->label == ptr2->next->label)
            {
                dup = ptr2->next;
                ptr2->next = ptr2->next->next;
                free(dup);
            }
            else
            {
                ptr2 = ptr2->next;
            }
        }
        ptr1 = ptr1->next;
    }
}

int background = 0;
int foreground = 1;

int thresold(int **image, int height, int width)
{

    // ANCHOR =======================NOTES=======================
    // ANCHOR inter-class variance  = variance between 2 classes
    // ANCHOR intra-class variance  = variance within 1 class

    /* -------------------------------------------------------------------------- */
    /*                          HISTOGRAM FOR THE IMAGE                           */
    /* -------------------------------------------------------------------------- */

    int hist_count[256] = {0};
    float pixel_probability[256] = {0};

    /* ------------------- COUNT THE NUMBER OF THE EACH PIXEL ------------------- */
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            hist_count[image[i][j]]++;
        }
    }

    ll total_pixel = height * width;

    /* ---------------------- PROBABILITY OF EACH PIXEL ---------------------- */
    for (int i = 0; i < 256; i++)
    {
        pixel_probability[i] = ((float)hist_count[i]) / total_pixel;
    }

    // Inter-Class Variance
    float interClassVariance = 0;
    int threshold_value;

    // calculate class probabilities for each value of pixel 0 to t=0,1,2....255
    for (int t = 1; t <= 256; t++)
    {
        // Class Probabilities
        float w1 = 0; // Probability for class 1
        float w2 = 0; // Probability for class 2

        // Class Means
        float u1 = 0; // Mean for class 1
        float u2 = 0; // Mean for class 2

        // calculate class probabilities
        for (int i = 0; i < t; i++)
        {
            w1 += pixel_probability[i];
        }
        for (int j = t; j <= 256; j++)
        {
            w2 += pixel_probability[j];
        }

        // calculate class means
        for (int i = 0; i < t; i++)
        {
            u1 += (i * pixel_probability[i]);
        }
        u1 = u1 / w1; // updated class1-mean

        for (int j = t; j <= 256; j++)
        {
            u2 += (j * pixel_probability[j]);
        }
        u2 = u2 / w2; // updated class2-mean

        float diff = abs(u1 - u2);
        float temp_variance = w1 * w2 * pow(diff, 2);
        if (temp_variance > interClassVariance)
        {
            interClassVariance = temp_variance;
            threshold_value = t;
        }
    }

    return threshold_value;
}

int **binarization(int **image, int **binimg, int thresoldVal, int height, int width, FILE *bin)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] < thresoldVal)
            {
                binimg[i][j] = background;
                // fprintf(bin, "%d ", binimg[i][j]);
            }
            else
            {
                binimg[i][j] = foreground;
                // fprintf(bin, "%d ", binimg[i][j]);
            }
        }
    }
    // printf("Binarization Complete Successfully\n");
    return binimg;
}

int **componentLabelling(int **binimg, int **image, int height, int width)
{
    int label = 0;

    /* ---------------------------- USING RASTER SCAN --------------------------- */

    for (int i = 1; i < height; i++)
    {
        for (int j = 1; j < width; j++)
        {
            if ((binimg[i][j] == foreground) && ((binimg[i - 1][j] == background) && (binimg[i][j - 1] == background)))
            {
                label++;
                image[i][j] = label;
                // printf("Condition 1\n");
            }

            else if ((binimg[i - 1][j] == foreground) && (binimg[i][j - 1] == foreground) && (binimg[i][j] == foreground))
            {
                // printf("Condition 2\n");
                if (image[i - 1][j] == image[i][j - 1])
                    image[i][j] = image[i - 1][j];
                else if (image[i - 1][j] != image[i][j - 1])
                {
                    image[i][j] = __min(image[i - 1][j], image[i][j - 1]);
                    equiv_Append(image[i][j], __max(image[i - 1][j], image[i][j - 1]));
                    // printf("%d %d\n", image[i][j], __max(image[i - 1][j], image[i][j - 1]));
                }
            }

            else if ((binimg[i][j] == foreground) && ((binimg[i - 1][j] == foreground) || (binimg[i][j - 1] == foreground)))
            {
                if (binimg[i - 1][j] == foreground)
                    image[i][j] = image[i - 1][j];
                else if (binimg[i][j - 1] == foreground)
                    image[i][j] = image[i][j - 1];
                // printf("Condition 3\n");
            }
        }
    }

    equiv_node *t = equiv_head;
    equiv_node *it = equiv_head;
    while (t != NULL)
    {
        while (it != NULL)
        {
            if (t->label2 == it->label2)
                equiv_Append(t->label1, it->label1);
            it = it->next;
        }
        t = t->next;
    }

    // equiv_node *s = equiv_head;
    // while (s != NULL)
    // {
    //     printf("%d %d\n", s->label1, s->label2);
    //     s = s->next;
    // }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            equiv_node *dummy = equiv_head;
            while (dummy != NULL)
            {
                if (image[i][j] == dummy->label2)
                {
                    image[i][j] = dummy->label1;
                }
                dummy = dummy->next;
            }
        }
    }
    printf("Component Labelling Done.\n");
    return image;
}

void leftmost(int **image, int height, int width, labelNode *p)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] == p->label)
            {
                p->leftx = j+1;    // Width = X-Axis
                p->lefty = i+1;    // Height = Y-Axis
                break;
            }
        }
    }
    printf("Left Most Point -->  ");
    printf("X = %d\tY = %d\n", p->leftx, p->lefty);
}

void bottommost(int **image, int height, int width, labelNode *p)
{
    for (int j = 0; j < width; j++)
    {
        for (int i = height-1; i >= 0; i--)
        {
            if (image[j][i] == p->label)
            {
                p->bottomx = i+1;      // Width = X-Axis
                p->bottomy = j+1;      // Height = Y-Axis 
                break;
            }
        }
    }
    printf("Bottom Most Point -->  ");
    printf("X = %d\tY = %d\n", p->bottomx, p->bottomy);
}

void rightmost(int **image, int height, int width, labelNode *p)
{
    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = width - 1; j >= 0; j--)
        {
            if (image[i][j] == p->label)
            {
                p->rightx = j+1;     // Width = X-Axis
                p->righty = i+1;     // Height = Y-Axis
                break;
            }
        }
    }
    printf("Right Most Point -->  ");
    printf("X = %d\tY = %d\n", p->rightx, p->righty);
}

void topmost(int **image, int height, int width, labelNode *p)
{
    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] == p->label)
            {
                p->topx = j+1;       // Width = X-Axis
                p->topy = i+1;       // Height = Y-Axis
                break;
            }
        }
    }
    printf("Top Most Point -->  ");
    printf("X = %d\tY = %d\n", p->topx, p->topy);
}

void boxComponent(int **image, int height, int width, int objcount)
{
    labelNode* ln = labelHead;
    int obj = 1;
    while (ln != NULL && obj <= objcount)
    {
        printf("\nOBJECT %d with Label %d -->\n", obj, ln->label);
        topmost(image, height, width, ln);
        rightmost(image, height, width, ln);
        leftmost(image, height, width, ln);
        bottommost(image, height, width, ln);
        ln = ln->next;
        obj++;
    }
}

/* ------------------------------- DRIVER CODE ------------------------------ */
int main(int argc, char *argv[])
{

    /* 
    in = input file
    bin = binarized file 
   */
    FILE *in = NULL, *bin = NULL;

    /* Height, Width and Maximum Pixel Value of the Image */
    int height, width, maxpix;

    /* Magic No of the Image and Str is for storing the strings read */
    char c, magicno[2], str[500];

    /* Opening Input File in Read Mode */
    in = fopen(argv[1], "r");

    /* Opening file in write-binary('wb') mode for binarization */
    bin = fopen(argv[2], "wb");

    /* - Check whether the file pointers have been opened required files or not - */
    if ((in != NULL) && (bin != NULL))
    {
        FUP; // FUP has been defined
    }
    else
    {
        FAF; // FAF has been defined
    }

    /* Reads and Writes the Magic No of the Image */
    fgets(magicno, 3, in); // reads from input file
    fscanf(in, "\n");      // reads the newline character
    fputs("P1", bin);      // 'P1' is the Magic No for a Binary Image
    fputs("\n", bin);      // writes the newline character

    /* Reads and Writes the Width and Height of the Image */
    fscanf(in, "%d %d", &width, &height); // reads
    fscanf(in, "\n");                     // reads the newline character
    printf("Image Width = %d\nImage Height = %d\n", width, height);
    fprintf(bin, "%d %d", width, height); // writes
    fprintf(bin, "\n");                   // writes the newline character

    /* ---------------- Reads and Writes the Maximum Pixel Value ---------------- */
    fscanf(in, "%d", &maxpix);
    // fprintf(out, "%d", maxpix);
    fprintf(bin, "1", maxpix);
    fscanf(in, "\n"); // reads the newline character
    // fprintf(out,"\n");                          // writes the newline character
    fprintf(bin, "\n"); // writes the newline character

    // printf("Image Details Read & Written Successfully. :)\n");

    /* 2-D Image Array to store Pixel Values */
    /* 'image' is the array of pointers,
        each of its part stores the base address 
        of the nested arrays that's why we should use double star(int **image) */
    int **image = NULL;
    int **binimg;

    image = (int **)malloc(height * sizeof(int *));
    binimg = (int **)malloc(height * sizeof(int *));
    if (image == NULL)
        MAF;
    if (binimg == NULL)
        MAF;

    /* ------------------ Reads and Writes Every Pixel's Value ------------------ */
    for (int i = 0; i < height; i++)
    {

        image[i] = (int *)malloc(width * sizeof(int));
        binimg[i] = (int *)malloc(width * sizeof(int));

        for (int j = 0; j < width; j++)
        {
            int x;
            fscanf(in, "%d", &x);
            fscanf(in, "\n");
            image[i][j] = x;
            if (feof(in))
                break;
        }
    }

    int thresoldVal = thresold(image, height, width);

    printf("Auto-generated Threshold Value for the Image = %d\n", thresoldVal);

    binimg = binarization(image, binimg, thresoldVal, height, width, bin);

    image = componentLabelling(binimg, image, height, width);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fprintf(bin, "%d ", image[i][j]);
            // printf("%d ", image[i][j]);
        }
        fprintf(bin, "\n");
        // printf("\n");
    }

    int lab = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] != lab && image[i][j] != 0)
            {
                lab = image[i][j];
                labelAppend(lab);
            }
        }
    }

    removeDuplicates();

    printf("Labels are - ");

    labelNode *ln = labelHead;
    int objcount = 0;
    while (ln != NULL)
    {
        printf("%d ", ln->label);
        objcount++;
        ln = ln->next;
    }

    printf("\nNumber(s) Of Objects- %d\n", objcount);

    boxComponent(image, height, width, objcount);

    fclose(in);
    fclose(bin);
    free(binimg);
    free(image);
}