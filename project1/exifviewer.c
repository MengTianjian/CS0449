//
//  main.c
//  CS0449EXIFViewer
//
//  Created by Meng Tianjian on 9/20/16.
//  Copyright © 2016 Pitt. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char Manufacturer[100];
    char Model[100];
    unsigned int ExposureTime[2];
    unsigned int FStop[2];
    char Date[100];
    unsigned int FocalLength[2];
    int ISO;
    long Width, Length;
}Header;

typedef struct
{
    unsigned short TagIdentifier;
    unsigned short DataType;
    int NumberofDataItems;
    int Value;
    int Correct;
}TIFFtag;

TIFFtag ReadTagIdentifier(FILE *f);
int ReadHeader(FILE *f, Header *header, TIFFtag tag);
void Print(Header header);

int main(int argc, const char * argv[]) {
    FILE *f = fopen(argv[1], "r");
    unsigned short num;
    Header header;
    char e[3];
    fseek(f, 12, SEEK_SET);
    fread(e, 1, 2, f);
    e[2] = '\0';
    if (strcmp(e, "II") != 0)
    {
        fprintf(stderr, "Error while reading file marker.\n");
        return 1;
    }
    int i;
    for (i = 0; i < 9; i++)
    {
        int a = 0;
        long b = 0;
        while (a == 0)
        {
            fread(&num, 2, 1, f);
            switch (num)
            {
                case 0x010F:
                case 0x0110:
                case 0xA002:
                case 0xA003:
                case 0x8827:
                case 0x829a:
                case 0x829d:
                case 0x920A:
                case 0x9003:
                    b = ftell(f);
                    if (ReadHeader(f, &header, ReadTagIdentifier(f)) == 0)
                    {
                        fprintf(stderr, "Error while reading file marker.\n");
                        return 1;
                    }
                    a++;
                    break;
                default:
                    break;
            }
        }
        fseek(f, b+10, SEEK_SET);
    }
    Print(header);
    return 0;
}

TIFFtag ReadTagIdentifier(FILE *f)
{
    TIFFtag tag;
    fseek(f, -2, SEEK_CUR);
    fread(&tag.TagIdentifier, 2, 1, f);
    fread(&tag.DataType, 2, 1, f);
    fread(&tag.NumberofDataItems, 4, 1, f);
    fread(&tag.Value, 4, 1, f);
    if (tag.DataType != 2 && tag.DataType != 3 && tag.DataType != 4 && tag.DataType != 5)
        tag.Correct = 0;
    else
        tag.Correct = 1;
    return tag;
}

int ReadHeader(FILE *f, Header *header, TIFFtag tag)
{
    if (tag.Correct == 0)
        return 0;
    if (tag.DataType == 4 || tag.DataType == 3)
    {
        switch (tag.TagIdentifier)
        {
            case 0xA002:
                header->Width = tag.Value;
                break;
            case 0xA003:
                header->Length = tag.Value;
                break;
            case 0x8827:
                header->ISO = tag.Value;
                break;
            default:
                break;
        }
    }
    else
    {
        fseek(f, tag.Value + 12, SEEK_SET);
        if (tag.DataType == 2)
        {
            char str[100];
            fread(str, 1, tag.NumberofDataItems, f);
            str[tag.NumberofDataItems] = '\0';
            switch (tag.TagIdentifier)
            {
                case 0x010F:
                    strcpy(header->Manufacturer, str);
                    break;
                case 0x0110:
                    strcpy(header->Model, str);
                    break;
                case 0x9003:
                    strcpy(header->Date, str);
                    break;
                default:
                    break;
            }
        }
        else if (tag.DataType == 5)
        {
            unsigned int a[2];
            fread(a, 4, 2, f);
            switch (tag.TagIdentifier)
            {
                case 0x829a:
                    header->ExposureTime[0] = a[0];
                    header->ExposureTime[1] = a[1];
                    break;
                case 0x829d:
                    header->FStop[0] = a[0];
                    header->FStop[1] = a[1];
                    break;
                case 0x920A:
                    header->FocalLength[0] = a[0];
                    header->FocalLength[1] = a[1];
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}

void Print(Header header)
{
    printf("Manufacturer:\t%s\n", header.Manufacturer);
    printf("Model:\t\t%s\n", header.Model);
    if (header.ExposureTime[0]%header.ExposureTime[1]==0)
        printf("Exposure Time:\t%d second\n", header.ExposureTime[0]/header.ExposureTime[1]);
    else
        printf("Exposure Time:\t%d/%d second\n", header.ExposureTime[0], header.ExposureTime[1]);
    printf("F-stop:\t\tf/%1.1f\n", (float)header.FStop[0]/(float)header.FStop[1]);
    printf("ISO:\t\tISO %d\n", header.ISO);
    printf("Date Taken:\t%s\n", header.Date);
    printf("Focal Length:\t%d mm\n", header.FocalLength[0]/header.FocalLength[1]);
    printf("Width:\t\t%ld pixels\n", header.Width);
    printf("Height:\t\t%ld pixels\n", header.Length);
}
