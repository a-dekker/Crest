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

#ifndef CREST_H
#define CREST_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <vector>

struct proc {
    int pid;
    int ppid;
    int uid;
    int gid;
    int cpu;
    int rss;
    QString cputime;
    QString proc_name;
    QString user_name;
    QString start_time;
    QString proc_name_nopath;
    QString mem_perc;
};

class ps : public QObject {
    Q_OBJECT
    std::vector<proc> get_ps();
public:
    explicit ps(QObject* parent = 0) : QObject(parent) {}
    ~ps() {}
    Q_INVOKABLE QVariantList get_ps_by(QString by, QString only_gui);
    Q_INVOKABLE QString load_avg();
    Q_INVOKABLE QString uptime();
    Q_INVOKABLE QVariantMap memory_values();
    Q_INVOKABLE bool hasSudo();
    Q_INVOKABLE int killproc(int pid, int signal, int userid);
};

#endif  // SRC_CREST_H_
