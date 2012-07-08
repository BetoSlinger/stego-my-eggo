// main.cpp
//
// this program reads a wave file and displays the header info
//

#include <windows.h>
#include <stdio.h>
#include "wave.h"
#include <vector>
#include <iostream>
#include "bitWiseChar.h"
#include <stdlib.h>
#include <time.h>
#include <fstream>
using namespace std;






WAVEFORMATEX *pHeader;
int parseMessage(BYTE *);

/*  parseMessage()
 *  argument: null terminated char *
 *  returns: vector of Bitwise Characters
 */

/* vector<BitWiseChar> parseMessage(char * m){
    vector<BitWiseChar> v;
    BitWiseChar t;
    while(*m != '\0'){
        t.setChar(*m);
        v.insert(v.end(),t );
        m++;
    }
    return v;
        
   
}   */
int twoBytes2Long(BYTE low , BYTE high){
    return (long long)(short)(high << 8) + low;
}


int readChunkHeader(FILE *fptr, W_CHUNK *pChunk)
{
	int x;

	// size = 1, count = 8 bytes
	x = (int) fread(pChunk, 1, 8, fptr);
	if(x != 8) return(FAILURE);

	return(SUCCESS);
} // readChunkHeader

// reads in the data portion of a chunk
BYTE *readChunkData(FILE *fptr, int size)
{
	BYTE *ptr;
	int tmp, x;

	tmp = size%2;	// size MUST be WORD aligned
	if(tmp != 0) size = size + 1;

	ptr = (BYTE *) malloc(size);
	if(ptr == NULL)
	{
		printf("\n\nError, could not allocate %d bytes of memory!\n\n", size);
		return(NULL);
	}

	x = (int) fread(ptr, 1, size, fptr);
	if(x != size)
	{
		printf("\n\nError reading chunkd data!\n\n");
		return(NULL);
	}

	return(ptr);	
} // readChunkData

// prints out wave format info
void printFormat(W_FORMAT fmt)
{
	printf("\n\nWAVE FORMAT INFO\n\n");
	printf("Compression Code:		%d\n", fmt.compCode);
	printf("Number of Channels:		%d\n", fmt.numChannels);
	printf("Sample Rate: 			%d\n", fmt.sampleRate);
	printf("Average Bytes/Second:		%d\n", fmt.avgBytesPerSec);
	printf("Block Align: 			%d\n", fmt.blockAlign);
	printf("Bits per Sample: 		%d\n", fmt.bitsPerSample);
	return;
} // printFormat

int main(int argc, char *argv[])
{
	FILE *fptr;
	DWORD fileSize;
	int x, cnt, dataFlag, formatFlag, noSampleFrames;
	W_CHUNK chunk[MAX_CHUNKS];		// assuming max of 8 chunks, should only be 3 for you
	BYTE *pChunkData[MAX_CHUNKS];
	W_FORMAT format;		// only 1 format chunk

	DWORD dataChunkSize;

	/*
    if(argc != 2)
	{
		printf("\n\nUSAGE:  wavereader filename.wav\n\n");
		system("pause");exit(-1);
	}
    */
	fptr = fopen("atmospace_jungle.wav", "rb");
	if(fptr == NULL)
	{
		printf("Could not file file named '%s.'\n\n", argv[1]);
		system("pause");exit(-1);
	}

	printf("Successfully opened file %s\n", argv[1]);

	// pChunk[0] is the chunk representing the file header
	x = readChunkHeader(fptr, &chunk[0]);
	if(x == FAILURE)
    {
          printf("read failed\n\n");
          system("pause");exit(-1);
    }
	// check to make sure it is a RIFF file
	if(memcmp( &(chunk[0].chunkID), "RIFF", 4) != 0)
	{
		printf("\n\nError, file is NOT a RIFF file!\n\n");
		system("pause");exit(-1);
	}
	fileSize = chunk[0].chunkSize + 8;

	// check to make sure it is a wave file
	pChunkData[0] = readChunkData(fptr, 4);

	if(memcmp( pChunkData[0], "WAVE", 4) != 0)
	{
		printf("\n\nError, file is not a WAVE file!\n\n");
		system("pause");exit(-1);
	}

	// chunk[1] should be format chunk, but if not, skip
	cnt = 1;
	dataFlag = -1;
	formatFlag = -1;

	while(cnt < MAX_CHUNKS)
	{
		x = readChunkHeader(fptr, &chunk[cnt]);
		if(x == FAILURE)
        {
              system("pause");
              exit(-1);
        }
		// read in chunk data
		pChunkData[cnt] = readChunkData(fptr, chunk[cnt].chunkSize);
		if(pChunkData[cnt] == NULL) 
        {
              system("pause");
              exit(-1);
        }      

		if(memcmp( &(chunk[cnt].chunkID), "data", 4) == 0)
			dataFlag = cnt;	// if find data chunk, take note

		if(memcmp( &(chunk[cnt].chunkID), "fmt ", 4) == 0)
		{
			formatFlag = cnt;	//	marks which chunk has format data
			break;	// found format chunk
		}

		cnt++;
	}

	if(cnt == MAX_CHUNKS)
	{
		printf("\n\nError, format chunk not found after 8 tries!\n\n");
		system("pause");exit(-1);
	}

	// check format size to make sure this is not a fancy WAVE file
	if(chunk[cnt].chunkSize != 16)
	{
		printf("\n\nError, this WAVE file is not a standard format - we will not use this one!\n\n");
		system("pause");exit(-1);
	}

	// put format chunk in our special variable
	// format chunk data already contained in pChunkData[cnt]
    memcpy(&format, pChunkData[cnt], 16);

	// make sure we are working with uncompressed PCM data
	if(format.compCode != 1)
	{
		printf("\n\nError, this file does not contain uncompressed PCM data!\n\n");
		system("pause");exit(-1);
	}

	printFormat(format);
	
	if(dataFlag == -1)	// have not found data chunk yet
	{
		while(cnt < MAX_CHUNKS)
		{
			x = readChunkHeader(fptr, &chunk[cnt]);
			
			if(x == FAILURE) 
            {
                 system("pause");
                 exit(-1);
            }
			// read in chunk data
			pChunkData[cnt] = readChunkData(fptr, chunk[cnt].chunkSize);
			if(pChunkData[cnt] == NULL)
            {
                system("pause");
                exit(-1);                                
            }

			if(memcmp( &(chunk[cnt].chunkID), "data", 4) == 0)
			{
				dataFlag = cnt;	// found data chunk
				break;
			}

			cnt++;
		}
	}
	
	

	// pChunkData[dataFlag] is a pointer to the begining of the WAVE data
	// if 8 bit, then it is unsigned	0 to 255
	// if 16 bit, then it is signed		-32768 to +32767
	// ask me any other questions
	// the web page should answer others
	
	dataChunkSize = (chunk[dataFlag].chunkSize);
	noSampleFrames = dataChunkSize/format.blockAlign;
	
	printf("Size of data chunk: %ld\nSample Frames: %d\n",dataChunkSize,noSampleFrames);
	
	
	cnt = 0;
    
    
	int noOfBlks = 4, i, r;
	BYTE * start = pChunkData[dataFlag];
	srand ( time(NULL) );
	long long left,right,leftm,rightm,diff; /* used long long as I anticipate large averages being required to minimize effect on cover */
	ifstream::pos_type size;
	char * b_message; 

    /* http://www.cplusplus.com/doc/tutorial/files/ */
    ifstream file ("message.bin", ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        b_message = new char [size];
        file.seekg (0, ios::beg);
        file.read (b_message, size);
        file.close();

        cout << "the message file content is in memory" << endl;

        
    }
    else cout << "Unable to open file";
    /* code quote ends here */
    
	

    while(cnt < size && cnt < noSampleFrames )
    {                  
          for(i = 0; i < 4 /*sample frame size in bytes */ ; i ++)
          {
              leftm = (long long)twoBytes2Long(b_message[(cnt * format.blockAlign) + 4 * i] , b_message[(cnt * format.blockAlign) +1  + 4 * i] );
              rightm = (long long)twoBytes2Long(b_message[(cnt * format.blockAlign) + 2 + 4 * i], b_message[(cnt * format.blockAlign) +3  + 4 * i] );
          }
          
          
              left = 0,right =0; 
              for(i = 0; i < noOfBlks ; i ++){
              
              left += (long long)twoBytes2Long(start[(cnt * format.blockAlign) + 4 * i] , (start[(cnt * format.blockAlign) +1  + 4 * i] ));
              right += (long long)twoBytes2Long(start[(cnt * format.blockAlign) + 2 + 4 * i], start[(cnt * format.blockAlign) +3  + 4 * i] );
             
              
              }
              //cout<< "left: " << left << endl;
              //cout<< "right: " << right << endl;
              left = left/noOfBlks;
              right = right/noOfBlks;
              //cout<< "left average: " << left << endl;
              //cout<< "right average: " << right << endl;
              cout<< "left average a: " << left << "left message: " << leftm << endl;
              cout<< "right average a: " << right <<"right message: " << rightm << endl;
              if((diff = leftm - left))
              {
                        cout << "diff: " << diff <<endl;
                        for( i = 0 ; i < noOfBlks; i ++){
                      
                            start[(cnt * format.blockAlign) + 4 * i] += diff & 255;
                            start[(cnt * format.blockAlign) + 1 + 4 * i] += diff >> 8;
                        
                        }
                        while(left != leftm){
                                    x = rand() % 4;
                                   
                                    if(left > leftm)
                                    {
                                            start[(cnt * format.blockAlign) + 4 * x]++;
                                            if (start[(cnt * format.blockAlign) + 4 * x] == 0)
                                            {
                                                 start[(cnt * format.blockAlign) + 4 * x + 1] ++;
                                            }
                                    }else{     
                                            start[(cnt * format.blockAlign) + 4 * x]--;
                                            if (start[(cnt * format.blockAlign) + 4 * x] == 0xFF )
                                            {
                                                 start[(cnt * format.blockAlign) + 4 * x + 1] --;
                                            }
                                    }  
                                    for(i = 0; i < noOfBlks ; i ++){
              
                                    left += (long long)twoBytes2Long(start[(cnt * format.blockAlign) + 4 * i] , (start[(cnt * format.blockAlign) +1  + 4 * i] ));
              
                                    
              
                                    }     
                                    left/=4; 
                                            
                        } 
              }
              if((diff = rightm - right))
              {
                         cout << "diff: " << diff <<endl; 
                        for( i = 0 ; i < noOfBlks; i ++){
                              
                              
                            start[(cnt * format.blockAlign) + 2 + 4 * i] += diff & 255;
                            start[(cnt * format.blockAlign) + 3 + 4 * i] += diff >> 8;
                        }    
                        while(right != rightm){
                                    x = rand() % 4;
                                   
                                    if(right > rightm)
                                    {
                                            start[(cnt * format.blockAlign) + 4 * x + 2]++;
                                            if (start[(cnt * format.blockAlign) + 4 * x + 2] == 0)
                                            {
                                                 start[(cnt * format.blockAlign) + 4 * x + 3] ++;
                                            }
                                    }else{     
                                            start[(cnt * format.blockAlign) + 4 * x + 2]--;
                                            if (start[(cnt * format.blockAlign) + 4 * x + 2] == 0xFF )
                                            {
                                                 start[(cnt * format.blockAlign) + 4 * x + 3] --;
                                            }
                                    } 
                                    for(i = 0; i < noOfBlks ; i ++)
                                    {
              
                                    right += (long long)twoBytes2Long(start[(cnt * format.blockAlign) + 2 + 4 * i] , (start[(cnt * format.blockAlign) +3  + 4 * i] ));
              
                                    
              
                                    }     
                                    right/=4;        
                                            
                        } 
              }       
               cout<< "left average b: " << left << "left message:" << leftm << endl;
               cout<< "right average b: " << right <<"right message:" << rightm << endl;
         // }
          //while(leftm != left && rightm != right);
         cout<< "left average c: " << left << "left message:" << leftm <<  endl;
         cout<< "right average c: " << right <<"right message:" << rightm << endl;
          
          cnt+=noOfBlks; 
                   
    }

	//printf("Left LSB: %X Right LSB: %X count: %d stop: %d \n", start[cnt * format.blockAlign], start[2 + (cnt * format.blockAlign)],cnt,noSampleFrames);
	
	
	

	printf("\n");
	
	fclose(fptr);
	
	printf("Exiting...\n");
	system("pause");

	exit(0);
} // main

