/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef __vtkClusteringSegmentation_h
#define __vtkClusteringSegmentation_h

#include "vtkPolyDataAlgorithm.h"

#include <set>

class vtkKdTreePointLocator;

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

void FindNeighborsWithinRadius(vtkKdTreePointLocator* kdtree, const double radius, const vtkIdType queryPointId, vtkIdList *neighbors);

#endif
