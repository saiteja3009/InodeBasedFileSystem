#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include<bits/stdc++.h>
using namespace std;

unordered_map<string,int>modes;
unordered_map<string,int>file_names;
vector<string>files;

struct super_block{
    int num_of_inode;
    int num_of_blocks;
    int size_block;
    int bit_inode[20000];
    int bit_block[1000000];
};

struct inode{
    int size;
    char name[20];
    int first_block;
    int last_block;
    int mode;
};

struct block{
    int next_block;
    char data[512];
};

struct super_block sb;
struct inode *inodes;
struct block *blocks;

int create_disk(string str){
    sb.num_of_inode=20000;
    sb.num_of_blocks=1000000;
    sb.size_block=sizeof(struct block);
    inodes=(inode *)malloc(sizeof(struct inode)*sb.num_of_inode);
    for(int i=0;i<sb.num_of_inode;i++){
        inodes[i].first_block=-1;
        inodes[i].last_block=-1;
        string nn="NA";
        strcpy(inodes[i].name,nn.c_str());
        inodes[i].size=-1;
        inodes[i].mode=-1;
        sb.bit_inode[i]=-1;
    }
    blocks=(block *)malloc(sizeof(struct block)*sb.num_of_blocks);
    for(int i=0;i<sb.num_of_blocks;i++){
        blocks[i].next_block=-1;
        string nn="";
        strcpy(blocks[i].data,nn.c_str());
        sb.bit_block[i]=-1;
    }

    FILE *file;
    file=fopen(str.c_str(),"w+");
    fwrite(&sb,sizeof(struct super_block),1,file);
    fwrite(inodes,sizeof(struct inode)*sb.num_of_inode,1,file);
    fwrite(blocks,sizeof(struct block)*sb.num_of_blocks,1,file);
    fclose(file);
    return 0;

}
int find_empty_inode(){
    int i;
    for(int i=0;i<sb.num_of_inode;i++){
        if(sb.bit_inode[i]==-1){
            return i;
        }
    }return -1;
}
int find_empty_block(){
    int i;
    for(int i=0;i<sb.num_of_blocks;i++){
        if(sb.bit_block[i]==-1){
            return i;
        }
    }return -1;
}

int create_file(string name){
    int in=find_empty_inode();
    sb.bit_inode[in]=1;
    files.push_back(name);
    strcpy(inodes[in].name,name.c_str());
    return in;
}

void write_file(int fd,int x){
    cout<<"Enter text to write into file and write end in a new line to end writing\n";
    string comp_str="";
    string s;
    while(getline(cin,s) && s!="end"){
        comp_str+=s+"\n";
    }
    //cout<<comp_str<<comp_str.length();
    //cout<<comp_str;
    //cout<<comp_str.length();
    int file_size=comp_str.length();
    inodes[fd].size=file_size;
    int prev=-1;
    int start=0,end=0;
    bool first=true;
    
    while(file_size>0){
        int ib=find_empty_block();
        if(x==0){
            blocks[inodes[fd].last_block].next_block=ib;
            x=2;
        }
        sb.bit_block[ib]=1;
        if(first==true && x==1){
            inodes[fd].first_block=ib;
            first=false;
        }
        if(prev!=-1){
                blocks[prev].next_block=ib;
            }
        int cur=min(510,file_size);
        string store=comp_str.substr(start,cur);
        strcpy(blocks[ib].data,store.c_str());
        start=start+cur;
        //blocks[ib].data=(*store);
        prev=ib;
        file_size-=cur;
    }
        blocks[prev].next_block=-2;
        inodes[fd].last_block=prev;
    cout<<"Written into file\n";
}


void read_file(int fd){
    if(inodes[fd].first_block==-1){
        cout<<"No file content exists\n";
        return;
    }
    int flag=1;
    int first=inodes[fd].first_block;
    int last=inodes[fd].last_block;
    while(first!=last){
        flag=0;
        cout<<blocks[first].data;
        first=blocks[first].next_block;
    }
    cout<<blocks[last].data;
    cout<<"File content read successfully\n";
}
void append_file(int fd){
    if(inodes[fd].first_block==-1){
        cout<<"No previous content exists\n";
    }
    write_file(fd,0);
}

void read_disk(string str)
{
    FILE *file;
    file=fopen(str.c_str(),"r");
    fread(&sb,sizeof(struct super_block),1,file);
    inodes=(inode *)malloc(sizeof(struct inode)*sb.num_of_inode);
    blocks=(block *)malloc(sizeof(struct block)*sb.num_of_blocks);
    fread(inodes,sizeof(struct inode),sb.num_of_inode,file);
    fread(blocks,sizeof(struct block),sb.num_of_blocks,file);
    for(int i=0;i<sb.num_of_inode;i++){
        if(inodes[i].first_block!=-1){
                files.push_back(inodes[i].name);
                file_names[inodes[i].name]=i;
        }
    }
    fclose(file);
}

void sync_disk(string str)
{
    FILE *file;
    file=fopen(str.c_str(),"w+");
    fwrite(&sb,sizeof(struct super_block),1,file);
    fwrite(inodes,sizeof(struct inode)*sb.num_of_inode,1,file);
    fwrite(blocks,sizeof(struct block)*sb.num_of_blocks,1,file);
    fclose(file);
}


int main(){
    while(1){
        printf("Enter the choice\n");
        printf("1. Create Disk\n2. Mount Disk\n3. Exit\n");
        int choice;
        scanf("%d",&choice);
        if(choice==1){
            cout<<"Enter the disk name\n";
            string disk;
            cin>>disk;
            int x=create_disk(disk);
            if(x==-1)
            {
                cout<<"Disk Creation failed\n";
            }
            else{
                cout<<"Disk is successfully created\n";
            }
        }
        else if(choice==2){
            cout<<"Enter disk name\n";
            string file_name;
            cin>>file_name;
            read_disk(file_name);
            cout<<"Disk mounted\n";
            while(1){
                printf("Enter the choice\n");
                printf("1. Create File\n2. Open File\n3. Read File\n4. Write File\n5. Append File\n6. Close File\n7. Delete File\n8. List of files\n9. List of opened files\n10. Unmount\n");
                int cho;
                cin>>cho;
                if(cho==1){
                    cout<<"Enter File Name\n";
                    string filename;
                    cin>>filename;
                    int x;
                    x=create_file(filename);
                    //sb.filenames[filename]=x;
                    file_names[filename]=x;
                    //cout<<x<<"\n";
                }
                else if(cho==2){
                    cout<<"Enter the file name\n";
                    string name;
                    bool flag=false;
                    cin>>name;
                    for(int i=0;i<files.size();i++){
                        if(name==files[i])
                        {
                            flag=true;
                        }
                    }
                       if(flag==false){ cout<<"File not exists\n";
                    }
                    else{
                    int fd=file_names[name];
                    //int fd=sb.filenames[filename];
                    cout<<"Enter the mode to open file\n";
                    printf("0. Read\n1. Write\n2. Append\n");
                    int mode;
                    cin>>mode;
                    inodes[fd].mode=mode;
                    modes[name]=mode;
                    if(mode==0){
                            cout<<"File: "<<name<<" opened in [Read] mode with fd "<<fd<<"\n";
                        }
                        else if(mode==1){
                            cout<<"File: "<<name<<" opened in [Write] mode with fd "<<fd<<"\n";
                        }
                        else if(mode==2){
                            cout<<"File: "<<name<<" opened in [Append] mode with fd "<<fd<<"\n";
                        }
                    //cout<<"Opened file in given mode with fd: "<<fd<<"\n";
                    //sb.opened_files[fd]=mode;
                    }

                }
                else if(cho==3){
                    cout<<"Enter File Desc\n";
                    int fd;
                    cin>>fd;
                    //int fd=sb.filenames[filename];
                    if(inodes[fd].mode!=0){
                        cout<<"File not opened in required mode\n";
                    }
                    else{
                        read_file(fd);
                    }
                }
                else if(cho==4){
                    cout<<"Enter File Desc\n";
                    int fd;
                    cin>>fd;
                    //int fd=sb.filenames[filename];
                    if(inodes[fd].mode!=1){
                        cout<<"File not opened in required mode\n";
                    }
                    else{
                        write_file(fd,1);
                    }
                }
                else if(cho==5){
                    cout<<"Enter File Desc\n";
                    int fd;
                    cin>>fd;
                    //int fd=sb.filenames[filename];
                    if(inodes[fd].mode!=2){
                        cout<<"File not opened in required mode\n";
                    }
                    else{
                        append_file(fd);
                    }
                }
                else if(cho==6){
                    cout<<"Enter the file descriptor to close\n";
                    int fd;
                    cin>>fd;
                    inodes[fd].mode=-1;
                    //sb.opened_files.erase(fd);
                    modes.erase(inodes[fd].name);
                    cout<<"File closed\n";
                }
                else if(cho==7){
                    cout<<"Enter the file name\n";
                    string name;
                    cin>>name;
                    if(modes.find(name)!=modes.end()){
                        cout<<"The file should be closed before deleting\n";
                    }
                    else{
                    int fd=file_names[name];
                    sb.bit_inode[fd]=-1;
                    cout<<"Deleted file\n";
                    int x;
                    for(int i=0;i<files.size();i++){
                        if(files[i]==name)
                        {
                            x=i;
                            break;
                        }
                    }
                    files.erase(files.begin()+x);
                    }
                }
                else if(cho==8){
                    cout<<"List of Files:\n";
                    for(int i=0;i<files.size();i++){
                        cout<<files[i]<<"\n";
                    }
                }
                else if(cho==9){
                    cout<<"List of opened files:\n";
                    for (auto i = modes.begin(); i != modes.end(); i++)
                    {
                        if(i->second==0){
                            cout<<"File: "<<i->first<<" opened in [Read] mode with fd "<<file_names[i->first]<<"\n";
                        }
                        else if(i->second==1){
                            cout<<"File: "<<i->first<<" opened in [Write] mode with fd "<<file_names[i->first]<<"\n";
                        }
                        else if(i->second==2){
                            cout<<"File: "<<i->first<<" opened in [Append] mode with fd "<<file_names[i->first]<<"\n";
                        }
                    }
                }
                else if(cho==10){
                    cout<<"unmounted\n";
                    files.clear();
                    file_names.clear();
                    modes.clear();
                    break;
                }

            }
            sync_disk(file_name);
        }
        else if(choice==3){
            break;
        }
        else{
            printf("Enter correct choice\n");
        }

    }
}