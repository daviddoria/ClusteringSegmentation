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

#include <sstream>

int main(int argc, char*argv[])
{
  // Verify arguments
  if(argc < 4)
    {
    std::cerr << "Required: input.vtp radius output.vtp" << std::endl;
    return EXIT_FAILURE;
    }
  
  // Parse arguments
  std::string inputFileName = argv[1];
  
  float radius = 1.0;
  std::stringstream ss;
  ss << argv[2];
  ss >> radius;
  
  std::string outputFileName = argv[3];
  
  // Output arguments
  std::cout << "Input: " << inputFileName << std::endl;
  std::cout << "Radius: " << radius << std::endl;
  std::cout << "Output: " << outputFileName << std::endl;
  
  vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(inputFileName.c_str());
  reader->Update();

  vtkSmartPointer<vtkClusteringSegmentation> clusteringSegmentation =
    vtkSmartPointer<vtkClusteringSegmentation>::New();
  clusteringSegmentation->SetInputConnection(reader->GetOutputPort());
  clusteringSegmentation->SetUseAutoRadius(false);
  clusteringSegmentation->SetRBNNRadius(radius);
  clusteringSegmentation->Update();

  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
    vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(outputFileName.c_str());
  writer->SetInputConnection(clusteringSegmentation->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}
