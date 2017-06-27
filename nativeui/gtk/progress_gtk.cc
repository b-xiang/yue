// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/progress.h"

#include <gtk/gtk.h>

#include "nativeui/gtk/widget_util.h"

namespace nu {

namespace {

void DeleteTimer(void* data) {
  guint timer = *reinterpret_cast<guint*>(&data);
  g_source_remove(timer);
}

gboolean OnTimer(GtkWidget* widget) {
  gtk_progress_bar_pulse(GTK_PROGRESS_BAR(widget));
  return G_SOURCE_CONTINUE;
}

}  // namespace

Progress::Progress() {
  TakeOverView(gtk_progress_bar_new());
  SetDefaultStyle(SizeF(GetPreferredSizeForWidget(GetNative())));
}

Progress::~Progress() {
}

void Progress::SetValue(float value) {
  SetIndeterminate(false);
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GetNative()), value / 100.);
}

float Progress::GetValue() const {
  return gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(GetNative())) * 100.;
}

void Progress::SetIndeterminate(bool indeterminate) {
  bool is_indeterminate = IsIndeterminate();
  if (indeterminate == is_indeterminate)
    return;
  if (indeterminate) {
    guint timer = g_timeout_add(100, reinterpret_cast<GSourceFunc>(OnTimer),
                                GetNative());
    g_object_set_data_full(G_OBJECT(GetNative()), "timer",
                           reinterpret_cast<void*>(timer), DeleteTimer);
  } else {
    g_object_set_data(G_OBJECT(GetNative()), "timer", nullptr);
  }
}

bool Progress::IsIndeterminate() const {
  return g_object_get_data(G_OBJECT(GetNative()), "timer");
}

}  // namespace nu
