#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include "vtkClusteringSegmentation.h"

void CreateDemoData(vtkPolyData* data);

int main(int, char*[])
{
  vtkSmartPointer<vtkPolyData> data =
    vtkSmartPointer<vtkPolyData>::New();
  CreateDemoData(data);

  vtkSmartPointer<vtkClusteringSegmentation> clusteringSegmentation =
    vtkSmartPointer<vtkClusteringSegmentation>::New();
  clusteringSegmentation->SetInputConnection(data->GetProducerPort());
  clusteringSegmentation->SetUseAutoRadius(true);
  clusteringSegmentation->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("clusters.vtp");
  writer->SetInputConnection(clusteringSegmentation->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}

void CreateDemoData(vtkPolyData* data)
{
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

  data->ShallowCopy(appendFilter->GetOutput());
}