#ifndef CREST_H
#define CREST_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <vector>

struct proc {
    int cpu;
    int rss;
    QString proc_name;
};

class ps : public QObject {
    Q_OBJECT
    std::vector<proc> get_ps();
public:
    explicit ps(QObject* parent = 0) : QObject(parent) {}
    ~ps() {}
    Q_INVOKABLE QVariantList get_ps_by(QString by, bool only_gui=false);
};

#endif // CREST_H
