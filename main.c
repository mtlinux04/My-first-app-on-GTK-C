#include <gtk/gtk.h>

#include "main.h"

// If "x" button is pressed on dialog window, then destroy it
void on_dialog_destroy(GtkWidget *widget, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;
    data->dialog = NULL;
}

void rename_task(GtkEditable *editable, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    GtkListBoxRow *row = gtk_list_box_get_selected_row(data->listbox);
    GtkWidget *child_outer_box = gtk_list_box_row_get_child(row);
    GtkWidget *child_inner_box = gtk_widget_get_first_child(GTK_WIDGET(child_outer_box));
    GtkWidget *child_checkbox = gtk_widget_get_first_child(GTK_WIDGET(child_inner_box));
    GtkWidget *child_label = gtk_widget_get_next_sibling(GTK_WIDGET(child_checkbox));

    const char *new = gtk_editable_get_text(GTK_EDITABLE(data->entry));
    gtk_label_set_text(GTK_LABEL(child_label), new);
    gtk_window_destroy(GTK_WINDOW(data->dialog));
}

void on_entry_button_active(GtkEntry *entry, gpointer user_data) {
    GtkButton *btn = GTK_BUTTON(user_data);
    gboolean is_active = gtk_entry_get_text_length(entry) == 0 ? FALSE : TRUE;
    gtk_widget_set_sensitive(GTK_WIDGET(btn), is_active);
}

void rename_task_dialog(GSimpleAction *action, GVariant *variant, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    data->dialog = gtk_window_new();
    g_signal_connect(data->dialog, "destroy", G_CALLBACK(on_dialog_destroy), data);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    // New text enter to update task name
    GtkWidget *entry = gtk_entry_new();
    data->entry = GTK_ENTRY(entry);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter your task name");

    gtk_box_append(GTK_BOX(box), entry);

    GtkWidget *tickBtn = gtk_button_new();
    gtk_button_set_icon_name(GTK_BUTTON(tickBtn), "dino-tick-symbolic");

    gtk_box_append(GTK_BOX(box), tickBtn);

    gtk_window_set_child(GTK_WINDOW(data->dialog), box);

    g_signal_connect(entry, "changed", G_CALLBACK(on_entry_button_active), tickBtn); // If text is empty, disable btn
    g_signal_connect(tickBtn, "clicked", G_CALLBACK(rename_task), data);

    if (data->dialog != NULL) {
        gtk_window_present(GTK_WINDOW(data->dialog));
        return;
    }
}

void complete_task(GObject *object, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;
    GtkCheckButton *checkbox = GTK_CHECK_BUTTON(object);

    if (gtk_check_button_get_active(checkbox)) {
        GtkListBoxRow *row = GTK_LIST_BOX_ROW(user_data);
        GtkWidget *list = gtk_widget_get_parent(GTK_WIDGET(row));

        gtk_list_box_select_row(GTK_LIST_BOX(list), row);
        gtk_list_box_remove(GTK_LIST_BOX(list), GTK_WIDGET(row));
    }
}

gboolean filter_list(GtkListBoxRow *row, gpointer user_data) {
    GtkWidget *child_outer_box = gtk_list_box_row_get_child(row);
    GtkWidget *child_inner_box = gtk_widget_get_first_child(child_outer_box);
    GtkWidget *child_checkbox = gtk_widget_get_first_child(child_inner_box);
    GtkWidget *child_label = gtk_widget_get_next_sibling(child_checkbox);

    const char* row_text = gtk_label_get_text(GTK_LABEL(child_label));
    char* term = (char *)user_data;
    return strstr(row_text, term) ? TRUE : FALSE;
}

void on_search_entry(GtkEditable *editable, gpointer user_data) {
    SearchData *self = (SearchData *)user_data;
    char* text = gtk_editable_get_text(editable);

    if (text)
        gtk_list_box_set_filter_func(self->search_listbox, filter_list, text, NULL);
}

void search_dialog(GtkButton *btn, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    if (data->dialog != NULL) {
        gtk_window_present(GTK_WINDOW(data->dialog));
        return;
    }

    data->dialog = gtk_window_new();
    g_signal_connect(data->dialog, "destroy", G_CALLBACK(on_dialog_destroy), data);

    gtk_window_set_title(GTK_WINDOW(data->dialog), "Search");
    gtk_window_set_default_size(GTK_WINDOW(data->dialog), 200, 150);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(data->dialog), box);

    data->entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->entry), "Enter the text find");

    gtk_box_append(GTK_BOX(box), GTK_WIDGET(data->entry));

    GtkWidget *listbox = gtk_list_box_new();
    SearchData *self = g_new0(SearchData, 1);
    self->search_listbox = GTK_LIST_BOX(listbox);

    /// **************************************** ///
    GtkListBoxRow *row;
    int i = 0;

    while ((row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(data->listbox), i++))) {
        GtkWidget *child_outer_box = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row));
        GtkWidget *child_inner_box = gtk_widget_get_first_child(child_outer_box);
        GtkWidget *child_checkbox = gtk_widget_get_first_child(child_inner_box);
        GtkWidget *child_label = gtk_widget_get_next_sibling(child_checkbox);

        // Creating new widgets for search listbox
        GtkWidget *row_current = gtk_list_box_row_new();
        GtkWidget *checkbox = gtk_check_button_new();
        GtkWidget *label = gtk_label_new(NULL);
        const char* row_text = gtk_label_get_text(GTK_LABEL(child_label));

        gtk_widget_set_sensitive(GTK_WIDGET(checkbox), FALSE);
        gtk_label_set_text(GTK_LABEL(label), row_text);

        // Creating boxes for placing widgets
        GtkWidget *box_row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_halign(box_row, GTK_ALIGN_FILL);
        gtk_widget_set_hexpand(box_row, TRUE);

        GtkWidget *box_row_task = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_append(GTK_BOX(box_row), box_row_task);

        gtk_box_append(GTK_BOX(box_row_task), checkbox);
        gtk_box_append(GTK_BOX(box_row_task), label);

        // 1111111111111
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row_current), box_row);
        gtk_list_box_append(self->search_listbox, row_current);
    }

    gtk_box_append(GTK_BOX(box), GTK_WIDGET(self->search_listbox));

    g_signal_connect(data->entry, "changed", G_CALLBACK(on_search_entry), self);
    /// **************************************** ///

    gtk_window_present(GTK_WINDOW(data->dialog));
}

void delete_task_confirmed(GtkButton *btn, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    if (data->row_to_delete != NULL)
        gtk_list_box_remove(data->listbox, GTK_WIDGET(data->row_to_delete));

    if (data->dialog) {
        gtk_window_destroy(GTK_WINDOW(data->dialog));
        data->dialog = NULL;
        data->row_to_delete = NULL;
    }
}

void delete_task_cancelled(GtkButton *btn, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    if (data->dialog) {
        gtk_window_destroy(GTK_WINDOW(data->dialog));
        data->dialog = NULL;
        data->row_to_delete = NULL;
    }
}

void delete_task_dialog(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    if (data->dialog != NULL) {
        gtk_window_present(GTK_WINDOW(data->dialog));
        return;
    }

    GtkListBoxRow *selected = gtk_list_box_get_selected_row(data->listbox);

    data->row_to_delete = selected;

    data->dialog = gtk_window_new();
    g_signal_connect(data->dialog, "destroy", G_CALLBACK(on_dialog_destroy), data);

    gtk_widget_set_size_request(data->dialog, 300, 250);

    GtkWidget *box_label = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *label = gtk_label_new("Are you sure, you want to delete the task?");

    gtk_box_append(GTK_BOX(box_label), label);

    GtkWidget *box_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    gtk_box_append(GTK_BOX(box_label), box_buttons);

    GtkWidget *yesBtn = gtk_button_new_with_label("Yes");
    GtkWidget *noBtn = gtk_button_new_with_label("No");

    gtk_box_append(GTK_BOX(box_buttons), yesBtn);
    gtk_box_append(GTK_BOX(box_buttons), noBtn);

    gtk_widget_set_valign(box_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(box_buttons, GTK_ALIGN_CENTER);

    g_signal_connect(yesBtn, "clicked", G_CALLBACK(delete_task_confirmed), data);
    g_signal_connect(noBtn, "clicked", G_CALLBACK(delete_task_cancelled), data);

    gtk_window_set_child(GTK_WINDOW(data->dialog), box_label);
    gtk_window_present(GTK_WINDOW(data->dialog));
}

void on_menu_button_active(GObject *object, GParamSpec *pspec, gpointer user_data) {
    GtkMenuButton *menuBtn = GTK_MENU_BUTTON(object);

    if (gtk_menu_button_get_active(menuBtn)) {
        GtkListBoxRow *row = GTK_LIST_BOX_ROW(user_data);
        GtkWidget *listbox = gtk_widget_get_parent(GTK_WIDGET(row));

        if (GTK_IS_LIST_BOX(listbox)) {
            gtk_list_box_select_row(GTK_LIST_BOX(listbox), row);
            gtk_widget_grab_focus(GTK_WIDGET(row));
        }
    }
}

void add_task(GtkButton *btn, gpointer user_data) {
    TaskData *data = (TaskData *)user_data;

    if (!data || !data->entry || !data->listbox)
        return;

    const char *rowText = gtk_editable_get_text(GTK_EDITABLE(data->entry));

    if (g_utf8_validate(rowText, -1, NULL) && (g_utf8_strlen(rowText, -1)) > 0) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *checkbox = gtk_check_button_new();
        GtkWidget *newLabel = gtk_label_new(rowText);

        GtkWidget *box_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_halign(box_row, GTK_ALIGN_FILL);

        GtkWidget *box_row_task = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_hexpand(box_row_task, TRUE);
        gtk_box_append(GTK_BOX(box_row), box_row_task);

        gtk_box_append(GTK_BOX(box_row_task), checkbox);
        gtk_box_append(GTK_BOX(box_row_task), newLabel);

        GtkWidget *box_row_menu = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_append(GTK_BOX(box_row), box_row_menu);

        GtkWidget *menuBtn = gtk_menu_button_new();
        gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menuBtn), "view-more-symbolic");
        gtk_widget_set_halign(menuBtn, GTK_ALIGN_END);

        gtk_box_append(GTK_BOX(box_row_menu), menuBtn);

        GSimpleActionGroup *actions = g_simple_action_group_new();
        GSimpleAction *delete = g_simple_action_new("delete", NULL);
        GSimpleAction *rename = g_simple_action_new("rename", NULL);

        g_signal_connect(delete, "activate", G_CALLBACK(delete_task_dialog), data);
        g_signal_connect(rename, "activate", G_CALLBACK(rename_task_dialog), data);

        g_action_map_add_action(G_ACTION_MAP(actions), G_ACTION(delete));
        g_action_map_add_action(G_ACTION_MAP(actions), G_ACTION(rename));

        gtk_widget_insert_action_group(row, "row", G_ACTION_GROUP(actions));

        g_object_unref(actions);

        GMenu *menu = g_menu_new();

        g_menu_append(menu, "Delete", "row.delete");
        g_menu_append(menu, "Rename", "row.rename");

        gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menuBtn), G_MENU_MODEL(menu));
        g_object_unref(menu);

        g_signal_connect(checkbox, "toggled", G_CALLBACK(complete_task), row);
        g_signal_connect(menuBtn, "notify::active", G_CALLBACK(on_menu_button_active), row);

        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box_row);
        gtk_list_box_append(data->listbox, row);
    }

    gtk_editable_set_text(GTK_EDITABLE(data->entry), "");

    if (data->dialog) {
        gtk_window_destroy(GTK_WINDOW(data->dialog));
        data->dialog = NULL;
    }
}

void add_task_dialog(GtkButton *btn, gpointer user_data) {
    TaskData *data = (TaskData *) user_data;

    if (data->dialog != NULL) {
        gtk_window_present(GTK_WINDOW(data->dialog));
        return;
    }

    data->dialog = gtk_window_new();
    g_signal_connect(data->dialog, "destroy", G_CALLBACK(on_dialog_destroy), data);

    gtk_widget_set_size_request(data->dialog, 200, 150);
    gtk_window_set_title(GTK_WINDOW(data->dialog), "Add task");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *entry = gtk_entry_new();
    data->entry = GTK_ENTRY(entry);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter the text");

    GtkWidget *addnewtaskBtn = gtk_button_new_with_label("Add new task");

    gtk_box_append(GTK_BOX(box), entry);
    gtk_box_append(GTK_BOX(box), addnewtaskBtn);

    g_signal_connect(addnewtaskBtn, "clicked", G_CALLBACK(add_task), data);

    gtk_window_set_child(GTK_WINDOW(data->dialog), box);
    gtk_window_present(GTK_WINDOW(data->dialog));
}

void app_activate(GApplication *app, gpointer *user_data) {
    GtkWidget *window = gtk_application_window_new (GTK_APPLICATION (app));
    gtk_window_set_title (GTK_WINDOW (window), "METANIT.COM");
    gtk_window_set_default_size (GTK_WINDOW (window), 300, 250);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_append(GTK_BOX(box), box_left);
    gtk_box_append(GTK_BOX(box), box_right);

    GtkWidget *addtaskBtn = gtk_button_new_with_label("Add task");
    GtkWidget *todayBtn = gtk_button_new_with_label("Today");
    GtkWidget *inboxBtn = gtk_button_new_with_label("Inbox");

    GtkWidget *entry = gtk_entry_new();

    gtk_box_append(GTK_BOX(box_left), addtaskBtn);
    gtk_box_append(GTK_BOX(box_left), todayBtn);
    gtk_box_append(GTK_BOX(box_left), inboxBtn);

    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    GtkWidget *searchBtn = gtk_button_new_from_icon_name("folder-saved-search-symbolic");

    gtk_box_append(GTK_BOX(toolbar), searchBtn);

    GtkWidget *tasks = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *listbox = gtk_list_box_new();
    gtk_widget_set_hexpand(listbox, TRUE);

    gtk_box_append(GTK_BOX(tasks), listbox);

    gtk_box_append(GTK_BOX(box_right), toolbar);
    gtk_box_append(GTK_BOX(box_right), tasks);

    TaskData *data = g_new0(TaskData, 1);
    data->listbox = GTK_LIST_BOX(listbox);

    g_signal_connect(addtaskBtn, "clicked", G_CALLBACK(add_task_dialog), data);
    g_signal_connect(searchBtn, "clicked", G_CALLBACK(search_dialog), data);

    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
