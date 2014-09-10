/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTransferFunctionEditorRepresentationShapes1D.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTransferFunctionEditorRepresentationShapes1D.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkTransferFunctionEditorRepresentationShapes1D, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkTransferFunctionEditorRepresentationShapes1D);

//----------------------------------------------------------------------------
void vtkTransferFunctionEditorRepresentationShapes1D::BuildRepresentation()
{
}

//----------------------------------------------------------------------------
void vtkTransferFunctionEditorRepresentationShapes1D::PrintSelf(
  ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
