/******************************************************************************
 *                                                                            *
 * Crest - top like tool for SailfishOS                                       *
 * Copyright (C) 2014 by Michal Hrusecky <Michal@Hrusecky.net>                *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                            *
 ******************************************************************************/

#include <QtQuick>
#include <sailfishapp.h>
#include "crest.h"
#include <QVariant>
#include <QRegExp>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

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
    if(!sys_check())
        return running;
    if((ch_pid = fork()) == 0) {
       close(1);
       close(pp[0]);
       dup2(pp[1],1);
       execlp("ps","ps", "axo", "pid,uid,pcpu,rss,args", NULL);
       return running;
    } else {
       close(pp[1]);
       waitpid(ch_pid,NULL,0);
       if(readc(pp[0], tmp) < 1)
          tmp = 0;
       while( tmp != 0) {
          pr.cpu = 0;
          pr.pid = 0;
          pr.uid = 0;
          pr.rss = 0;
          pr.proc_name = "";
          // Space
          while( ! isdigit(tmp) )
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          i = 0;
          // PID
          while( isdigit(tmp) || (tmp == '.') ) {
             buff[i++] = tmp;
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          }
          if(i > 0) {
             buff[i] = 0;
             sscanf(buff, "%d", &pr.pid);
          }
          // Space
          while( ! isdigit(tmp) )
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          i = 0;
          // UID
          while( isdigit(tmp) || (tmp == '.') ) {
             buff[i++] = tmp;
             if(readc(pp[0], tmp) < 1) {
                tmp = 0;
                goto save;
             }
          }
          if(i > 0) {
             buff[i] = 0;
             sscanf(buff, "%d", &pr.uid);
          }
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
          // Rest
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

bool ps::sys_check() {
    if(system("test -x \"`which ps`\"") != 0) {
        return false;
    }
    return true;
}

QVariantList ps::get_ps_by(QString by, bool only_gui) {
    std::vector<QVariantMap> ls;
    static char buff[16];
    QVariantList ret;
    int point;
    int uid = getuid();
    QVariantMap mp;
    std::vector<proc> procs = get_ps();

    if(by=="cpu") {
        std::sort(procs.begin(), procs.end(), [](proc a, proc b) { return a.cpu > b.cpu; });
    } else if(by=="rss") {
        std::sort(procs.begin(), procs.end(), [](proc a, proc b) { return a.rss > b.rss; });
    }

    for(auto i : procs) {
        mp.clear();
        if(only_gui) {
            if((((point = i.proc_name.indexOf("harbour-")) != -1) || ((point = i.proc_name.indexOf("jolla-")) != -1)) && (i.proc_name.indexOf("invoker")==-1)) {
                mp.insert("name", i.proc_name.mid(point));
            } else {
            if(i.proc_name.contains(QRegExp("^[a-z]+\\.[a-zA-Z0-9.]*\\s*$")))
                mp.insert("name", i.proc_name);
            else
                continue;
            }
        } else {
           if(i.proc_name[0] == '[')
               continue;
           mp.insert("name", i.proc_name);
        }
        if(i.rss < 999)
           sprintf(buff,"%d kB", i.rss);
        else if (i.rss < 2048)
           sprintf(buff,"%d %3d kB", i.rss/1000, i.rss %1000);
        else {
           int tmp = (i.rss * 100) / 1024;
           sprintf(buff,"%d.%2d MB", tmp/100, tmp % 100);
        }
        mp.insert("pid", i.pid);
        mp.insert("killable", i.uid == uid);
        mp.insert("rss", QString(buff));
        sprintf(buff,"%d.%d %%", i.cpu/10, i.cpu%10);
        mp.insert("cpu", QString(buff));
        ls.push_back(mp);
    }
    if(by=="name") {
        std::sort(ls.begin(), ls.end(), [](QVariantMap a, QVariantMap b) { return (a.find("name")->toString() < b.find("name")->toString()); });
    }
    for(auto i: ls)
        ret.append(i);

    return ret;
}

int ps::kill(int pid, int signal) {
    char buff[64];
    if((pid > 0) && (pid!=getpid())) {
        sprintf(buff,"kill -%d %d", signal, pid);
        printf("kill -%d %d\n$? = %d", signal, pid, system(buff));
    }
    return 0;
}

QString ps::load_avg() {
    char buff[24];
    char *ptr = buff;
    int cr;
    FILE* fl = NULL;

    fl = fopen("/proc/loadavg","r");
    while(((cr = fgetc(fl)) != EOF) && ((isdigit(cr) || (cr=='.'))))
        (*(ptr++))=cr;
    (*ptr) = 0;
    fclose(fl);
    return buff;
}

int main(int argc, char *argv[])
{
    qmlRegisterType<ps>("harbour.crest.ps", 1, 0, "PS");
    return SailfishApp::main(argc, argv);
}

