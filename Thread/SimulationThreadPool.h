#ifndef SIMULATIONTHREADPOOL_H
#define SIMULATIONTHREADPOOL_H

#include <QObject>
#include <QVector>

class EquipmentDb;
class Random;
class QThread;

class SimulationThreadPool: public QObject {
    Q_OBJECT
public:
    SimulationThreadPool(EquipmentDb* equipment_db, QObject* parent = nullptr);
    ~SimulationThreadPool();

    void run_sim(const QString& setup_string);

public slots:
    void error_string(const QString&, const QString&);
    void result(const QString&, double);

signals:
    void thread_result(QString dps);
    void thread_setup_string(QString setup_string);

protected:
private:
    EquipmentDb* equipment_db;
    Random* random;
    int running_threads;

    QVector<QPair<unsigned, double>> thread_results;
    QVector<QPair<unsigned, QThread*>> thread_pool;

    void setup_thread(const unsigned thread_id);
    void check_threads_finished();
};

#endif // SIMULATIONTHREADPOOL_H
