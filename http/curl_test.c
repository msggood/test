#include <stdio.h>  
#include <string.h>  
#include <curl/curl.h>  
#include <unistd.h>

int recv_func_callbak(void *ptr, int size, int nmemb, void *userp)
{
    strcat( userp, ptr);
    return size * nmemb;     //必须返回这个大小, 否则只回调一次, 不清楚为何.
}

int main(int argc, char *argv[])  
{  
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  
  char rec_page[1024];
  
  if(curl_global_init(CURL_GLOBAL_ALL)!=0)
  {
    printf("curl_global_init error!\r\n");
  }
  
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "sendfile",  
               CURLFORM_FILE, "./test.file",  
               CURLFORM_END);  
#if 0  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "filename",  
               CURLFORM_COPYCONTENTS, "test.file",  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);  
#endif  
  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
//  headerlist = curl_slist_append(headerlist, buf);  
  memset(rec_page,'\0',1024);
  if(curl) {  
    curl_easy_setopt(curl, CURLOPT_URL, "http://121.40.175.154:55555/?m=home&c=api&a=Hua_GetRouteCheckpoint&deviceId=1111");  
//    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
//    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);  
  
    /* Perform the request, res will get the return code */  
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, recv_func_callbak);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, rec_page);
    res = curl_easy_perform(curl);  

    /* Check for errors */  
    if(res != CURLE_OK)  
      fprintf(stderr, "curl_easy_perform() failed: %s\n",  
              curl_easy_strerror(res));  
  
    /* always cleanup */  
    printf("%s\r\n",rec_page);
    
    curl_easy_cleanup(curl);  
  
    /* then cleanup the formpost chain */  
    curl_formfree(formpost);  
    /* free slist */  
    curl_slist_free_all (headerlist);  
  }  
  return 0;  
}  
