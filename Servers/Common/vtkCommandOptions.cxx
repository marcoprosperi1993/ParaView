/*=========================================================================

  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCommandOptions.h"
#include "vtkCommandOptionsXMLParser.h"
#include "vtkObjectFactory.h"
#include "vtkPVConfig.h" //For PARAVIEW_ALWAYS_SECURE_CONNECTION option

#include <vtksys/CommandLineArguments.hxx>
#include <vtksys/SystemTools.hxx>


//----------------------------------------------------------------------------
//****************************************************************************
class vtkCommandOptionsInternal
{
public:
  vtkCommandOptionsInternal(vtkCommandOptions* p)
    {
      this->XMLParser = vtkCommandOptionsXMLParser::New();
      this->XMLParser->SetPVOptions(p);
    }
  ~vtkCommandOptionsInternal()
    {
      this->XMLParser->Delete();
    }
  vtkCommandOptionsXMLParser* XMLParser;
  vtksys::CommandLineArguments CMD;
};
//****************************************************************************
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkCommandOptions);
vtkCxxRevisionMacro(vtkCommandOptions, "$Revision$");

//----------------------------------------------------------------------------
vtkCommandOptions::vtkCommandOptions()
{
  this->ProcessType = EVERYBODY;
  // Initialize vtksys::CommandLineArguments
  this->Internals = new vtkCommandOptionsInternal(this);
  this->Internals->CMD.SetUnknownArgumentCallback(vtkCommandOptions::UnknownArgumentHandler);
  this->Internals->CMD.SetClientData(this);
  this->UnknownArgument = 0;
  this->HelpSelected = 0;
  
  this->ErrorMessage = 0;
  this->Argc = 0;
  this->Argv = 0;

  this->XMLConfigFile = 0;
}

//----------------------------------------------------------------------------
vtkCommandOptions::~vtkCommandOptions()
{
  this->SetXMLConfigFile(0);
  
  // Remove internals
  this->SetUnknownArgument(0);
  this->SetErrorMessage(0);
  this->CleanArgcArgv();
  delete this->Internals;
}

//----------------------------------------------------------------------------
const char* vtkCommandOptions::GetHelp()
{
  int width = vtksys::SystemTools::GetTerminalWidth();
  if ( width < 9 )
    {
    width = 80;
    }

  this->Internals->CMD.SetLineLength(width);
  this->Internals->CMD.SetLineLength(300);

  return this->Internals->CMD.GetHelp();
}

//----------------------------------------------------------------------------
void vtkCommandOptions::Initialize()
{
}

//----------------------------------------------------------------------------
int vtkCommandOptions::PostProcess(int, const char* const*)
{
  return 1;
}

//----------------------------------------------------------------------------
int vtkCommandOptions::WrongArgument(const char* argument)
{
  // if the unknown file is a config file then it is OK
  if(this->XMLConfigFile && strcmp(argument, this->XMLConfigFile) == 0)
    {
    // if the UnknownArgument is the XMLConfigFile then set the 
    // UnknownArgument to null as it really is not Unknown anymore.
    if(this->UnknownArgument && 
       (strcmp(this->UnknownArgument, this->XMLConfigFile) == 0))
      {
      this->SetUnknownArgument(0);
      }
    return 1;
    }

  return 0;
}

//----------------------------------------------------------------------------
const char* vtkCommandOptions::GetArgv0()
{
  return this->Internals->CMD.GetArgv0();
}

//----------------------------------------------------------------------------
int vtkCommandOptions::LoadXMLConfigFile(const char* fname)
{
  this->Internals->XMLParser->SetFileName(fname);
  this->Internals->XMLParser->Parse();
  this->SetXMLConfigFile(fname);
  return 1;
}

//----------------------------------------------------------------------------
int vtkCommandOptions::Parse(int argc, const char* const argv[])
{
  this->Internals->CMD.Initialize(argc, argv);
  this->Initialize();
  this->AddBooleanArgument("--help", "/?", &this->HelpSelected, 
                           "Displays available command line arguments.",
                           vtkCommandOptions::EVERYBODY);

  // First get options from the xml file
  for(int i =0; i < argc; ++i)
    {
    vtkstd::string arg = argv[i];
    if(arg.size() > 4 && arg.find(".pvx") == (arg.size() -4))
      {
      if(!this->LoadXMLConfigFile(arg.c_str()))
        {
        return 0;
        }
      }
    }
  // now get options from the command line
  int res1 = this->Internals->CMD.Parse();
  int res2 = this->PostProcess(argc, argv);
  //cout << "Res1: " << res1 << " Res2: " << res2 << endl;
  this->CleanArgcArgv();
  this->Internals->CMD.GetRemainingArguments(&this->Argc, &this->Argv);

  return res1 && res2;
}

//----------------------------------------------------------------------------
void vtkCommandOptions::CleanArgcArgv()
{
  int cc;
  if ( this->Argv )
    {
    for ( cc = 0; cc < this->Argc; cc ++ )
      {
      delete [] this->Argv[cc];
      }
    delete [] this->Argv;
    this->Argv = 0;
    }
}
//----------------------------------------------------------------------------
void vtkCommandOptions::AddDeprecatedArgument(const char* longarg, const char* shortarg,
                                         const char* help, int type)
{
  // if it is for xml or not for the current process do nothing
  if((type == XMLONLY) || !(type & this->ProcessType))
    {
    return;
    }
  // Add a callback for the deprecated argument handling
  this->Internals->CMD.AddCallback(longarg, vtksys::CommandLineArguments::NO_ARGUMENT,
                                   vtkCommandOptions::DeprecatedArgumentHandler, this, help);
  if(shortarg)
    {
    this->Internals->CMD.AddCallback(shortarg, vtksys::CommandLineArguments::NO_ARGUMENT,
                                     vtkCommandOptions::DeprecatedArgumentHandler, this, help);
    }
}

//----------------------------------------------------------------------------
int vtkCommandOptions::DeprecatedArgument(const char* argument)
{
  ostrstream str;
  str << "  " << this->Internals->CMD.GetHelp(argument);
  str << ends;
  this->SetErrorMessage(str.str());
  delete [] str.str();
  return 0;
}


//----------------------------------------------------------------------------
void vtkCommandOptions::AddBooleanArgument(const char* longarg, const char* shortarg,
                                      int* var, const char* help, int type)
{
  // add the argument to the XML parser
  this->Internals->XMLParser->AddBooleanArgument(longarg, var, type);
  if(type == XMLONLY)
    {
    return;
    }
  // if the process type matches then add the argument to the command line
  if(type & this->ProcessType || type == vtkCommandOptions::EVERYBODY)
    {
    this->Internals->CMD.AddBooleanArgument(longarg, var, help);
    if ( shortarg )
      {
      this->Internals->CMD.AddBooleanArgument(shortarg, var, longarg);
      }
    }
}

//----------------------------------------------------------------------------
void vtkCommandOptions::AddArgument(const char* longarg, const char* shortarg, int* var, const char* help, int type)
{
  this->Internals->XMLParser->AddArgument(longarg, var, type);
  if(type == XMLONLY)
    {
    return;
    }
  if(type & this->ProcessType || type == vtkCommandOptions::EVERYBODY)
    {
    typedef vtksys::CommandLineArguments argT;
    this->Internals->CMD.AddArgument(longarg, argT::EQUAL_ARGUMENT, var, help);
    if ( shortarg )
      {
      this->Internals->CMD.AddArgument(shortarg, argT::EQUAL_ARGUMENT, var, longarg);
      }
    }
}

//----------------------------------------------------------------------------
void vtkCommandOptions::AddArgument(const char* longarg, const char* shortarg, char** var, const char* help, int type)
{
  this->Internals->XMLParser->AddArgument(longarg, var, type);
  if(type == XMLONLY)
    {
    return;
    }
  if(type & this->ProcessType || type == vtkCommandOptions::EVERYBODY)
    {
    typedef vtksys::CommandLineArguments argT;
    this->Internals->CMD.AddArgument(longarg, argT::EQUAL_ARGUMENT, var, help);
    if ( shortarg )
      {
      this->Internals->CMD.AddArgument(shortarg, argT::EQUAL_ARGUMENT, var, longarg);
      }
    }
}

//----------------------------------------------------------------------------
int vtkCommandOptions::UnknownArgumentHandler(const char* argument, void* call_data)
{
  vtkCommandOptions* self = static_cast<vtkCommandOptions*>(call_data);
  if ( self )
    {
    self->SetUnknownArgument(argument);
    return self->WrongArgument(argument);
    }
  return 0;
}

//----------------------------------------------------------------------------
int vtkCommandOptions::DeprecatedArgumentHandler(const char* argument, 
                                            const char* , void* call_data)
{
  //cout << "UnknownArgumentHandler: " << argument << endl;
  vtkCommandOptions* self = static_cast<vtkCommandOptions*>(call_data);
  if ( self )
    {
    return self->DeprecatedArgument(argument);
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkCommandOptions::GetRemainingArguments(int* argc, char*** argv)
{
  *argc = this->Argc;
  *argv = this->Argv;
}

//----------------------------------------------------------------------------
int vtkCommandOptions::GetLastArgument()
{
  return this->Internals->CMD.GetLastArgument();
}

//----------------------------------------------------------------------------
void vtkCommandOptions::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "XMLConfigFile: " << (this->XMLConfigFile?this->XMLConfigFile:"(none)") << endl;
  os << indent << "UnknownArgument: " << (this->UnknownArgument?this->UnknownArgument:"(none)") << endl;
  os << indent << "ErrorMessage: " << (this->ErrorMessage?this->ErrorMessage:"(none)") << endl;
  os << indent << "HelpSelected: " << this->HelpSelected << endl;
}
