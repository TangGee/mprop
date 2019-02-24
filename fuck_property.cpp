#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/system_properties.h>

struct prop_info;

extern int __system_property_area_init();
extern int __system_property_get(const char *name, char *value);
int __system_property_add(const char *name, unsigned int namelen,
                          const char *value, unsigned int valuelen);
extern const prop_info *__system_property_find(const char *name);

extern void *__system_property_area__ = NULL;
extern int __system_property_update(prop_info *pi, const char *value, unsigned int len);

void usage() {
    printf("usage: mprop property_name  property_value\n");
}


int main(int argc,char **argv){
   printf("start....\n");
   const int fd = open("/dev/__properties__",
                       O_RDWR | O_NOFOLLOW | O_CLOEXEC | O_EXCL, 0444);

   if (argc!=3) {
       usage();
       return -1;
   }

   if (strlen(argv[1])+1>PROP_NAME_MAX) {
       printf("property name is to long\n");
       return -1;
   }

   if (sizeof(argv[2])+1>PROP_VALUE_MAX) {
       printf("property value is to long\n");
       return -1;
   }

   if (fd<0) {
      perror("open");
       return -1;
   }

   struct stat fd_stat;
   if (fstat(fd, &fd_stat) <0) {
       perror("fstat");
       return -1;
   }

   if (!S_ISREG(fd_stat.st_mode)) {
       printf("internal error\n");
       return -1;
   }

    void *const memory_area = mmap(NULL, fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    __system_property_area__ = memory_area;

    char value[PROP_VALUE_MAX];
    bool update = true;
    if(__system_property_get(argv[1],value)<=0) {
        update = false;
    }

    if (update) {
        printf("old value : %s\n",value);


        prop_info *pinfo = const_cast<prop_info *>(__system_property_find(argv[1]));
        if (!pinfo){
            printf("internal error\n");
            return -1;
        }
        if(__system_property_update(pinfo,argv[2],strlen(argv[2]))) {
            printf("update error\n");
            return -1;
        }

    } else {
        if(__system_property_add(argv[1],strlen(argv[1]),argv[2],strlen(argv[2]))) {
            printf("update error\n");
            return -1;
        }
    }

    printf("new value %s\n",argv[2]);
    return 0;

}
