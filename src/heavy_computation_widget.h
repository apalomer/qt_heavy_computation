#ifndef HEAVY_COMPUTATION_WIDGET_H
#define HEAVY_COMPUTATION_WIDGET_H

#include <QProgressDialog>
#include <QWidget>

#include "heavy_computation_object.h"

#ifdef _WIN32
#ifdef QTDOULESPINBOXHEAVYCOMPUTATION
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#endif

namespace Ui
{
class HeavyComputationWidget;
}

class EXPORT HeavyComputationWidget : public QWidget
{
  Q_OBJECT

public:
  explicit HeavyComputationWidget(QWidget *parent = nullptr);
  ~HeavyComputationWidget() override;

signals:

  void requestComputation(int value, QProgressDialog *progress);
private slots:
  void startHeavyComputationInThread();

  void startHeavyComputationConcurrent();

  void onComputationDone(int result);

private:
  QProgressDialog *createProgressDialog();
  Ui::HeavyComputationWidget *ui = nullptr;

  QThread *thread_ = nullptr;
  HeavyComputationObject *worker_ = nullptr;
  HeavyComputationObject *concurrent_ = nullptr;
};

#endif  // HEAVY_COMPUTATION_WIDGET_H
