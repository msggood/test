#ifndef _FILEMANAGE_H_
#define _FILEMANAGE_H_
int list_file(int sod);
void download(int sfd, char *file);//上传
void upload(int sfd, char *file, int mode);//下载
void sem_p(int sid);//进行p操作
void sem_v(int sid);//进行v操作
void *get_shm(int shm_id);//取得共享内存id
int creat_shm();//创建共享内存
void init_semval(int sem_id);//初始化信号量
int  sem_create();//创建信号量
#endif


















