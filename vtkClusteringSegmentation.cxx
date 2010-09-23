#include "vtkClusteringSegmentation.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"

#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPolyData.h>
#include <vtkKdTreePointLocator.h>
#include <vtkIdList.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>

vtkStandardNewMacro(vtkClusteringSegmentation);

vtkClusteringSegmentation::vtkClusteringSegmentation()
{
  this->UseAutoRadius = true;
}

double vtkClusteringSegmentation::ComputeAutoRadius(vtkPolyData* data)
{
  double bounds[6];
  data->GetBounds(bounds);

  double delx = bounds[1] - bounds[0];
  double dely = bounds[3] - bounds[2];
  double delz = bounds[5] - bounds[4];
  //std::cout << "delx: " << delx << " dely: " << dely << " delz: " << delz << std::endl;
  double minDim = std::min(delx, std::min(dely,delz));

  this->RBNNRadius = minDim / 10.;
}

int vtkClusteringSegmentation::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  // Get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // Get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if(this->UseAutoRadius)
    {
    this->ComputeAutoRadius(input);
    }

  // Create a vector to keep track of the points that are already assigned to a superpoint
  std::vector<int> clusterLabels(input->GetNumberOfPoints(), -1);

  // Create a kd tree
  vtkSmartPointer<vtkKdTreePointLocator> kdTree =
    vtkSmartPointer<vtkKdTreePointLocator>::New();
  kdTree->SetDataSet(input);
  kdTree->BuildLocator();

  int clusterID = 0;

  for(vtkIdType pointID = 0; pointID < input->GetNumberOfPoints(); ++pointID)
    {
    if(clusterLabels[pointID] != -1)
      {
      continue;
      }

    // Find all the points around the query point
    vtkSmartPointer<vtkIdList> neighbors =
      vtkSmartPointer<vtkIdList>::New();
    kdTree->FindPointsWithinRadius(this->RBNNRadius, pointID, neighbors);

    // Label the point the same as its first neighbor with a label
    for(vtkIdType n = 0; n < neighbors->GetNumberOfIds(); n++)
      {
      if(clusterLabels[neighbors->GetId(n)] != -1)
        {
        clusterLabels[pointID] = clusterLabels[neighbors->GetId(n)];
        break;
        }
      }

    // If no neighbors already had labels, assign this point to a new cluster
    if(clusterLabels[pointID] == -1)
      {
      clusterLabels[pointID] = clusterID;
      }


    // Label all neighbors that are unlabeled the same as the current point
    for(vtkIdType n = 0; n < neighbors->GetNumberOfIds(); n++)
      {
      if(clusterLabels[neighbors->GetId(n)] == -1)
        {
        clusterLabels[neighbors->GetId(n)] = clusterLabels[pointID];
        }
      }

    // Construct a list of all neighboring point cluster IDs
    std::set<int> neighborClusters;
    neighborClusters.insert(clusterLabels[pointID]);
    for(vtkIdType n = 0; n < neighbors->GetNumberOfIds(); n++)
      {
      neighborClusters.insert(clusterLabels[neighbors->GetId(n)]);
      }

    Replace(clusterLabels, neighborClusters, clusterLabels[pointID]);

    clusterID++;

    } //end for

  clusterLabels = ReNumber(clusterLabels);

  vtkSmartPointer<vtkIntArray> labelArray =
    vtkSmartPointer<vtkIntArray>::New();
  labelArray->SetNumberOfComponents(1);

  for(unsigned int i = 0; i < clusterLabels.size(); i++)
    {
    labelArray->InsertNextValue(clusterLabels[i]);
    }

  input->GetPointData()->SetScalars(labelArray);

  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  glyphFilter->SetInputConnection(input->GetProducerPort());
  glyphFilter->Update();

  output->ShallowCopy(glyphFilter->GetOutput());

  return 1;
}


//----------------------------------------------------------------------------
void vtkClusteringSegmentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


void Replace(std::vector<int> &input, std::set<int> toReplace, int replacement)
{
  for(unsigned int i = 0; i < input.size(); i++)
    {
    for(std::set<int>::iterator it1 = toReplace.begin(); it1 != toReplace.end(); it1++)
      {
      if(input[i] == *it1)
        {
        input[i] = replacement;
        }
      }
    }
}

void Replace(std::vector<int> &input, int toReplace, int replacement)
{
  for(unsigned int i = 0; i < input.size(); i++)
    {
    if(input[i] == toReplace)
      {
      input[i] = replacement;
      }
    }
}

std::vector<int> ReNumber(std::vector<int> input)
{
  // Create a list of all of the numbers that are used
  std::set<int> usedNumbers;
  for(unsigned int i = 0; i < input.size(); i++)
    {
    usedNumbers.insert(input[i]);
    }

  // Replace each number with sequential numbers starting at 0
  int count = 0;
  for(std::set<int>::iterator it1 = usedNumbers.begin(); it1 != usedNumbers.end(); it1++)
    {
    Replace(input, *it1, count);
    count++;
    }

  return input;
}