#include "heavy_computation_object.h"

#include <QApplication>
#include <QDebug>
#include <QThread>

#include <thread>
HeavyComputationObject* HeavyComputationObject::createHeavyComputationObjectInThread(QObject* thread_parent)
{
  QThread* th = new QThread(thread_parent);
  HeavyComputationObject* heavy_comp_obj = new HeavyComputationObject;
  heavy_comp_obj->moveToThread(th);
  connect(heavy_comp_obj, &QObject::objectNameChanged, th, &QThread::setObjectName);
  th->start();
  return heavy_comp_obj;
}

HeavyComputationObject::HeavyComputationObject(QObject* parent) : QObject(parent), cancelled_(false), is_busy_(false)
{
}

void HeavyComputationObject::connectProgress(QProgressDialog* progress)
{
  if (!progress)
    return;
  connections_.push_back(connect(this, &HeavyComputationObject::computationInit, progress, &QProgressDialog::show));
  connections_.push_back(connect(this, &HeavyComputationObject::computationDone, progress, &QProgressDialog::close));
  connections_.push_back(
      connect(progress, &QProgressDialog::canceled, this, &HeavyComputationObject::cancel, Qt::DirectConnection));
  connections_.push_back(
      connect(this, &HeavyComputationObject::setProgressDialogText, progress, &QProgressDialog::setLabelText));
  connections_.push_back(
      connect(this, &HeavyComputationObject::setProgressTitle, progress, &QProgressDialog::setWindowTitle));
  connections_.push_back(
      connect(this, &HeavyComputationObject::setProgressDialogMinimum, progress, &QProgressDialog::setMinimum));
  connections_.push_back(
      connect(this, &HeavyComputationObject::setProgressDialogMaximum, progress, &QProgressDialog::setMaximum));
  connections_.push_back(
      connect(this, &HeavyComputationObject::setProgressDialogValue, progress, &QProgressDialog::setValue));
  connections_.push_back(
      connect(this, &HeavyComputationObject::computationDone, progress, &QProgressDialog::deleteLater));
}

void HeavyComputationObject::cancel()
{
  qDebug() << objectName() << " " << QString::number(n_) << " cancel requested!";
  if (!cancelled_)
    cancelled_ = true;
}

void HeavyComputationObject::heavyComputation(int value, QProgressDialog* progress)
{
  n_++;
  connectProgress(progress);
  heavyComputation(value);
}

void HeavyComputationObject::heavyComputation(int value)
{
  is_busy_ = true;
  cancelled_ = false;
  emit computationInit();
  emit setProgressDialogText("Computing in thread", QPrivateSignal());
  emit setProgressDialogMinimum(0, QPrivateSignal());
  emit setProgressDialogMaximum(value - 1, QPrivateSignal());
  qDebug() << objectName() << " " << QString::number(n_) << " heavy computation thread: " << QThread::currentThread();
  qDebug() << objectName() << " " << QString::number(n_) << " app thread: " << qApp->thread();
  for (int i = 0; i < value && !cancelled_; i++)
  {
    emit setProgressDialogValue(i, QPrivateSignal());
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  if (!cancelled_)
  {
    qDebug() << objectName() << " " << QString::number(n_) << " emit computation done";
    emit computationDone(value + 1);
  }
  else
  {
    qDebug() << objectName() << " " << QString::number(n_) << " emit cancelled";
    emit cancelled();
  }
  disconnectAllSavedConnections();
  is_busy_ = false;
}

void HeavyComputationObject::disconnectAllSavedConnections()
{
  qDebug() << objectName() << " " << QString::number(n_) << " disconnecting all connections.";
  for (auto connection : connections_)
    disconnect(connection);
  connections_.clear();
}

bool HeavyComputationObject::isBusy() const
{
  return is_busy_;
}
