#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

#define FILENAMEMAXLENGTH 128
#define DIRTAG 4
#define FILETAG 8

struct dirdesc {
	unsigned short iddir;
	unsigned char* dirname;
	unsigned char isdir;
	unsigned short filecount;
	unsigned short* fileids;
};
struct filedata {
	unsigned short fileid;
	unsigned short filesize;
	unsigned char* data;
};
struct filetree {
	struct dirdesc* data;
	struct filetree* parent;
	struct filetree* children;
};

void printfiletree(struct filetree *node) {
	if (node == NULL) return;
	struct dirdesc* data = node->data;
	printf("name: %s files: %d isdir: %u\n", data->dirname, data->filecount, data->isdir);
	for (int i = 0; i < data->filecount; i++) {
		// ERROR
		struct filetree next = (node->children)[i];
		printfiletree(&next);
	}
}

void scandir2(char* dirname, int deep, struct filetree *tree) {
	DIR *dir;
	struct dirent *ent;
	int files = 0;
	int index = 0;
	char subpath[FILENAMEMAXLENGTH];
	for (int i = 0; i < deep; i++) printf("-");
	printf("%s\n", dirname);
	if ((dir = opendir(dirname)) == NULL) {
		printf("/unable to open\n");
		return;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_name[0] != '.') { 
			files++;
		}
	}
	tree->data->filecount = files;
	tree->children = (struct filetree*)malloc(sizeof(struct filetree) * files);
	dir = opendir(dirname);
	ent = NULL;
	printf("files: %d\n", files);
	while ((ent = readdir(dir)) != NULL) {
		subpath[0] = '\0';
		strcat(subpath, dirname);
		strcat(subpath, "/");
		if (ent->d_type == DIRTAG) {
			if (strcmp(ent->d_name, ".") && (strcmp(ent->d_name, ".."))) {
				// if current dir is not . ..
			
				strcat(subpath, ent->d_name);
				struct filetree item;
				struct dirdesc data;

				data.iddir = 0;
				data.dirname = subpath;
				data.isdir = 1;
				data.fileids = NULL;
				data.filecount = 0;
				
				item.parent = tree;
				item.children = NULL;
				item.data = &data;

				(tree->children)[index] = item;
				index++;
				scandir2(subpath, ++deep, &item);
			}
		}
		else { 
			strcat(subpath, ent->d_name);
			printf("+ new file : %s\n", subpath);
		
			struct filetree item;
			struct dirdesc data;

			data.dirname = subpath;
			data.fileids = NULL;
			data.filecount = 0;
			data.iddir = 0;
			data.isdir = 0;
			
			item.data = &data;
			item.parent = tree;
			item.children = NULL;
			
			(tree->children)[index] = item;
			index++;
		}
	}
	return;
}


int main(int argc, char** argv) {
	if (argc <= 1) {
		printf("enter arg\n");
		return -1;
	}
	if (argc > 3) {
		printf("too many args\n");
		return -2;
	}
	printf("Archivator TATAR...\n");
	char* path;
	path = argv[1];

	struct filetree *root = (struct filetree*)malloc(sizeof(struct filetree));
	root->children = NULL;
	root->parent = NULL;

	struct dirdesc data;
	data.dirname = path;
	data.iddir = 0;
	data.isdir = 1;
	data.filecount = 0;
	data.fileids = NULL;

	root->data = &data;

	printf("_______START ANALYZ TREE_______\n");
	scandir2(path, 0, root);
	printf("_______STOP  ANALYZ TREE_______\n");

	printf("_______PRINT TREE_______\n");
	printfiletree(root);
	printf("_______STOP PRINT_______\n");
	
	return 0;
}
