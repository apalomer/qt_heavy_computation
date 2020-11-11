#ifndef HEAVYCOMPUTATIONOBJECT_H
#define HEAVYCOMPUTATIONOBJECT_H

#include <QMutex>
#include <QObject>
#include <QProgressDialog>

class HeavyComputationObject : public QObject
{
  Q_OBJECT
public:
  static HeavyComputationObject *createHeavyComputationObjectInThread(QThread *thread);

  explicit HeavyComputationObject(QObject *parent = nullptr);
  ~HeavyComputationObject();

  bool isBusy() const;
signals:
  void computationInit();
  void computationDone(int result);
  void setProgressDialogText(QString text, QPrivateSignal);
  void setProgressDialogMinimum(int minimum, QPrivateSignal);
  void setProgressDialogMaximum(int maximum, QPrivateSignal);
  void setProgressDialogValue(int value, QPrivateSignal);
  void setProgressTitle(QString text, QPrivateSignal);
  void cancelled();

public slots:

  void heavyComputation(int value, QProgressDialog *progress);
  void connectProgress(QProgressDialog *progress);
  void heavyComputation(int value);
  void cancel();

protected slots:
  void disconnectAllSavedConnections();

protected:
  std::vector<QMetaObject::Connection> connections_;

  std::atomic<bool> cancelled_;

  std::atomic<bool> is_busy_;

  QMutex mutex_;

  int n_ = 0;
};

#endif  // HEAVYCOMPUTATIONOBJECT_H
