/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
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
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <string.h>
#include "plugin.h"
#include "pypplets.h"

#define MIME_TYPES_HANDLED  "application/x-python"
#define PLUGIN_NAME         "Pypplets - Python Applets for Gecko"
#define MIME_TYPES_DESCRIPTION  MIME_TYPES_HANDLED":pypplets:"PLUGIN_NAME


#ifdef BUILD_ID
String SHOW = "NEW STUFF";
#else
String SHOW="HELLO STUFF";
#endif

#define PLUGIN_DESCRIPTION  PLUGIN_NAME " (Pypplets Gecko Plug-in)"


char* NPP_GetMIMEDescription(void)
{
  printf("CALLED --------------------1\n");
  return(MIME_TYPES_DESCRIPTION);
}

/////////////////////////////////////
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
}

// get values per plugin
NPError NS_PluginGetValue(NPPVariable aVariable, void *aValue)
{
  NPError err = NPERR_NO_ERROR;
  switch (aVariable) {
    case NPPVpluginNameString:
      *((char **)aValue) = PLUGIN_NAME;
      break;
    case NPPVpluginDescriptionString:
      *((char **)aValue) = PLUGIN_DESCRIPTION;
      break;
    default:
      err = NPERR_INVALID_PARAM;
      break;
  }
  return err;
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if(!aCreateDataStruct)
    return NULL;

  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct->instance);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if(aPlugin)
    delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE),
  mWindow(0),
  mXtwidget(0),
  mFontInfo(0)
{
  printf("Initializing");
  //script_py = (char *)malloc(sizeof(char) * 8192);
  //memset(script_py, 0, sizeof(char) * 8192);
  script_py = new std::string();
  
  
  python_version = (char *)malloc(sizeof(char) * 20);
  strcpy(python_version, "Python: ");
  strncat(python_version, Py_GetVersion(), 5);
}


nsPluginInstance::~nsPluginInstance()
{
}

static void
xt_event_handler(Widget xtwidget, nsPluginInstance *plugin, XEvent *xevent, Boolean *b)
{
  switch (xevent->type) {
    case Expose:
      // get rid of all other exposure events
      if (plugin) {
        //while(XCheckTypedWindowEvent(plugin->Display(), plugin->Window(), Expose, xevent));
        plugin->draw();
      }
      default:
        break;
  }
}

void nsPluginInstance::draw()
{
  unsigned int h = mHeight/2;
  unsigned int w = 3 * mWidth/4;
  int x = (mWidth - w)/2; // center
  int y = h/2;
  if (x >= 0 && y >= 0) {
    GC gc = XCreateGC(mDisplay, mWindow, 0, NULL);
    if (!gc) 
      return;
    XDrawRectangle(mDisplay, mWindow, gc, x, y, w, h);
  const char *string = getVersion();

  if (string && *string) {
    int l = strlen(string);
    int fmba = mFontInfo->max_bounds.ascent;
    int fmbd = mFontInfo->max_bounds.descent;
    int fh = fmba + fmbd;
    y += fh;
    x += 32;
      XDrawString(mDisplay, mWindow, gc, x, y, string, l); 
    }
    XFreeGC(mDisplay, gc);
  }
}

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
  if(aWindow == NULL)
    return FALSE;
  
  if (SetWindow(aWindow))
  mInitialized = TRUE;
	
  return mInitialized;
}

void nsPluginInstance::shut()
{
  mInitialized = FALSE;
}

const char * nsPluginInstance::getVersion()
{
  //return NPN_UserAgent(mInstance);
  printf("Using Python Version: %s\n", python_version);
  return SHOW;
//  return python_version;
}

NPError nsPluginInstance::GetValue(NPPVariable aVariable, void *aValue)
{
  NPError err = NPERR_NO_ERROR;

  switch (aVariable) {
    case NPPVpluginNameString:
    case NPPVpluginDescriptionString:
      return NS_PluginGetValue(aVariable, aValue) ;
      break;
    default:
      err = NPERR_INVALID_PARAM;
      break;
  }
  return err;

}

NPError nsPluginInstance::SetWindow(NPWindow* aWindow)
{
  if(aWindow == NULL)
    return FALSE;

  /*mX = aWindow->x;
  mY = aWindow->y;
  mWidth = aWindow->width;
  mHeight = aWindow->height;
  if (mWindow == (Window) aWindow->window) {
    gWindow = gdk_window_foreign_new((Window)aWindow->window);
    // The page with the plugin is being resized.
    // Save any UI information because the next time
    // around expect a SetWindow with a new window id.
  } else {
    mWindow = (Window) aWindow->window;
    NPSetWindowCallbackStruct *ws_info = (NPSetWindowCallbackStruct *)aWindow->ws_info;
    gWindow = gdk_window_foreign_new_for_display((Window)aWindow->window);
    mDisplay = ws_info->display;
    mVisual = ws_info->visual;
    mDepth = ws_info->depth;
    mColormap = ws_info->colormap;

    if (!mFontInfo) {
      if (!(mFontInfo = XLoadQueryFont(mDisplay, "9x15")))
        printf("Cannot open 9X15 font\n");
    }
    // add xt event handler
    Widget xtwidget = XtWindowToWidget(mDisplay, mWindow);
    if (xtwidget && mXtwidget != xtwidget) {
      mXtwidget = xtwidget;
      long event_mask = ExposureMask;
      XSelectInput(mDisplay, mWindow, event_mask);
      XtAddEventHandler(xtwidget, event_mask, False, (XtEventHandler)xt_event_handler, this);
    }
  }
  draw();*/
  return TRUE;
}
//Streams
NPError nsPluginInstance::NewStream(NPMIMEType type, NPStream *stream, NPBool seekable, uint16* stype){
   printf("New Stream \n");
   script_py->clear();
   return NPERR_NO_ERROR;
}

int32 nsPluginInstance::Write(NPStream* stream, int32 offset, int32 len, void* buf){
	printf("WRITE FUNC\n");
	printf("WRITE_READY FUNC\n");
	printf("LOADING URL: %s\n", stream->url);
	printf("LEN: %d\n", len);
	script_py->append(std::string((char *)buf), 0, len - 1);
	printf("RET: %d\n", script_py->size());	
	return script_py->size();
}

int32 nsPluginInstance::WriteReady(NPStream* stream){
	return 8192;
}

NPError nsPluginInstance::DestroyStream(NPStream* stream, NPReason reason){
	printf("DESTROY: \n");
	printf("%s\n", script_py->c_str());
	if(reason == NPRES_DONE){
	   printf("Stream is Done\n");
	}
	
	//We Load Our Code
	Pypplets *pyp = Pypplets_Init();
	pyp->SetScript(script_py);
	pyp->TestRunCode();
	Pypplets_Destroy();
	
	return NPERR_NO_ERROR;
}
//End Streams
