#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>
#include "headerboiz.h"

int add(char toadd,treeNode* parent)
{
	int index; //index of the array where the data needs to be added

	//if char is uppercase set to lowercase for proper hashing, can't use isalpha because correct math is needed for proper indexing
	if ((isalpha(toadd)) && (((int)toadd - (int)'a') < 0))
	{
		index = ((int)toadd - (int)'A');
	}

	//set proper index for numbers
	else if(isdigit(toadd))
	{
		index = ((int)toadd - 22);
	}

	//index for lowercase letters
	else
	{
		index = ((int)toadd - (int)'a');
	}

	//if the array slot that needs to be inserted into is null, create it
	if (parent->next[index] == NULL)
	{
		
		//if the character to be added is a letter add 97 for proper ASCII value
		if(isalpha(toadd))
		{
			parent->next[index] = buildNode((char)(index + 97),-1,(parent->level)+1);

			//increment max word length if node is being added to the deepest level
			if(parent->level + 2 > maxlen)
			{
				maxlen++;
			}
		}

		//if the character to be added is a number subtract 26 for proper ASCII value
		else
		{
			parent->next[index] = buildNode(toadd,-1,(parent->level)+1);

			//increment max word length if node is being added to the deepest level
			if(parent->level + 2 > maxlen)
			{
				maxlen++;
			}
		}
	}

	//if not null then up the count for stored values
	else
	{
		parent->next[index]->count++;
	}

	//make the child refer to the parent
	parent->next[index]->parent = parent;

	//return the index of the added piece of data
	return index;
}

//frees the count array in a single treeNode and then frees the treeNode
void deleteOne(treeNode* todelete)
{

	if(todelete->countarr != NULL){
		int i = 0;
		while(i < filecount)
		{	
			if(todelete->countarr[i] != NULL)
			{
				if(todelete->countarr[i]->filepath != NULL)
				{
					free(todelete->countarr[i]->filepath);
				}
				
				free(todelete->countarr[i]);
			}
			i++;
		}

		free(todelete->countarr);
	} 
	
	free(todelete);
}

//delete the whole Tree
void deleteTree(treeNode* parent) 
{
	int i;

	i = 0;

	//case where nothing needs to be freed
	if(parent == NULL)
	{
		return;
	}

	//cycle through every entry in the tree
	if(parent->next != NULL)
	{
		for(i = 0; i < 36; i++)
		{
			if(parent->next[i] != NULL)
			{
				deleteTree(parent->next[i]);
			}
		}
	}

	//delete the entry when bubbling back up
	deleteOne(parent);
}

//creates a node to insert into a tree array slot
treeNode* buildNode(char data,int isWord,int level)
{
	treeNode* node = (treeNode*)calloc(1,sizeof(treeNode));
	node->data = data;
	node->count = 1;
	node->isWord = isWord;
	node->level = level;
	node->countarr = NULL;

return node;
}

//creates a node to insert into a count array
countNode* buildCountNode(char* path)
{
	countNode* node = (countNode*)malloc(1*sizeof(countNode));
	node->count = 1;
	node->filepath = (char*)malloc(1*(strlen(path)+1));
	memcpy(node->filepath,path,strlen(path));
	node->filepath[strlen(path)] = '\0';
	return node;
}

//creates the root and builds every node in the tree, if a word is found ups the word count for the proper file in the count array
int buildTree(char* input, size_t size, int filecount, int fileindex, char* path)
{
	int i = 0;
	treeNode* temp = NULL; //for travsersals
	fWordCount = 0; //set file word count
	temp = root;

	for (i = 0;i <size;i++) {
		//check if the input is a letter and insert it if it is
		if (isalpha(input[i])) {
			temp = temp->next[add(input[i],temp)];
		}

		//if the first character is a number return to the root
		else if((isdigit(input[i])) && (temp->level < 0)){
			temp = root;	
		}

		//if the input is a number and the the first character in a word then insert it
		else if((isdigit(input[i])) && (temp->level >= 0)){
			temp = temp->next[add(input[i],temp)];	
		}

		//not a letter or a number. Check if at the root and if not signal the end of a word and return to the root
		else{
			if (temp->data != '$')
			{
				temp->isWord = 0;
				wordcount++;
				fWordCount++;

				//if the count array does not need to be created them simply increment the proper index
				if(temp->countarr != NULL)
				{
					//if the count isn't 0 and the path is already set then just increment the count
					if(temp->countarr[fileindex] != NULL && temp->countarr[fileindex]->count != 0){
						temp->countarr[fileindex]->count++;
					}

					//if the array is already allocated but the file path isn't set yet then increment the count and set the file path
					else{
						temp->countarr[fileindex] = buildCountNode(path);
					}
				}

				//if the count array that need to be added to does not exist then create it and add to it
				else{
					temp->countarr = (countNode**)calloc(filecount, sizeof(countNode*));
					temp->countarr[fileindex] = buildCountNode(path);
				}
			}
			temp = root; //return to top of tree
		}	
	}

	return 0;
}