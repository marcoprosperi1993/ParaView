/*******************************************************************/
/*                               XDMF                              */
/*                   eXtensible Data Model and Format              */
/*                                                                 */
/*  Id : $Id$  */
/*  Date : $Date$ */
/*  Version : $Revision$ */
/*                                                                 */
/*  Author:                                                        */
/*     Jerry A. Clarke                                             */
/*     clarke@arl.army.mil                                         */
/*     US Army Research Laboratory                                 */
/*     Aberdeen Proving Ground, MD                                 */
/*                                                                 */
/*     Copyright @ 2002 US Army Research Laboratory                */
/*     All Rights Reserved                                         */
/*     See Copyright.txt or http://www.arl.hpc.mil/ice for details */
/*                                                                 */
/*     This software is distributed WITHOUT ANY WARRANTY; without  */
/*     even the implied warranty of MERCHANTABILITY or FITNESS     */
/*     FOR A PARTICULAR PURPOSE.  See the above copyright notice   */
/*     for more information.                                       */
/*                                                                 */
/*******************************************************************/
#include "XdmfInformation.h"

XdmfInformation::XdmfInformation() {
    this->SetElementName("Information");
    this->Value = NULL;
}

XdmfInformation::~XdmfInformation() {
}

XdmfInt32
XdmfInformation::Insert( XdmfElement *Child){
    if(Child && XDMF_WORD_CMP(Child->GetElementName(), "Information")){
        return(XdmfElement::Insert(Child));
    }else{
        XdmfErrorMessage("Information can only Insert Information elements");
    }
    return(XDMF_FAIL);
}

XdmfInt32 XdmfInformation::UpdateInformation(){
    XdmfConstString Value;

    XdmfElement::UpdateInformation();
    Value = this->Get("Name");
    if(Value) this->SetName(Value);
    Value = this->Get("Value");
    if(!Value) Value = this->Get("CDATA");
    if(Value) this->SetValue(Value);
    return(XDMF_SUCCESS);
}

XdmfInt32 XdmfInformation::Build(){
    if(XdmfElement::Build() != XDMF_SUCCESS) return(XDMF_FAIL);
    // If Value isn't already an XML Attribute and
    // the value is > 10 chars, put it in the CDATA
    if((this->Get("Value") == NULL)  && (strlen(this->Value) > 10)){
        this->Set("CDATA", this->Value);
    }else{
        this->Set("Value", this->Value);
    }
    return(XDMF_SUCCESS);
}
