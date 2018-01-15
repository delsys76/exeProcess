#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>





int fn_GetProcessID(char *name)
{
    DIR *dir_p;
    struct dirent *dir_entry_p;
    char dir_name[40];  // ??? buffer overrun potential
    char target_name[252];  // ??? buffer overrun potential
    int target_result;
    char exe_link[252];
    int errorcount;
    int ret;

    errorcount=0;
    ret = -1;
    dir_p = opendir("/proc/"); // Open /proc/ directory
    while(NULL != (dir_entry_p = readdir(dir_p)))
    {
        if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name))
        { // Checking for numbered directories 
            strcpy(dir_name, "/proc/");
            strcat(dir_name, dir_entry_p->d_name);
            strcat(dir_name, "/");      // Obtaining the full-path eg: /proc/24657/ 
            exe_link[0] = 0;
            strcat(exe_link, dir_name);
            strcat(exe_link, "exe"); // Getting the full-path of that exe link
            target_result = readlink(exe_link, target_name, sizeof(target_name)-1); // Getting the target of the exe ie to which binary it points to
            if (target_result > 0)
            {
                target_name[target_result] = 0;
                if (strstr(target_name, name) != NULL)
                {       // Searching for process name in the target name -- ??? could be a better search !!!
                    ret = atoi(dir_entry_p->d_name);
                    printf("getProcessID(%s) :Found. id = %d\n", name, ret);
                    closedir(dir_p);
                    return ret;
                }
            }
        }
    }
    closedir(dir_p);
    printf("getProcessID(%s) : id = 0 (could not find process)\n", name);
    return ret;
}

int fn_KillProcess(int pid)
{
    int ret = 1;
    char kill_cmd[100] = {0,};

    sprintf(kill_cmd, "kill -9 %d",pid);
    ret = system(kill_cmd);

    return ret;
}

int fn_ExitProcess()
{
    int ret = 0;
    int pid = 0;
    int status;

    pid = fn_GetProcessID("top");
    if(pid >0)
    {
        ret =  fn_KillProcess(pid);
        waitpid(-1, &status, 0);
    } 
}


int fn_ExecProcess()
{
    int ret = 1;
    int pid;
    int child_pid;

    fn_ExitProcess();
    sleep(1);

    if((pid = fork()) == 0)
    {
        child_pid = getpid();
        if(execl("/bin/sh","/bin/sh","./execs/exe_script",NULL) == -1)
        {
            printf("Start Fail\n");
            exit(0);
        }
        else
        {
            printf("Start......\n");
        }

    }


    sleep(10);

    fn_ExitProcess();

    return ret;

}



void main()
{
    fn_ExecProcess();
}
