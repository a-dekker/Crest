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

#include "crest.h"

#include <ctype.h>
#include <sailfishapp.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <QDirIterator>
#include <QRegExp>
#include <QVariant>
#include <QtQuick>
#include <algorithm>
#include <iostream>
#include <string>

#define readc(a, b) (read(a, &b, 1))

QStringList installedApps;

std::vector<proc> ps::get_ps() {
    int pp[2], i, a, b;
    char tmp;
    char buff[129];
    std::vector<proc> running;
    proc pr;
    pid_t ch_pid;

    if (pipe(pp) == -1) return running;
    if (!sys_check()) return running;
    if ((ch_pid = fork()) == 0) {
        close(1);
        close(pp[0]);
        dup2(pp[1], 1);
        execlp("ps", "ps", "axo",
               "pid,uid,pcpu,rss,time,%mem,ppid,gid,start_time,args", NULL);
        return running;
    } else {
        close(pp[1]);
        waitpid(ch_pid, NULL, 0);
        if (readc(pp[0], tmp) < 1) tmp = 0;
        while (tmp != 0) {
            pr.cpu = 0;
            pr.pid = 0;
            pr.ppid = 0;
            pr.uid = 0;
            pr.gid = 0;
            pr.rss = 0;
            pr.cputime = "00:00:00";
            pr.start_time = "";
            pr.proc_name = "";
            pr.mem_perc = "0.0";
            // Space
            while (!isdigit(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            i = 0;
            // PID
            while (isdigit(tmp)) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                sscanf(buff, "%d", &pr.pid);
            }
            // Space
            while (!isdigit(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            i = 0;
            // UID
            while (isdigit(tmp) || (tmp == '.')) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                sscanf(buff, "%d", &pr.uid);
            }
            // Space
            while (!isdigit(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            i = 0;
            // CPU usage
            while (isdigit(tmp) || (tmp == '.')) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                a = b = 0;
                sscanf(buff, "%d.%d", &a, &b);
                pr.cpu = a * 10 + b;
            }
            // Space
            while (!isdigit(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            i = 0;
            // Memory
            while (isdigit(tmp)) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                sscanf(buff, "%d", &(pr.rss));
            }
            // Space
            while (isspace(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            // CPU Time
            i = 0;
            while (tmp != 0 && i < 9) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                pr.cputime = buff;
            }
            // Space
            while (isspace(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            // Percentage mem
            i = 0;
            while (isdigit(tmp) || (tmp == '.')) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                pr.mem_perc = buff;
            }
            // Space
            while (isspace(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            // PPID
            i = 0;
            while (isdigit(tmp)) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                sscanf(buff, "%d", &pr.ppid);
            }
            // Space
            while (isspace(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            // GID
            i = 0;
            while (isdigit(tmp)) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                sscanf(buff, "%d", &pr.gid);
            }
            // Space
            while (isspace(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            // Start time
            i = 0;
            while (!isspace(tmp)) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                pr.start_time = buff;
            }
            // Space
            while (isspace(tmp))
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            // Process name
            i = 0;
            while (tmp != '\n' && tmp != 0 && i < 128) {
                buff[i++] = tmp;
                if (readc(pp[0], tmp) < 1) {
                    tmp = 0;
                    goto save;
                }
            }
            if (i > 0) {
                buff[i] = 0;
                pr.proc_name = buff;
            }
            // Rest
            if (i == 128)
                while (tmp != '\n')
                    if (readc(pp[0], tmp) < 1) {
                        tmp = 0;
                        goto save;
                    }
        save:
            if (pr.proc_name.length() > 1) {
                running.push_back(pr);
            }
        }
        close(pp[0]);
        return running;
    }
}

using std::string;

QString getFileName(const QString &s) {
    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    QString firstWord = s.split(" ").at(0);
    string firstw = firstWord.toStdString();
    size_t i = firstw.rfind(sep, firstw.length());
    if (i != string::npos) {
        QString newWord =
            QString::fromStdString((firstw.substr(i + 1, firstw.length() - i)));
        QString s_nopath = s;
        s_nopath = s_nopath.replace(firstWord, newWord);
        return (s_nopath);
    }
    return (s);
}

bool ps::sys_check() {
    if (system("test -x \"`which ps`\"") != 0) {
        return false;
    }
    return true;
}

QVariantList ps::get_ps_by(QString by, QString list_type) {
    QVariantList ls;
    static char buff[16];
    QVariantMap mp;
    std::vector<proc> procs = get_ps();

    if (by == "cpu") {
        std::sort(procs.begin(), procs.end(),
                  [](proc a, proc b) { return a.cpu > b.cpu; });
    } else if (by == "rss") {
        std::sort(procs.begin(), procs.end(),
                  [](proc a, proc b) { return a.rss > b.rss; });
    } else if (by == "name") {
        std::sort(procs.begin(), procs.end(),
                  [](proc a, proc b) { return a.proc_name < b.proc_name; });
    }

    int loopcount = 0;
    for (auto i : procs) {
        loopcount++;
        mp.clear();
        if (list_type == "gui_only") {
            if (installedApps.contains(
                    getFileName(i.proc_name).split(" ").at(0))) {
                mp.insert("name", i.proc_name);
                mp.insert("name_nopath", getFileName(i.proc_name));
            } else {
                continue;
            }
        } else if (list_type == "all_procs") {
            if (i.proc_name[0] == '[') continue;
            mp.insert("name", i.proc_name);
            mp.insert("name_nopath", getFileName(i.proc_name));
        } else {
            mp.insert("name", i.proc_name);
            if (i.proc_name[0] == '[') {
                mp.insert("name_nopath", i.proc_name);
            } else {
                mp.insert("name_nopath", getFileName(i.proc_name));
            }
        }
        if (i.rss < 999) {
            snprintf(buff, sizeof(buff), "%d kB", i.rss);
        } else if (i.rss < 2048) {
            snprintf(buff, sizeof(buff), "%d %3d kB", i.rss / 1000,
                     i.rss % 1000);
        } else {
            int tmp = (i.rss * 100) / 1024;
            snprintf(buff, sizeof(buff), "%d.%2d MB", tmp / 100, tmp % 100);
        }
        mp.insert("pid", i.pid);
        mp.insert("ppid", i.ppid);
        mp.insert("proc_owner", i.uid);
        mp.insert("gid", i.gid);
        mp.insert("rss", QString(buff));
        snprintf(buff, sizeof(buff), "%d.%d %%", i.cpu / 10, i.cpu % 10);
        mp.insert("cpu", QString(buff));
        mp.insert("cputime", i.cputime);
        mp.insert("start_time", i.start_time);
        mp.insert("mem_perc", i.mem_perc);
        ls.append(mp);
        if (loopcount == 60 && list_type == "incl_nocmd") {
            break;
        }
    }
    return ls;
}

int ps::kill(int pid, int signal, int userid) {
    char buff[64];
    char sudoCmd[14];
    int uid = getuid();
    if (userid == uid) {
        snprintf(sudoCmd, sizeof(sudoCmd), "%s", "");
    } else {
        snprintf(sudoCmd, sizeof(sudoCmd), "/usr/bin/sudo");
    }
    if ((pid > 0) && (pid != getpid())) {
        snprintf(buff, sizeof(buff), "%s /bin/kill -%d %d", sudoCmd, signal,
                 pid);
        printf("%s /bin/kill -%d %d\n$? = %d\n", sudoCmd, signal, pid,
               system(buff));
    }
    return 0;
}

bool ps::hasSudo() {
    if (access("/usr/bin/sudo", F_OK) != -1) {
        int status = system("/usr/bin/sudo -nv");
        if (status == 0) return true;
    }
    return false;
}

QString ps::load_avg() {
    char buff[24];
    char *ptr = buff;
    int cr;
    FILE *fl = NULL;

    fl = fopen("/proc/loadavg", "r");
    while (((cr = fgetc(fl)) != EOF) && ((isdigit(cr) || (cr == '.'))))
        (*(ptr++)) = cr;
    (*ptr) = 0;
    fclose(fl);
    return buff;
}

QString ps::uptime() {
    char uptime_chr[28];
    FILE *fl = NULL;
    long uptime;

    fl = fopen("/proc/uptime", "r");
    if (fgets(uptime_chr, 12, fl) == NULL)
        qDebug() << "Error reading /proc/uptime";
    fclose(fl);
    uptime = strtol(uptime_chr, NULL, 10);
    char *buf;
    size_t sz;
    sz = snprintf(NULL, 0, "%ld secs, %ld hours\n", uptime, uptime / 3600);
    buf = (char *)malloc(sz +
                         1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz + 1, "%lds/%ldhr\n", uptime, uptime / 3600);

    return buf;
}

QStringList getAppList() {
    QStringList listOfApps;
    QString baseName;
    QDirIterator it("/usr/share/applications", QStringList() << "*.desktop",
                    QDir::Files);
    while (it.hasNext()) {
        it.next();
        baseName = it.fileName();
        if (baseName.startsWith(
                "apkd_launcher")) {  // android name needs cleanup
            baseName = baseName.replace("apkd_launcher_", "");
            const int n = baseName.indexOf('-');
            baseName = baseName.remove(n, baseName.length() - n);
            baseName = baseName.replace("_", ".");
        } else {  // sailfish just has .desktop at the end
            baseName = baseName.replace(".desktop", "");
        }
        listOfApps << "sailfish-qml";
        listOfApps << baseName;
    }
    return listOfApps;
}

int main(int argc, char *argv[]) {
    installedApps = getAppList();
    qmlRegisterType<ps>("harbour.crest.ps", 1, 0, "PS");
    return SailfishApp::main(argc, argv);
}
