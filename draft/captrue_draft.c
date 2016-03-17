#if 0
void ftp_redo(void)
{
    sleep(30);
    if(connet_ftpserver_state == DISCONNECT)
         connet_and_send_data();
     else
         read_data_from_db_and_send();
}
#endif
#if 0
           pthread_mutex_lock(&send_mutex);
           pthread_cond_wait(&cond,&send_mutex);
           pthread_mutex_unlock(&send_mutex);   
#endif

#if 0
static void sig_action(int sig,siginfo_t *info, void*ctx)
{
//	int ret;
	sigset_t newmask, oldmask;
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGUSR1);
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) /* 屏蔽newmask,将当前的保存到oldmask */
		perror("sigprocmask(SIG_BLOCK) error!");	
    if(sig != SIGUSR1)
        return;
    switch(info->si_int)
    {
        case ADD_FILE_RECORD:
        default:
            if(login_state == LOGIN)
            {
#if 0            
                pthread_mutex_lock(&send_mutex);
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&send_mutex);
#endif                
            }
            break;         
#if 0                        
            DEBUG("ftpclient:SIGNAL_SEND\n");
            if(login_state != LOGIN)
           	{
             	ret=ftp_login();
    	        if(ret==OK)
	    		{    
		    		login_state = LOGIN;                    
			    }else
    			{
	    			login_state = DISLOGIN;
                    DEBUG("exit(-1)\n");
                    exit(-1);
		    	}
            }
            else                
#endif                
#if 0                
                ret=read_data_from_db_and_send();   
                if(ret!=0)
                {
                    DEBUG("exit(-1)\n");
                    exit(-1);
                }
#endif                
            
    }
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) /*恢复成oldmask */
		perror("sigprocmask(SIG_SETMASK) error!");
}
#endif


#if 0  
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO;
    act.sa_sigaction=sig_action;
    if(sigaction(SIGUSR1,&act,NULL)<0)
    {
        DEBUG("sigaction error!\n");
        exit(-1);
    }
    if(sigaction(SIGUSR2,&act,NULL)<0)
    {
        DEBUG("sigaction error!\n");
        exit(-1);
    }
#endif

#if 0

if(fork() ==0)
  {
    sprintf(buf,"%s","ab");
    write(filedes[1],buf,sizeof(buf));
  }
  else
  {
    read(filedes[0],buf,sizeof(buf));
    DEBUG("%s\n",buf);
  }

int ret;
pthread_mutex_init(&send_mutex,NULL);  
pthread_cond_init(&cond,NULL);     
pthread_mutex_init(&send_mutex,NULL); 
void *thread_result;  
pthread_t tid_send; /* Stream capture in another thread */
ret = pthread_create(&tid_send, NULL, read_data_from_db_and_send, NULL);
if(ret!=0)
{
    DEBUG("exit(-1)\n");
    exit(-1);
}

#endif

#if 0
static void *read_data_from_db_and_send(void*m)
{  
    while(1)
    {    
        pthread_mutex_lock(&send_mutex);
        pthread_cond_wait(&cond,&send_mutex);
        pthread_mutex_unlock(&send_mutex);            
        while(login_state == LOGIN)
        {    
            char send_buf[50];
            int ret=0;    
            memset(send_buf,'\0',sizeof(send_buf));
            ret=pop_filename_item(file_name_db,"filename_table",send_buf);
        //        show_all_table(file_name_db,"jpeg_h264");
    //          DEBUG("send:%s,len1=%d\n",send_buf,sizeof(send_buf));
            if(ret==1)
            {
               ret=ftp_put(send_buf);
               if(ret==ERROR)
                   break;
               ret=ftp_get_reply(sock_control);
               if(ret==226)
               {                                       
                    FILE *fp = popen("ps -e | grep \'recorder\' | awk \'{print $1}\'", "r");    
                    if(fp==NULL)
                        break;
                    char buffer[30] = {0};
                    while (NULL != fgets(buffer, 30, fp)) //逐行读取执行结果并打印
                    {
                        DEBUG("PID:%s\n", buffer);
                        break;
                    }
                    recorder_pid=atoi(buffer);
                    pclose(fp); //关闭返回的文件指针，注意不是用fclose噢
                    union sigval mysigval;
                    mysigval.sival_int = DEL_FILE_RECORD;
                    if(sigqueue(recorder_pid,SIGUSR1,mysigval) == -1){
                        perror("sigqueue DEL FILE RECORD SIGNAL to recorder_pid error\r\n");
                    }
                    del_filename_item(file_name_db,"filename_table",send_buf);
          	        if(send_and_del_local_file == 1)
	        	        del_file(send_buf);					
               }
            }else
            {
                break;            
            }
        }
    }
    return NULL;
}
#endif
