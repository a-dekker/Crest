#include <QtQuick>
#include <sailfishapp.h>
#include "crest.h"
#include <QVariant>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define readc(a,b) (read(a, &b, 1))

std::vector<proc> ps::get_ps() {
    int pp[2], i, a, b;
    char tmp;
    char buff[129];
    std::vector<proc> running;
    proc pr;
    pid_t ch_pid;

    if(pipe(pp) == -1)
        return running;
    if((ch_pid = fork()) == 0) {
       close(1);
       close(pp[0]);
       dup2(pp[1],1);
       execlp("ps","ps", "axo", "%cpu,rss,args", NULL);
       return running;
    } else {
       close(pp[1]);
       waitpid(ch_pid,NULL,0);
       if(readc(pp[0], tmp) < 1)
          tmp = 0;
       while( tmp != 0) {
          pr.cpu = 0;
          pr.rss = 0;
          pr.proc_name = "";
          // Space
          while( ! isdigit(tmp) )
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          i = 0;
          // CPU usage
          while( isdigit(tmp) || (tmp == '.') ) {
             buff[i++] = tmp;
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          }
          if(i > 0) {
             buff[i] = 0;
             a = b = 0;
             sscanf(buff, "%d.%d", &a, &b);
             pr.cpu = a*10 + b;
          }
          // Space
          while( ! isdigit(tmp) )
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          i = 0;
          // Memory
          while( isdigit(tmp) ) {
             buff[i++] = tmp;
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          }
          if(i > 0) {
             buff[i] = 0;
             sscanf(buff, "%d", &(pr.rss));
          }
          // Space
          while( isspace(tmp) )
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          // Process name
          i = 0;
          while( tmp != '\n' && tmp != 0 && i < 128) {
             buff[i++] = tmp;
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          }
          if(i > 0) {
             buff[i] = 0;
             pr.proc_name = buff;
          }
          if(i == 128)
             while( tmp != '\n' )
                if(readc(pp[0], tmp) < 1) {
                   tmp = 0;
                   goto save;
                }
 save:
          if(pr.proc_name.length() > 1) {
             running.push_back(pr);
          }
       }
       close(pp[0]);
       return running;
    }
}

QVariantList ps::get_ps_by(QString by, bool only_gui) {
    QVariantList ls;
    static char buff[16];
    QVariantMap mp;
    std::vector<proc> procs = get_ps();

    if(by=="cpu") {
        std::sort(procs.begin(), procs.end(), [](proc a, proc b) { return a.cpu > b.cpu; });
    } else if(by=="rss") {
        std::sort(procs.begin(), procs.end(), [](proc a, proc b) { return a.rss > b.rss; });
    } else if(by=="name") {
        std::sort(procs.begin(), procs.end(), [](proc a, proc b) { return a.proc_name < b.proc_name; });
    }

    for(auto i : procs) {
        mp.clear();
        mp.insert("name", i.proc_name);
        if(i.rss < 999)
           sprintf(buff,"%d kB", i.rss);
        else if (i.rss < 4092)
           sprintf(buff,"%d %3d kB", i.rss/1000, i.rss %1000);
        else {
           int tmp = (i.rss * 100) / 1024;
           sprintf(buff,"%d.%2d MB", tmp/100, tmp % 100);
        }
        mp.insert("rss", QString(buff));
        sprintf(buff,"%d.%d %%", i.cpu/10, i.cpu%10);
        mp.insert("cpu", QString(buff));
        ls.append(mp);
    }
    return ls;
}

int main(int argc, char *argv[])
{
    qmlRegisterType<ps>("PS", 1, 0, "PS");

    return SailfishApp::main(argc, argv);
}

