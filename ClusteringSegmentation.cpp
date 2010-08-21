#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkKdTreePointLocator.h>
#include <vtkIdList.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkAppendPolyData.h>

#include <set>

std::vector<int> ReNumber(std::vector<int> input);
void Replace(std::vector<int> &input, int toReplace, int replacement);
void Replace(std::vector<int> &input, std::set<int> toReplace, int replacement);

int main(int argc, char* argv[])
{
  /*
  vtkSmartPointer<vtkXMLPolyDataReader> reader =
      vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(argv[1]);
  reader->Update();
  */

  vtkSmartPointer<vtkSphereSource> sphereSource0 =
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource0->SetThetaResolution(20);
  sphereSource0->SetPhiResolution(20);
  sphereSource0->Update();

  vtkSmartPointer<vtkSphereSource> sphereSource1 =
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource1->SetThetaResolution(20);
  sphereSource1->SetPhiResolution(20);
  sphereSource1->SetCenter(5,0,0);
  sphereSource1->Update();

  vtkSmartPointer<vtkAppendPolyData> appendFilter =
    vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputConnection(sphereSource0->GetOutputPort());
  appendFilter->AddInputConnection(sphereSource1->GetOutputPort());
  appendFilter->Update();

  vtkSmartPointer<vtkPolyData> input =
    vtkSmartPointer<vtkPolyData>::New();
  input->ShallowCopy(appendFilter->GetOutput());

  //decide on a reasonable e-sphere size
  double bounds[6];
  input->GetBounds(bounds);

  double delx = bounds[1] - bounds[0];
  double dely = bounds[3] - bounds[2];
  double delz = bounds[5] - bounds[4];
  std::cout << "delx: " << delx << " dely: " << dely << " delz: " << delz << std::endl;
  double minDim = std::min(delx, std::min(dely,delz));

  //double eRadius = minDim / 10.;
  double eRadius = minDim / 3.;
  std::cout << "eRadius: " << eRadius << std::endl;

  //create a vector to keep track of the points that are already assigned to a superpoint
  std::vector<int> clusterLabels(input->GetNumberOfPoints(), -1);

  //Create a kd tree
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

    //find all the points around the query point
    vtkSmartPointer<vtkIdList> neighbors =
      vtkSmartPointer<vtkIdList>::New();
    kdTree->FindPointsWithinRadius(eRadius, pointID, neighbors);

    // Label the point the same as its first neighbor with a label
    for(vtkIdType n = 0; n < neighbors->GetNumberOfIds(); n++)
      {
      if(clusterLabels[neighbors->GetId(n)] != -1)
        {
        clusterLabels[pointID] = clusterLabels[neighbors->GetId(n)];
        break;
        }
      }

    //if no neighbors already had labels, assign this point to a new cluster
    if(clusterLabels[pointID] == -1)
      {
      clusterLabels[pointID] = clusterID;
      }


    //label all neighbors that are unlabeled the same as the current point
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

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
      vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("/home/doriad/Desktop/clusters.vtp");
  writer->SetInputConnection(glyphFilter->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
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