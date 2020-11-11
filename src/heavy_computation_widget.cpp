#include "heavy_computation_widget.h"
#include "ui_heavy_computation_widget.h"

#include <thread>

#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>

HeavyComputationWidget::HeavyComputationWidget(QWidget* parent) : QWidget(parent), ui(new Ui::HeavyComputationWidget)
{
  // Set up the UI
  ui->setupUi(this);

  // Connect the pushbuttons
  connect(ui->worker_pushButton, &QPushButton::clicked, this, &HeavyComputationWidget::startHeavyComputationInThread);
  connect(ui->concurrent_pushButton, &QPushButton::clicked, this,
          &HeavyComputationWidget::startHeavyComputationConcurrent);

  // Create thread
  thread_ = new QThread(this);
  thread_->start();

  // Get worker ready
  worker_ = HeavyComputationObject::createHeavyComputationObjectInThread(thread_);
  worker_->setObjectName("Worker");
  connect(this, &HeavyComputationWidget::requestComputation, worker_,
          QOverload<int, QProgressDialog*>::of(&HeavyComputationObject::heavyComputation));
  connect(worker_, &HeavyComputationObject::computationDone, this, &HeavyComputationWidget::onComputationDone);

  // Get concurrent ready
  concurrent_ = new HeavyComputationObject(this);
  concurrent_->setObjectName("Concurrent");
  connect(concurrent_, &HeavyComputationObject::computationDone, this, &HeavyComputationWidget::onComputationDone);
}

HeavyComputationWidget::~HeavyComputationWidget()
{
  thread_->quit();
  thread_->wait();
  delete ui;
}

void HeavyComputationWidget::startHeavyComputationInThread()
{
  if (worker_->isBusy())
  {
    QMessageBox::warning(this, "Error", "The worker is busy. Please, try later.");
    return;
  }
  emit requestComputation(ui->spinBox->value(), createProgressDialog());
}

void HeavyComputationWidget::startHeavyComputationConcurrent()
{
  QtConcurrent::run(concurrent_, &HeavyComputationObject::heavyComputation, ui->spinBox->value(),
                    createProgressDialog());
}

void HeavyComputationWidget::onComputationDone(int result)
{
  qDebug() << "Current thread: " << QThread::currentThread();
  qDebug() << "App thread: " << qApp->thread();
  Q_ASSERT(QThread::currentThread() == qApp->thread());
  ui->result_label->setNum(result);
  qDebug() << "Computation processed";
}

QProgressDialog* HeavyComputationWidget::createProgressDialog()
{
  QProgressDialog* progress = new QProgressDialog("Computing", "Cancell", 0, 0, this);
  progress->setWindowTitle(windowTitle());
  progress->setWindowFlags((progress->windowFlags() | Qt::CustomizeWindowHint) &
                           ~Qt::WindowCloseButtonHint);  // Hide close button
  progress->setWindowModality(Qt::WindowModal);
  return progress;
}
