// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_GTK_PLUGIN_CONTAINER_H_
#define CONTENT_BROWSER_RENDERER_HOST_GTK_PLUGIN_CONTAINER_H_

// Windowed plugins are embedded via XEmbed, which is implemented by
// GtkPlug/GtkSocket.  But we want to control sizing and positioning
// directly, so we need a subclass of GtkSocket that sidesteps the
// size_request handler.
//
// The custom size_request handler just reports the size set by
// gtk_plugin_container_set_size.

typedef struct _GtkWidget GtkWidget;


// Put this in the declarations for a class to be uncopyable.
#define DISALLOW_COPY(TypeName) \
  TypeName(const TypeName&) = delete

// Put this in the declarations for a class to be unassignable.
#define DISALLOW_ASSIGN(TypeName) TypeName& operator=(const TypeName&) = delete

// Put this in the declarations for a class to be uncopyable and unassignable.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  DISALLOW_COPY(TypeName);                 \
  DISALLOW_ASSIGN(TypeName)

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
// This is especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                           \
  DISALLOW_COPY_AND_ASSIGN(TypeName)


namespace content {

// Return a new GtkPluginContainer.
// Intentionally GTK-style here since we're creating a custom GTK widget.
// This is a GtkSocket subclass; see its documentation for available methods.
GtkWidget* gtk_plugin_container_new();

// Sets the size of the GtkPluginContainer.
void gtk_plugin_container_set_size(GtkWidget *widget, int width, int height);

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_GTK_PLUGIN_CONTAINER_H_
