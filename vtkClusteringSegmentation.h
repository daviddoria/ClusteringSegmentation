#ifndef __vtkClusteringSegmentation_h
#define __vtkClusteringSegmentation_h

#include "vtkPolyDataAlgorithm.h"

#include <set>

class vtkClusteringSegmentation : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkClusteringSegmentation,vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkClusteringSegmentation *New();

  vtkSetMacro(RBNNRadius, double);
  vtkGetMacro(RBNNRadius, double);

  vtkSetMacro(UseAutoRadius, bool);
  vtkGetMacro(UseAutoRadius, bool);

protected:
  vtkClusteringSegmentation();
  ~vtkClusteringSegmentation(){}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  bool UseAutoRadius;
  double RBNNRadius;

  double ComputeAutoRadius(vtkPolyData* data);

private:
  vtkClusteringSegmentation(const vtkClusteringSegmentation&);  // Not implemented.
  void operator=(const vtkClusteringSegmentation&);  // Not implemented.

};


std::vector<int> ReNumber(std::vector<int> input);
void Replace(std::vector<int> &input, int toReplace, int replacement);
void Replace(std::vector<int> &input, std::set<int> toReplace, int replacement);

#endif