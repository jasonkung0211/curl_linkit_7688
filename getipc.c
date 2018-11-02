#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

int diff = 437;
int count = 0;
int debounce = 0;
FILE *fp;
char *snapshot = "";
time_t mytime;
unsigned char *timestamp;
unsigned char *macaddr;


char *
str_replace(const char *str, const char *sub, const char *replace) {
  char *pos = (char *) str;

  int size = (
        strlen(str)
      - (strlen(sub) * 1)
      + strlen(replace) * 1
    ) + 1;

  char *result = (char *) malloc(size);
  if (NULL == result) return NULL;
  memset(result, '\0', size);
  char *current;
  while ((current = strstr(pos, sub))) {
    int len = current - pos;
    strncat(result, pos, len);
    strncat(result, replace, strlen(replace));
    pos = current + strlen(sub);
  }

  if (pos != (str + strlen(str))) {
    strncat(result, pos, (str - pos));
  }

  return result;
}


/* parse headers for Content-Length */
static size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb,
                                   void *stream)
{
  int r;
  long len = 0;

  r = sscanf(ptr, "Content-Length: %ld\n", &len);
  if(r)
    *((long *) stream) = len;

  return size * nmemb;
}

/* discard downloaded data */
static size_t discardfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
  (void)ptr;
  (void)stream;
  return size * nmemb;
}

/* read data to upload */
static size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
  FILE *f = stream;
  size_t n;

  if(ferror(f))
    return CURL_READFUNC_ABORT;

  n = fread(ptr, size, nmemb, f) * size;

  return n;
}


static int upload(CURL *curlhandle, const char *remotepath,
                  const char *localpath, long timeout, long tries)
{
  FILE *f;
  long uploaded_len = 0;
  CURLcode r = CURLE_GOT_NOTHING;
  int c;

  f = fopen(localpath, "rb");
  if(!f) {
    perror(NULL);
    return 0;
  }

  curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);

  curl_easy_setopt(curlhandle, CURLOPT_URL, remotepath);

  if(timeout)
    curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);

  curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
  curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &uploaded_len);

  curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, discardfunc);

  curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);
  curl_easy_setopt(curlhandle, CURLOPT_READDATA, f);

  /* disable passive mode */
  curl_easy_setopt(curlhandle, CURLOPT_FTPPORT, "-");
  curl_easy_setopt(curlhandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

  curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);

  for(c = 0; (r != CURLE_OK) && (c < tries); c++) {
    /* are we resuming? */
    if(c) { /* yes */
      /* determine the length of the file already written */

      /*
       * With NOBODY and NOHEADER, libcurl will issue a SIZE
       * command, but the only way to retrieve the result is
       * to parse the returned Content-Length header. Thus,
       * getcontentlengthfunc(). We need discardfunc() above
       * because HEADER will dump the headers to stdout
       * without it.
       */
      curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 1L);
      curl_easy_setopt(curlhandle, CURLOPT_HEADER, 1L);

      r = curl_easy_perform(curlhandle);
      if(r != CURLE_OK)
        continue;

      curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 0L);
      curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0L);

      fseek(f, uploaded_len, SEEK_SET);

      curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1L);
    }
    else { /* no */
      curl_easy_setopt(curlhandle, CURLOPT_APPEND, 0L);
    }

    r = curl_easy_perform(curlhandle);
  }

  fclose(f);

  if(r == CURLE_OK)
    return 1;
  else {
    fprintf(stderr, "%s\n", curl_easy_strerror(r));
    return 0;
  }
}

int download_jpeg(char *url)
{
  printf("%s\n", url);
  FILE *fp = fopen("out.jpg", "wb");
  if (!fp)
  {
    printf("!!! Failed to create file on the disk\n");
    return -1;
  }

  CURL *curlCtx = curl_easy_init();
  curl_easy_setopt(curlCtx, CURLOPT_URL, url);
  curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, NULL);
  curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
  curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

  CURLcode rc = curl_easy_perform(curlCtx);
  if (rc)
  {
    printf("!!! Failed to download: %s\n", url);
    fclose(fp);
    return -1;
  }

  long res_code = 0;
  curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
  if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
  {
    printf("!!! Response code: %ld\n", res_code);
    fclose(fp);
    return -1;
  }

  curl_easy_cleanup(curlCtx);
  fflush(fp);
  fclose(fp);


  // //-----------FTP Upload--------------------------------------------------------------------------
  CURL *curl = curl_easy_init();
  if(curl){
    count++;
    static char buf [80];
    char buffer[21];
    sprintf(buffer, "%lu", time(NULL));
    timestamp = &buffer[0] ;
    sprintf(buf, "ftp://ipcam:ipcam123@54.213.189.215/%s_%s.jpg" , macaddr, timestamp);
    upload(curl, buf, "out.jpg", 0, 1);
    curl_easy_cleanup(curl);
  }
  // //-----------FTP Upload--------------------------------------------------------------------------
  return 0;
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{

  if(diff > strlen(contents)){
    //pass
  }else if(diff - (strlen(contents))  > 20){
    time_t now = time(NULL);
    double secdiff = difftime(now, mytime);
    //5 sec.
    if(secdiff > 5 ){
      debounce++;
      if(debounce > 5){
        debounce = 0;
        mytime = time(NULL);
        download_jpeg(snapshot);
      }else{
        return size * nmemb;
      }
    }
  }
  diff = strlen(contents);

  return size * nmemb;
}

int main(int argc, char *args[])
{
  //args[1] >> IP , args[2] >> MAC Addr
  if (argc <= 2)
  {
    return -1;
  }

  char *url1 = "http://user:12345678@IP/snap";
  char *url2 = "http://user:12345678@IP/getalarmmotion";

  snapshot = str_replace(url1, "IP", args[1]);
  char *motion = str_replace(url2, "IP", args[1]);

  macaddr = args[2];

  CURL *curl_handle;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, motion);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if (res != CURLE_OK)
  {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  /* we're done with libcurl, so clean it up */
  curl_global_cleanup();

  return 0;
}
