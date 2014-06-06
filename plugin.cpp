/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
* Version: NPL 1.1/GPL 2.0/LGPL 2.1
*
* The contents of this file are subject to the Netscape Public License
* Version 1.1 (the "License"); you may not use this file except in
* compliance with the License. You may obtain a copy of the License at
* http://www.mozilla.org/NPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is mozilla.org code.
*
* The Initial Developer of the Original Code is 
* Netscape Communications Corporation.
* Portions created by the Initial Developer are Copyright (C) 1998
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Alternatively, the contents of this file may be used under the terms of
* either the GNU General Public License Version 2 or later (the "GPL"), or 
* the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
* in which case the provisions of the GPL or the LGPL are applicable instead
* of those above. If you wish to allow use of your version of this file only
* under the terms of either the GPL or the LGPL, and not to allow others to
* use your version of this file under the terms of the NPL, indicate your
* decision by deleting the provisions above and replace them with the notice
* and other provisions required by the GPL or the LGPL. If you do not delete
* the provisions above, a recipient may use your version of this file under
* the terms of any one of the NPL, the GPL or the LGPL.
*
* ***** END LICENSE BLOCK ***** */

//////////////////////////////////////////////////
//
// CPlugin class implementation
//
#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef XP_MAC
#include <TextEdit.h>
#endif

#ifdef XP_UNIX
#include <string.h>
#endif

#include <stdio.h>
#include "plugin.h"
#include "npfunctions.h"

static NPIdentifier sFoo_id;
static NPIdentifier sBar_id;
static NPIdentifier sDocument_id;
static NPIdentifier sBody_id;
static NPIdentifier sCreateElement_id;
static NPIdentifier sCreateTextNode_id;
static NPIdentifier sAppendChild_id;
static NPIdentifier sPluginType_id;
static NPObject *sWindowObj;

// Helper class that can be used to map calls to the NPObject hooks
// into virtual methods on instances of classes that derive from this
// class.
class ScriptablePluginObjectBase : public NPObject
{
public:
    ScriptablePluginObjectBase(NPP npp)
        : mNpp(npp)
    {
    }

    virtual ~ScriptablePluginObjectBase()
    {
    }

    // Virtual NPObject hooks called through this base class. Override
    // as you see fit.
    virtual void Invalidate();
    virtual bool HasMethod(NPIdentifier name);
    virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                        uint32_t argCount, NPVariant *result);
    virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                               NPVariant *result);
    virtual bool HasProperty(NPIdentifier name);
    virtual bool GetProperty(NPIdentifier name, NPVariant *result);
    virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
    virtual bool RemoveProperty(NPIdentifier name);
    virtual bool Enumerate(NPIdentifier **identifier, uint32_t *count);
    virtual bool Construct(const NPVariant *args, uint32_t argCount,
        NPVariant *result);

public:
    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name,
        const NPVariant *args, uint32_t argCount,
        NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
        uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name,
        NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name,
        const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
    static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier,
        uint32_t *count);
    static bool _Construct(NPObject *npobj, const NPVariant *args,
        uint32_t argCount, NPVariant *result);

protected:
    NPP mNpp;
};

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                        \
    static NPClass s##_class##_NPClass = {                                        \
    NP_CLASS_STRUCT_VERSION_CTOR,                                               \
    ctor,                                                                       \
    ScriptablePluginObjectBase::_Deallocate,                                    \
    ScriptablePluginObjectBase::_Invalidate,                                    \
    ScriptablePluginObjectBase::_HasMethod,                                     \
    ScriptablePluginObjectBase::_Invoke,                                        \
    ScriptablePluginObjectBase::_InvokeDefault,                                 \
    ScriptablePluginObjectBase::_HasProperty,                                   \
    ScriptablePluginObjectBase::_GetProperty,                                   \
    ScriptablePluginObjectBase::_SetProperty,                                   \
    ScriptablePluginObjectBase::_RemoveProperty,                                \
    ScriptablePluginObjectBase::_Enumerate,                                     \
    ScriptablePluginObjectBase::_Construct                                      \
}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass

void
ScriptablePluginObjectBase::Invalidate()
{
}

bool
ScriptablePluginObjectBase::HasMethod(NPIdentifier name)
{
    return false;
}

bool
ScriptablePluginObjectBase::Invoke(NPIdentifier name, const NPVariant *args,
                                   uint32_t argCount, NPVariant *result)
{
    return false;
}

bool
ScriptablePluginObjectBase::InvokeDefault(const NPVariant *args,
                                          uint32_t argCount, NPVariant *result)
{
    return false;
}

bool
ScriptablePluginObjectBase::HasProperty(NPIdentifier name)
{
    return false;
}

bool
ScriptablePluginObjectBase::GetProperty(NPIdentifier name, NPVariant *result)
{
    return false;
}

bool
ScriptablePluginObjectBase::SetProperty(NPIdentifier name,
                                        const NPVariant *value)
{
    return false;
}

bool
ScriptablePluginObjectBase::RemoveProperty(NPIdentifier name)
{
    return false;
}

bool
ScriptablePluginObjectBase::Enumerate(NPIdentifier **identifier,
                                      uint32_t *count)
{
    return false;
}

bool
ScriptablePluginObjectBase::Construct(const NPVariant *args, uint32_t argCount,
                                      NPVariant *result)
{
    return false;
}

// static
void
ScriptablePluginObjectBase::_Deallocate(NPObject *npobj)
{
    // Call the virtual destructor.
    delete (ScriptablePluginObjectBase *)npobj;
}

// static
void
ScriptablePluginObjectBase::_Invalidate(NPObject *npobj)
{
    ((ScriptablePluginObjectBase *)npobj)->Invalidate();
}

// static
bool
ScriptablePluginObjectBase::_HasMethod(NPObject *npobj, NPIdentifier name)
{
    return ((ScriptablePluginObjectBase *)npobj)->HasMethod(name);
}

// static
bool
ScriptablePluginObjectBase::_Invoke(NPObject *npobj, NPIdentifier name,
                                    const NPVariant *args, uint32_t argCount,
                                    NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->Invoke(name, args, argCount,
                                                       result);
}

// static
bool
ScriptablePluginObjectBase::_InvokeDefault(NPObject *npobj,
                                           const NPVariant *args,
                                           uint32_t argCount,
                                           NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->InvokeDefault(args, argCount,
                                                              result);
}

// static
bool
ScriptablePluginObjectBase::_HasProperty(NPObject * npobj, NPIdentifier name)
{
    return ((ScriptablePluginObjectBase *)npobj)->HasProperty(name);
}

// static
bool
ScriptablePluginObjectBase::_GetProperty(NPObject *npobj, NPIdentifier name,
                                         NPVariant *result)
{
    return ((ScriptablePluginObjectBase *)npobj)->GetProperty(name, result);
}

// static
bool
ScriptablePluginObjectBase::_SetProperty(NPObject *npobj, NPIdentifier name,
                                         const NPVariant *value)
{
    return ((ScriptablePluginObjectBase *)npobj)->SetProperty(name, value);
}

// static
bool
ScriptablePluginObjectBase::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    return ((ScriptablePluginObjectBase *)npobj)->RemoveProperty(name);
}

// static
bool
ScriptablePluginObjectBase::_Enumerate(NPObject *npobj,
                                       NPIdentifier **identifier,
                                       uint32_t *count)
{
    return ((ScriptablePluginObjectBase *)npobj)->Enumerate(identifier, count);
}

// static
bool
ScriptablePluginObjectBase::_Construct(NPObject *npobj, const NPVariant *args,
                                       uint32_t argCount, NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->Construct(args, argCount,
                                                          result);
}


class ConstructablePluginObject : public ScriptablePluginObjectBase
{
public:
  ConstructablePluginObject(NPP npp)
    : ScriptablePluginObjectBase(npp)
  {
  }

  virtual bool Construct(const NPVariant *args, uint32_t argCount,
                         NPVariant *result);
};

static NPObject *
AllocateConstructablePluginObject(NPP npp, NPClass *aClass)
{
  return new ConstructablePluginObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ConstructablePluginObject,
                                 AllocateConstructablePluginObject);
bool
ConstructablePluginObject::Construct(const NPVariant *args, uint32_t argCount,
                                     NPVariant *result)
{
    printf("Creating new ConstructablePluginObject!\n");

    NPObject *myobj =
        NPN_CreateObject(mNpp, GET_NPOBJECT_CLASS(ConstructablePluginObject));
    if (!myobj)
        return false;

    OBJECT_TO_NPVARIANT(myobj, *result);
    return true;
}

class ScriptablePluginObject : public ScriptablePluginObjectBase
{
public:
    ScriptablePluginObject(NPP npp);
    virtual ~ScriptablePluginObject();

    virtual bool HasMethod(NPIdentifier name);
    virtual bool HasProperty(NPIdentifier name);
    virtual bool GetProperty(NPIdentifier name, NPVariant *result);
    virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);
  virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                             NPVariant *result);
};

static NPObject *
AllocateScriptablePluginObject(NPP npp, NPClass *aClass)
{
    return new ScriptablePluginObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ScriptablePluginObject,
                                 AllocateScriptablePluginObject);
								 
ScriptablePluginObject::ScriptablePluginObject(NPP npp) : ScriptablePluginObjectBase(npp)
{
    // Get the instance of the browser window object
    NPN_GetValue(npp, NPNVWindowNPObject, &sWindowObj);

    // initialize a temporary npid.
    NPIdentifier n = NPN_GetStringIdentifier("foof");

    // setup the properties for the plugin.  These id's are unique to the plugin and assigned
    // by the browser. npids can be eiter properties or methods.
    sFoo_id = NPN_GetStringIdentifier("foo");
    sBar_id = NPN_GetStringIdentifier("bar");

    // define some npids for HTML elements that we commonly use.
    sDocument_id = NPN_GetStringIdentifier("document");
    sBody_id = NPN_GetStringIdentifier("body");

    // these are the element npid used to create new elements to be appended to the dom
    sCreateElement_id = NPN_GetStringIdentifier("createElement");
    sCreateTextNode_id = NPN_GetStringIdentifier("createTextNode");

    // used to append to the loaded dom
    sAppendChild_id = NPN_GetStringIdentifier("appendChild");

    // required: used in the construction of the plugin
    sPluginType_id = NPN_GetStringIdentifier("PluginType");

}

ScriptablePluginObject::~ScriptablePluginObject()
{
    if (sWindowObj)
        NPN_ReleaseObject(sWindowObj);
}

bool
ScriptablePluginObject::HasMethod(NPIdentifier name)
{
    return name == sFoo_id;
}

bool
ScriptablePluginObject::HasProperty(NPIdentifier name)
{
  return (name == sBar_id ||
          name == sPluginType_id);
}

bool
ScriptablePluginObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    VOID_TO_NPVARIANT(*result);

    if (name == sBar_id) {
        static int a = 17;

        INT32_TO_NPVARIANT(a, *result);

        a += 5;
        return true;
    }

    if (name == sPluginType_id) {
        NPObject *myobj =  NPN_CreateObject(mNpp, GET_NPOBJECT_CLASS(ConstructablePluginObject));
        if (!myobj) {
            return false;
        }

        OBJECT_TO_NPVARIANT(myobj, *result);

        return true;
    }

    return true;
}

bool ScriptablePluginObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
    if (name == sBar_id) {
        printf ("bar set\n");
        return true;
    }

    return false;
}

bool
ScriptablePluginObject::Invoke(NPIdentifier name, const NPVariant *args,
                               uint32_t argCount, NPVariant *result)
{
    if (name == sFoo_id) {
        printf ("foo called!\n");

        // get the main document from the window object.
        NPVariant docv;
        NPN_GetProperty(mNpp, sWindowObj, sDocument_id, &docv);

        // convert the variant into a NPObject
        NPObject *doc = NPVARIANT_TO_OBJECT(docv);

        // insert a DIV tag into the document
        NPVariant strv;
        // create the element.
        NPUTF8 *putf8Str = (NPUTF8 *)NPN_MemAlloc(strlen("div")+1);
        strncpy(putf8Str, "div", strlen("div"));

        STRINGZ_TO_NPVARIANT(putf8Str, strv);

        // Create the div tag object that will be inserted into the DOM.
        NPVariant divv;
        NPN_Invoke(mNpp, doc, sCreateElement_id, &strv, 1, &divv);

        // now put some text into the div tag
        // first create the string.
        putf8Str = (NPUTF8 *)NPN_MemAlloc(strlen("I'm created by a plugin!")+1);
        strncpy(putf8Str, "I'm created by a plugin!", strlen("I'm created by a plugin!"));
        STRINGZ_TO_NPVARIANT(putf8Str, strv);

        // now insert it into the DIV tag we just added.
        NPVariant textv;
        NPN_Invoke(mNpp, doc, sCreateTextNode_id, &strv, 1, &textv);

        NPVariant v;
        NPN_Invoke(mNpp, NPVARIANT_TO_OBJECT(divv), sAppendChild_id, &textv, 1, &v);
        NPN_ReleaseVariantValue(&v);
        NPN_ReleaseVariantValue(&textv);

        // now insert the DIV tag into the body of the loaded HTML document.
        NPVariant bodyv;

        // get the body element as a NPObject
        NPN_GetProperty(mNpp, doc, sBody_id, &bodyv);

        // Append the new div tag into the DOM
        NPN_Invoke(mNpp, NPVARIANT_TO_OBJECT(bodyv), sAppendChild_id, &divv, 1, &v);
        NPN_ReleaseVariantValue(&v);
        NPN_ReleaseVariantValue(&divv);
        NPN_ReleaseVariantValue(&bodyv);

        NPN_ReleaseVariantValue(&docv);

        // allocate a return value as a string.  
        NPUTF8 *putf8RetStr = (NPUTF8 *)NPN_MemAlloc(strlen("foo return val")+1);
        strncpy(putf8RetStr, "foo return val", strlen("foo return val"));
        STRINGZ_TO_NPVARIANT(putf8RetStr, *result);
        return true;
    }

    return false;
}

bool
ScriptablePluginObject::InvokeDefault(const NPVariant *args, uint32_t argCount,
                                      NPVariant *result)
{
    printf ("ScriptablePluginObject default method called!\n");

    // this is crashing because we are not using the correct memory heap.
    // Any string or object allocated for a return value MUST be allocated off
    // the browser's heap.
    //STRINGZ_TO_NPVARIANT(strdup("default method return val"), *result);

    NPUTF8 *putf8RetStr = (NPUTF8 *)NPN_MemAlloc(strlen("default method return val")+1);
    strncpy(putf8RetStr, "default method return val", strlen("default method return val"));
    STRINGZ_TO_NPVARIANT(putf8RetStr, *result);

    return true;
}

CPlugin::CPlugin(NPP pNPInstance) :
m_pNPInstance(pNPInstance),
m_pNPStream(NULL),
m_bInitialized(FALSE),
m_pScriptableObject(NULL)
{
    NPVariant v;
    NPVariant rval;
    NPObject *browserWndObj = NULL;


#ifdef XP_WIN
    m_hWnd = NULL;
#endif
    // setup the display string
    const char *ua = NPN_UserAgent(m_pNPInstance);
    strcpy(m_String, ua);

    if ( NPERR_NO_ERROR == NPN_GetValue(m_pNPInstance, NPNVWindowNPObject, &browserWndObj))
    {
        // this is interesting.  
        // Here we are setting up an instance in JavaScript of the ScriptablePluginObject
        // by inserting a new instance into the DOM.  this will be referenced by document.pluginobj;
        //
        NPObject *myobj = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(ScriptablePluginObject));

        if ( myobj )
        {
            // create an identifier for the plugin object
            NPIdentifier n = NPN_GetStringIdentifier("pluginobj");

            // now insert into the DOM
            OBJECT_TO_NPVARIANT(myobj, v);
            if (NPN_SetProperty(m_pNPInstance, browserWndObj, n, &v))
            {
                // validate the object
                if ( NPN_GetProperty(m_pNPInstance, browserWndObj, n, &rval) )
                {
                    printf ("Object set/get test ");

                    if (NPVARIANT_IS_OBJECT(rval) && NPVARIANT_TO_OBJECT(rval) == myobj) {
                        printf ("succeeded!\n");
                    } else {
                        printf ("FAILED!\n");
                    }
                }
                else printf("failed to get pluginobj");

                NPN_ReleaseVariantValue(&rval);
            }
            else printf("failed to set pluginobj");

            NPN_ReleaseObject(myobj);
        }
        else printf("failed to create pluginobj");
    }
    else printf("failed to get browser window");
}

CPlugin::~CPlugin()
{
    if (m_pScriptableObject)
        NPN_ReleaseObject(m_pScriptableObject);
}

#ifdef XP_WIN
static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static WNDPROC lpOldProc = NULL;
#endif

NPBool CPlugin::init(NPWindow* pNPWindow)
{
    if(pNPWindow == NULL)
        return FALSE;

#ifdef XP_WIN
    m_hWnd = (HWND)pNPWindow->window;
    if(m_hWnd == NULL)
        return FALSE;

    // subclass window so we can intercept window messages and
    // do our drawing to it
    lpOldProc = SubclassWindow(m_hWnd, (WNDPROC)PluginWinProc);

    // associate window with our CPlugin object so we can access 
    // it in the window procedure
    SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
#endif

    m_Window = pNPWindow;

    m_bInitialized = TRUE;
    return TRUE;
}

void CPlugin::shut()
{
#ifdef XP_WIN
    // subclass it back
    SubclassWindow(m_hWnd, lpOldProc);
    m_hWnd = NULL;
#endif

    m_bInitialized = FALSE;
}

NPBool CPlugin::isInitialized()
{
    return m_bInitialized;
}

int16_t CPlugin::handleEvent(void* event)
{
#ifdef XP_MAC
    NPEvent* ev = (NPEvent*)event;
    if (m_Window) {
        Rect box = { m_Window->y, m_Window->x,
            m_Window->y + m_Window->height, m_Window->x + m_Window->width };
        if (ev->what == updateEvt) {
            ::TETextBox(m_String, strlen(m_String), &box, teJustCenter);
        }
    }
#endif
    return 0;
}

// this will force to draw a version string in the plugin window
void CPlugin::showVersion()
{
    const char *ua = NPN_UserAgent(m_pNPInstance);
    strcpy(m_String, ua);

#ifdef XP_WIN
    InvalidateRect(m_hWnd, NULL, TRUE);
    UpdateWindow(m_hWnd);
#endif

    if (m_Window) {
        NPRect r =
        {
            (uint16_t)m_Window->y, (uint16_t)m_Window->x,
            (uint16_t)(m_Window->y + m_Window->height),
            (uint16_t)(m_Window->x + m_Window->width)
        };

        NPN_InvalidateRect(m_pNPInstance, &r);
    }
}

// this will clean the plugin window
void CPlugin::clear()
{
    strcpy(m_String, "");

#ifdef XP_WIN
    InvalidateRect(m_hWnd, NULL, TRUE);
    UpdateWindow(m_hWnd);
#endif
}


void CPlugin::getVersion(char* *aVersion)
{
    const char *ua = NPN_UserAgent(m_pNPInstance);
    char*& version = *aVersion;
    version = (char*)NPN_MemAlloc(1 + strlen(ua));
    if (version)
        strcpy(version, ua);
}


NPObject *CPlugin::GetScriptableObject()
{
    if (!m_pScriptableObject) {
        m_pScriptableObject = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(ScriptablePluginObject));
    }

    if (m_pScriptableObject) {
        NPN_RetainObject(m_pScriptableObject);
    }

    return m_pScriptableObject;
}


#ifdef XP_WIN
static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT:
        {
            // draw a frame and display the string
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);

            HBRUSH hbr = CreateSolidBrush(RGB(rand()%255, rand()%255, rand()%255));
            FillRect(hdc, &rc, hbr);
            FrameRect(hdc, &rc, GetStockBrush(DKGRAY_BRUSH));

            CPlugin * p = (CPlugin *)GetWindowLong(hWnd, GWL_USERDATA);
            if(p) {
                if (p->m_String[0] == 0) {
                    strcpy("foo", p->m_String);
                }

                DrawTextA(hdc, p->m_String, strlen(p->m_String), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }

            DeleteObject(hbr);
            EndPaint(hWnd, &ps);
        }
        break;
    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
#endif
