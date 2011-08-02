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
