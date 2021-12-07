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
#include <dirent.h>
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
#include <cmath>
#include <iostream>
#include <string>

#define readc(a, b) (read(a, &b, 1))
#define LOC_MAXLEN 5048

QStringList installedApps;

int check_if_number(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int gettimesinceboot() {
    long Hertz = sysconf(_SC_CLK_TCK);
    FILE *procuptime;
    int sec, ssec;

    procuptime = fopen("/proc/uptime", "r");
    fscanf(procuptime, "%d.%ds", &sec, &ssec);
    fclose(procuptime);
    return (int)((sec * Hertz) + ssec);
}

std::vector<proc> ps::get_ps() {
    std::vector<proc> running;
    proc pr;

    DIR *dirp;
    FILE *fp;
    struct dirent *pid_entry;
    char path[1024], read_buf[1024], read_buf_full[5048];
    char uid_int_str[6] = {0}, *line;
    char uptime_str[10];
    char *user;
    size_t len = 0;
    dirp = opendir("/proc/");
    if (dirp == NULL) {
        perror("proc error");
        exit(0);
    }
    strcpy(path, "/proc/");
    strcat(path, "uptime");

    fp = fopen(path, "r");
    if (fp != NULL) {
        getline(&line, &len, fp);
        sscanf(line, "%s ", uptime_str);
        fclose(fp);
    }

    long uptime = atof(uptime_str);
    long Hertz = sysconf(_SC_CLK_TCK);

    strcpy(path, "/proc/");
    strcat(path, "meminfo");

    fp = fopen(path, "r");
    unsigned long long total_memory;
    if (fp != NULL) {
        getline(&line, &len, fp);
        sscanf(line, "MemTotal:        %llu kB", &total_memory);
        fclose(fp);
    }

    while ((pid_entry = readdir(dirp)) != NULL) {
        if (check_if_number(pid_entry->d_name)) {
            // we only handle pid dirs
            strcpy(path, "/proc/");
            strcat(path, pid_entry->d_name);
            strcat(path, "/status");
            unsigned long long vm_rss;
            fp = fopen(path, "r");
            unsigned long long vm_size;

            vm_size = 0;
            vm_rss = 0;
            if (fp != NULL) {
                char myline[1000];
                while (fgets(myline, sizeof myline, fp) != NULL) {
                    if ((strstr(myline, "Uid:")) != NULL) {
                        sscanf(myline, "Uid:    %s ", uid_int_str);
                    }
                    if ((strstr(myline, "VmSize:")) != NULL) {
                        sscanf(myline, "VmSize:    %llu kB", &vm_size);
                    }
                    if ((strstr(myline, "VmRSS:")) != NULL) {
                        sscanf(myline, "VmRSS:\t    %llu kB", &vm_rss);
                    }
                }
                fclose(fp);
            } else {
                fprintf(stdout, "FP is NULL, proc has gone?\n");
                continue;
            }
            // int current_user = getuid();
            float memory_usage = (100.0 * vm_rss) / total_memory;
            strcpy(path, "/proc/");
            strcat(path, pid_entry->d_name);
            strcat(path, "/comm");

            fp = fopen(path, "r");
            if (fp != NULL) {
                fscanf(fp, "%s", read_buf);
                fclose(fp);
            }
            strcpy(path, "/proc/");
            strcat(path, pid_entry->d_name);
            strcat(path, "/cmdline");

            fp = fopen(path, "r");
            if (fp != NULL) {
                int c;
                strcpy(read_buf_full, "");
                while ((c = getc(fp)) != EOF) {
                    // cmdline is \0 separated regarding parameters
                    if (c == '\0') {
                        c = ' ';
                    }
                    snprintf(read_buf_full + strlen(read_buf_full),
                             LOC_MAXLEN - strlen(read_buf_full), "%c",
                             (char)putchar(c));
                    printf("\b");  // remove character from stdout
                }
                fclose(fp);
                if (strlen(read_buf_full) == 0) {
                    strcpy(read_buf_full, "[");
                    strcat(read_buf_full, read_buf);
                    strcat(read_buf_full, "]");
                }
            }
            strcpy(path, "/proc/");
            strcat(path, pid_entry->d_name);
            strcat(path, "/stat");
            fp = fopen(path, "r");
            getline(&line, &len, fp);
            char state;
            unsigned int flags;
            int ppid, pgrp, session, tty_nr, tpgid;
            unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
            unsigned long long starttime;
            long cutime, cstime, priority, nice, num_threads, itreavalue;
            int skip =
                (int)strlen(pid_entry->d_name) + (int)strlen(read_buf) + 4;
            sscanf(&line[skip],
                   "%c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld "
                   "%ld %ld  %ld %llu",
                   &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags,
                   &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime,
                   &cutime, &cstime, &priority, &nice, &num_threads,
                   &itreavalue, &starttime);
            unsigned long total_time = utime + stime;
            total_time =
                total_time + (unsigned long)cutime + (unsigned long)cstime;
            float seconds = uptime - (starttime / Hertz);
            float cpu_usage = 1000 * ((total_time / Hertz) / seconds);
            if (std::isnan(cpu_usage))  // if entry is missing in proc
            {
                cpu_usage = 0.0;
            }
            if (std::isnan(memory_usage))  // if entry is missing in proc
            {
                memory_usage = 0.0;
            }

            fclose(fp);
            user = uid_int_str;
            int sinceboot = gettimesinceboot();
            int sec_running = (int)(sinceboot - starttime);
            time_t rt = time(NULL) - (sec_running / Hertz);
            char start_time[1024];

            strftime(start_time, sizeof(start_time), "%H:%M", localtime(&rt));

            // fprintf(stdout, "%6s %8s %10.1f %7llu %10.1f %6i %6i %7llu %5c
            // %10s %s\n",
            //         pid_entry->d_name, user, cpu_usage, vm_rss, memory_usage,
            //         ppid, tpgid, vm_size, state, start_time, read_buf_full);
            // printf("Start desect result\n");
            pr.cpu = cpu_usage;
            pr.pid = atoi(pid_entry->d_name);
            pr.ppid = ppid;
            pr.uid = atoi(user);
            pr.gid = tpgid;
            pr.rss = vm_rss;
            pr.cputime = "00:00:00";
            pr.start_time = start_time;
            pr.mem_perc = QString::number(memory_usage, 'g', 2);
            pr.proc_name = read_buf_full;
            running.push_back(pr);
        }
    }
    closedir(dirp);
    return running;
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
            snprintf(buff, sizeof(buff), "%d KB", i.rss);
        } else if (i.rss < 2048) {
            snprintf(buff, sizeof(buff), "%d.%03d KB", i.rss / 1000,
                     i.rss % 1000);
        } else {
            int tmp = (i.rss * 100) / 1000;
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

int ps::killproc(int pid, int signal, int userid) {
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
    int n = uptime;
    int days = n / (24 * 3600);
    n = n % (24 * 3600);
    int hours = n / 3600;
    n %= 3600;
    int minutes = n / 60;
    n %= 60;
    int seconds = n;
    sz = snprintf(NULL, 0, "%lds/%ldhr\n\%dd %dh %dm %ds", uptime,
                  uptime / 3600, days, hours, minutes, seconds);
    buf = (char *)malloc(sz +
                         1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz + 1, "%lds/%ldhr\n\%dd %dh %dm %ds", uptime, uptime / 3600,
             days, hours, minutes, seconds);

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
